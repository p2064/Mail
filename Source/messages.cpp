#include "messages.h"
#include "ui_messages.h"

Messages::Messages(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Messages)
{
    ui->setupUi(this);
    this->setWindowTitle("Сообщения");

    listWidget = new QListWidget(this);
    connect(this->listWidget,SIGNAL(itemClicked(QListWidgetItem *)),this,SLOT(display(QListWidgetItem *)));

    database = new Database();
    parser = new Parser();

    QFont fonts("Helvetica", 12, QFont::Bold);

    queryString = "select * from data where acc_id = ";

    listWidget->setFixedSize(350,500);
    this->setFixedSize(1000,700);
    listWidget->setFont(fonts);


    QMenu *file = new QMenu();
    QAction *delte = new QAction("Удалить аккаунт");
    QAction *block = new QAction("Заблокировать аккаунт");
    QAction *changePass = new QAction("Изменить пароль");

    file->addAction(delte);
    file->addAction(block);
    file->addAction(changePass);
    connect(block, SIGNAL(triggered()), this, SLOT(slotBlock()));
    connect(delte, SIGNAL(triggered()), this, SLOT(slotDelete()));
    connect(changePass, SIGNAL(triggered()), this, SLOT(changePassword()));
    ui->accSet->setMenu(file);
}

Messages::~Messages()
{

    delete ui;
}

void Messages::display (QListWidgetItem * msg)
{

    int counter=0;
    database->query.exec(queryString + QString::number(this->acc_id));
    database->queryRecord = database->query.record();
    while (database->query.next()) {
        counter++;
        if(counter==(listWidget->row(msg)+1))
            ui->textBrowser->setText(
                        database->query.value(database->queryRecord.indexOf("themeMsg")).toString()+
                        database->query.value(database->queryRecord.indexOf("fromWho")).toString() + "\n\n" +    // database->query.value(database->queryRecord.indexOf("toWhom")).toString() + "Date:  " +
                        database->query.value(database->queryRecord.indexOf("textMsg")).toString());
    }
    database->query.clear();
    database->queryRecord.clear();
}

void Messages::refresh()
{

    listWidget->clear();
    system("/usr/local/Cellar/fetchmail/6.4.12/bin/fetchmail");
    parser->sendInfo(this->acc_id);
    database->query.exec(queryString + QString::number(this->acc_id));
    database->queryRecord = database->query.record(); //get data info for account

    int rowCount=0;
    while (database->query.next()) {
        rowCount++;
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(database->query.value(database->queryRecord.indexOf("themeMsg")).toString() +
                         database->query.value(database->queryRecord.indexOf("fromWho")).toString() + "\n");
        listWidget->insertItem(rowCount, newItem);
        newItem->setData(Qt::UserRole,database->query.value(database->queryRecord.indexOf("msg_id")).toInt());
    }

    database->query.first();
    ui->msgsAmountLBL->setText("Всего полученно писем: " + QString::number(rowCount));
    database->queryRecord.clear();
    database->query.clear();

    database->query.exec("select * from auth where acc_id = " + QString::number(this->acc_id)); //customizationQuery
    database->queryRecord =   database->query.record(); //customQueryRecord
    database->query.first();
    ui->nameEdit->setText(database->query.value(database->queryRecord.indexOf("name")).toString());
    ui->surnameEdit->setText(database->query.value(database->queryRecord.indexOf("surname")).toString());

    QPixmap avatar = QPixmap(database->query.value(database->queryRecord.indexOf("avatar")).toString());
    ui->avatar->setPixmap(avatar.scaled(150,140));

    database->query.clear();
    database->queryRecord.clear();



}

void Messages::shortDisplay()
{
    database->query.exec(queryString + QString::number(this->acc_id));
    database->queryRecord = database->query.record();
    refresh();
    database->query.clear();
    database->queryRecord.clear();
}


void Messages::on_refreshButton_clicked()
{
    listWidget->clear();
    refresh();
}

void Messages::on_hideButton_clicked()
{

    database->query.clear();
    QString deleteQuery;

    QList <QListWidgetItem *> selectedList = listWidget->selectedItems();
    QList <QListWidgetItem *> :: iterator itt;
    itt=selectedList.begin();

    QListWidgetItem * ittToPtr = nullptr;
    for (int i=1; i<=selectedList.size(); i++ ) {
        int tmp=listWidget->row(*itt);
        ittToPtr = listWidget->item(tmp);
        deleteQuery = "delete from data where msg_id = " + ittToPtr->data(Qt::UserRole).toString();
        database->query.exec(deleteQuery);
        itt++;
    }

    database->query.clear();
    refresh();
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->choiceButton->setChecked(false);

}

void Messages::on_choiceButton_clicked()
{
    (listWidget->selectionMode()==QAbstractItemView::MultiSelection)?listWidget->setSelectionMode(QAbstractItemView::SingleSelection):
                                                                     listWidget->setSelectionMode(QAbstractItemView::MultiSelection);
}

void Messages::updateInfo()
{
    database->query.exec("update auth set name = '" + ui->nameEdit->text() + "' where acc_ID = " +  QString::number(this->acc_id));
    database->query.clear();
    database->query.exec("update auth set surname = '" + ui->surnameEdit->text() + "' where acc_ID = " + QString::number(this->acc_id));
    database->query.clear();
    refresh();
}

void Messages::on_changePersInfo_clicked() //Button slot
{
    ui->nameEdit->setEnabled(true);
    ui->surnameEdit->setEnabled(true);
    ui->birthEbit->setEnabled(true);
}

void Messages::on_applyChanges_clicked()  //Button slot
{
    updateInfo();
    ui->nameEdit->setEnabled(false);
    ui->surnameEdit->setEnabled(false);
    ui->birthEbit->setEnabled(false);
}

void Messages::on_toolButton_clicked()
{
    QString imgPath = QFileDialog::getOpenFileName(this, "Open Dialog", "", "*.png *.jpg *.bmp *.JPG");
    if (imgPath.length() == 0) return;
    QString setAvatar = "UPDATE auth SET avatar = '" + imgPath + "' WHERE acc_ID = " + QString::number(this->acc_id) + ";";
    database->query.exec(setAvatar); //img update query
    database->query.clear();
    refresh();
}

void Messages::slotBlock()
{
    database->query.exec("update auth set isBlocked = '1' where acc_ID = " +  QString::number(this->acc_id)); //acc block query
    database->query.clear();
}

void Messages::slotDelete()
{

    QMessageBox msgBox;
    QPushButton *confirmButton = msgBox.addButton(tr("Удалить"), QMessageBox::ActionRole);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText("Вы уверенны, что хотите удалит аккаунт? \nВсе данные будут удалены");
    msgBox.exec();
    if (msgBox.clickedButton() == confirmButton){
        database->query.exec("delete from auth where acc_ID = " +  QString::number(this->acc_id)); //deleting info from auth table
        database->query.exec("delete from data where acc_id = " +  QString::number(this->acc_id)); //deleting info from data table
    }
    delete confirmButton;
}

void Messages::changePassword()
{
    QDialog dlgWindow(this);
    dlgWindow.setWindowTitle(tr("Обновление пароля"));

    QLineEdit *ledit1 = new QLineEdit(&dlgWindow);
    QLineEdit *ledit2 = new QLineEdit(&dlgWindow);

    QDialogButtonBox *btn_box = new QDialogButtonBox(&dlgWindow);
    btn_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(btn_box, &QDialogButtonBox::accepted, &dlgWindow, &QDialog::accept);
    connect(btn_box, &QDialogButtonBox::rejected, &dlgWindow, &QDialog::reject);

    QFormLayout layout;
    layout.addRow(tr("Ваш текущий пароль:"), ledit1);
    layout.addRow(tr("Новый пароль:"), ledit2);
    layout.addWidget(btn_box);

    dlgWindow.setLayout(&layout);

    if(dlgWindow.exec() == QDialog::Accepted)
    {

        database->query.exec("select password from auth where acc_id = " + QString::number(this->acc_id)); //
        database->queryRecord =   database->query.record();
        database->query.first();
        if (database->query.value(database->queryRecord.indexOf("password")).toString() == ledit1->text())
        {
            database->query.clear();
            QString changeString = "update auth set password = '" + ledit2->text() + "' where acc_ID = " +  QString::number(this->acc_id);
            database->query.exec(changeString);
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setText("Вы ввели не существующий пароль");
            msgBox.exec();
        }
        dlgWindow.close();
    }


    delete ledit1;
    delete ledit2;
    delete btn_box;

}




