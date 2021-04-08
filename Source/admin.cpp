#include "admin.h"
#include "ui_admin.h"

Admin::Admin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Admin)
{

    ui->setupUi(this);
    this->setWindowTitle("Админ");
    this->setFixedSize(1044,700);
    ui->blockedUsers->setAlignment(Qt::AlignCenter);
    ui->unverifiedUsers->setAlignment(Qt::AlignCenter);
    ui->totalMessages->setAlignment(Qt::AlignCenter);


    database = new Database();
    statistics = new Statistics(ui);

    on_getTable_clicked();
    statistics->getDonutPieChart();
    statistics->getVerticalBarChart();

    backUp = new QMenu();
    fullBackUp = new QAction("Полный бэкап");
    structBackUp = new QAction("Бэкап структуры");
    fullBackUp->setObjectName("fullBackup");
    structBackUp->setObjectName("structBackUp");

    backUp->addAction(fullBackUp);
    backUp->addAction(structBackUp);
    connect(fullBackUp, SIGNAL(triggered()), this, SLOT(slotBackup()));
    connect(structBackUp, SIGNAL(triggered()), this, SLOT(slotBackup()));
    ui->backUp->setMenu(backUp);

    import = new QMenu();
    overwiteImport = new QAction("Имопрт с перезаписью");
    addImport = new QAction("Импорт с дозаписью");
    overwiteImport->setObjectName("overwiteImport");
    addImport->setObjectName("addImport");

    import->addAction(overwiteImport);
    import->addAction(addImport);
    connect(overwiteImport, SIGNAL(triggered()), this, SLOT(slotImport()));
    connect(addImport, SIGNAL(triggered()), this, SLOT(slotImport()));

    ui->importButton->setMenu(import);
}

Admin::~Admin()
{
    delete ui;
    delete model;
    delete statistics;

}

void Admin::on_getTable_clicked()
{
    database->query.exec("select * from auth where login = '" + ui->lineEdit->text() + "'");
    database->query.first();
    database->queryRecord = database->query.record();
    statistics->acc_ID = database->queryRecord.value(0).toString();
    database->queryRecord.clear();

    model = new QSqlTableModel(this, database->getDatabase());
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    switch (ui->comboBox->currentIndex()) {
    case 0:
        model->setTable("auth");
        break;
    case 1:
        model->setTable("data");
        break;
    case 2:
        model->setTable("auth");
        model->setFilter("acc_ID = " + statistics->acc_ID);
        break;
    case 3:
        model->setTable("data");
        model->setFilter("acc_id = " + statistics->acc_ID);
        break;
    }

    if (!statistics->acc_ID.toInt() && ui->comboBox->currentIndex() > 1){
        QMessageBox::critical(this, "Пользователь не найден", "Пожалуйста, введите существующий логин.", QMessageBox::Ok);
    }
    else {
        model->select();
        ui->tableView->setModel(model);
        on_updatePage_clicked();
    }
}

void Admin::on_deleteSelectedRows_clicked()
{
    QItemSelection selectedRows = ui->tableView->selectionModel()->selection();
    QVector<int> selectedIndexes;

    foreach(const QModelIndex & index, selectedRows.indexes())
    {
        if (!(std::find(selectedIndexes.begin(), selectedIndexes.end(), index.row()) != selectedIndexes.end()))
        {
            selectedIndexes.append(index.row());
            model->removeRow(index.row());
        }
    }
}

void Admin::on_updatePage_clicked()
{
    updateTable();
    statistics->updateStats();
    statistics->getDonutPieChart();
    statistics->getVerticalBarChart();
}

void Admin::updateTable()
{
    model->select();

}

void Admin::on_submitChanges_clicked()
{
    model->submitAll();
    on_updatePage_clicked();
}

void Admin::on_unblockAll_clicked()
{
    if (model->tableName() == "auth") {
        model->setEditStrategy(QSqlTableModel::OnFieldChange);
        for (int i = 0; i < model->rowCount(); i++)
            model->setData(model->index(i, 3), 0, Qt::EditRole);
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    }
    else {
        database->query.exec("update auth set isBlocked = 0");
    }
}

void Admin::on_verifyAll_clicked()
{
    if (model->tableName() == "auth") {
        model->setEditStrategy(QSqlTableModel::OnFieldChange);
        for (int i = 0; i < model->rowCount(); i++)
            model->setData(model->index(i, 4), 1, Qt::EditRole);
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    }
    else {
        database->query.exec("update auth set isVerified = 1");
    }
}

void Admin::on_deleteAllMessages_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Внимание!", "Вы уверены, что хотите удалить все письма?",
                                                              QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (model->tableName() == "data"){
            model->removeRows(0, model->rowCount());
            model->submitAll();
        }
        else
            database->query.exec("DELETE FROM data WHERE msg_id;");
    }
}

QString Admin::on_backUp_clicked()
{
    QString path = QFileDialog::getExistingDirectory(
                this, "Open Dialog","");

    return path;
}

pair <QString,QString> Admin::intermediateImport()
{

    pair <QString, QString> toReturn;
    QString path = QFileDialog::getOpenFileName(
                this, "Open Dialog"," *.csv");
    if (path.length() == 0) return toReturn;


    path.push_front("cp ");


    std::string strPath = path.toStdString();

    strPath+=" /usr/local/mysql/data";
    const char* final = strPath.c_str();
    system(final);

    QString result;
    bool flag = false;
    for (auto i = path.end(); i >= path.begin(); --i){
        if (*i == '/'){
            while (i != path.end()) {
                result.push_back(*i);
                i++;
            }
            flag = true;
        }
        if (flag == true) break;
    }

    QString importStr = "LOAD DATA  INFILE '/usr/local/mysql/data" + result +
            "' INTO TABLE testdb.data "
            "FIELDS TERMINATED BY ';' "
            "ENCLOSED BY '''' "
            "LINES TERMINATED BY '\n' "
            "IGNORE 1 LINES "
            "(acc_id, themeMsg, fromWho,toWhom,dateMsg,textMsg)";


    toReturn.first = importStr;
    toReturn.second = result;

    return toReturn;
}

void Admin::slotImport()
{
    pair <QString, QString> intermeddiateValues = intermediateImport();

     if (intermeddiateValues.second.length() == 0) {
         qDebug()<<"BB2";
        return;
}

    if (QObject::sender()->objectName() == "overwiteImport")
    {
        database->query.exec("delete from data");
        database->query.clear();

    }


    database->query.exec(intermeddiateValues.first);
    std::string tmp = intermeddiateValues.second.toStdString();
    std::string rmStr = "rm /usr/local/mysql/data" + tmp;
    const char* rmChar = rmStr.c_str();
    system(rmChar);

}

void Admin::slotBackup()
{
    QString dumpStr = "/usr/local/mysql-8.0.21-macos10.15-x86_64/bin/mysqldump -uroot -p20643579 testdb ";
    QTime time = QTime::currentTime();
    QString tmpTime = time.toString();
    QString path = on_backUp_clicked();
    if(path.length()==0) return;



    if (QObject::sender()->objectName() == "structBackUp") dumpStr += "--no-data ";

    QString tmpStr = dumpStr + "> " + path + "/backup" + tmpTime + ".sql";
    QString tmpStr2 = dumpStr + "--xml >" +  path + "/backup" + tmpTime + ".xml";
    std::string str = tmpStr.toStdString();

    std::string str2 = tmpStr2.toStdString();
    const char* systemQuery = str.c_str();
    const char* systemQuery2 = str2.c_str();
    system(systemQuery2);
    system(systemQuery);

}


