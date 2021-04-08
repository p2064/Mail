#include "statistics.h"
#include "ui_admin.h"

Statistics::Statistics(Ui::Admin *ui)
{
    this->ui = ui;
    database = new Database();
    chart = new QChart();
    chartView = new QChartView(chart);
    barChart = new QChart();
    barChartView = new QChartView(barChart);
}
Statistics::~Statistics()
{
    delete ui;
}

void Statistics::updateStats()
{
    database->query.exec("SELECT SUM(isBlocked), SUM(isVerified = 0) FROM auth");
    database->query.first();
    ui->blockedUsers->setText("Всего заблокировано: " +  database->query.value(0).toString());
    ui->unverifiedUsers->setText("Всего на подтверждении: " + database->query.value(1).toString());

    database->query.exec("SELECT COUNT(*) FROM data");
    database->query.first();
    ui->totalMessages->setText("Всего писем в системе: " +  database->query.value(0).toString());

    QSqlQuery query = QSqlQuery(database->getDatabase());
    database->query.exec("SELECT acc_ID FROM auth");
    while(database->query.next()){
        query.exec("UPDATE auth SET total_msg = (SELECT COUNT(data.msg_id) FROM data WHERE data.acc_id = "
                   + database->query.value("acc_ID").toString() + ") WHERE acc_ID = " + database->query.value("acc_ID").toString());
    }
    query.clear();
}

void Statistics::getDonutPieChart()
{
    ui->donutPieChart->hide();

    QPieSeries *series = new QPieSeries();
    series->setHoleSize(0.35);
    series->setVerticalPosition(0.6);

    QPieSlice *seriesSlice = new QPieSlice();
    chart->setTheme(QChart::ChartThemeQt);

    if (ui->comboBox->currentIndex() <= 1){
        database->query.exec("SELECT COUNT(*) FROM auth where isBlocked = 1 or isVerified = 0");
        database->query.first();
        int inactive = database->query.value(0).toInt();

        database->query.exec("select * from auth");
        seriesSlice = series->append("Активные: " + QString::number(database->query.size() - inactive), database->query.size() - inactive);
        series->append("Заблокированные / неподтвержденные: " + QString::number(inactive), inactive);
        chart->setTitle("Распределение пользователей:");
    }

    if (ui->comboBox->currentIndex() >= 2){
        database->query.exec("SELECT COUNT(*) FROM data where acc_id = " + acc_ID);
        database->query.first();
        int individual = database->query.value(0).toInt();

        database->query.exec("SELECT COUNT(*) FROM data");
        database->query.first();
        int total = database->query.value(0).toInt();

        seriesSlice = series->append("Письма " + ui->lineEdit->text() + ": " + QString::number(individual), individual);
        series->append("Остальные письма: " + QString::number(total - individual), total - individual);
        chart->setTitle("Доля " + ui->lineEdit->text() + " в общем объеме писем:");
    }
    seriesSlice->setExploded();
    seriesSlice->setPen(QPen(Qt::darkGreen, 2));
    seriesSlice->setBrush(Qt::green);

    chart->removeAllSeries();
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setTitleBrush(Qt::white);
    chart->setBackgroundBrush(QBrush(QColor("transparent")));
    chart->legend()->setAlignment(Qt::AlignLeft);
    chart->legend()->detachFromChart();
    chart->legend()->setGeometry(50, 40, 400, 200);
    chart->legend()->setLabelBrush(Qt::white);

    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setParent(ui->donutPieChart);
    chartView->resize(375, 300);
    ui->donutPieChart->show();


}

void Statistics::getVerticalBarChart()
{
    ui->verticalBarChart->hide();

    QStackedBarSeries *series = new QStackedBarSeries();
    QStringList categories;
    QBarSet *set0 = new QBarSet("");

    if (ui->comboBox->currentIndex() <= 1){
        database->query.exec("SELECT acc_ID, total_msg FROM auth ORDER BY total_msg DESC");
        for (int i = 0; i < 5 && database->query.next(); ++i) {
            categories.append(database->query.value(0).toString());
            set0->append(database->query.value(1).toInt());
        }
        set0->setBrush(QBrush(Qt::green));
        series->append(set0);
        barChart->legend()->hide();
        barChart->setTitle("Топ пользователей по количеству писем:");
    }

    if (ui->comboBox->currentIndex() >= 2){
        QStringList labels = {"Аватар", "Имя", "Фамилия", "Дата рождения"};
        QStringList colorLabels = {};
        QStringList grayLabels = {};

        QBarSet *set1 = new QBarSet("");
        QBarSet *set2 = new QBarSet("");
        QBarSet *set3 = new QBarSet("");

        set0->append(25);
        set1->append(25);
        set2->append(25);
        set3->append(25);

        database->query.exec("select avatar, name, surname, dateBirth from auth where acc_ID = " + acc_ID);
        database->query.first();
        database->queryRecord = database->query.record();

        for(int i = 0; i < 4; i++){
            if (database->queryRecord.value(i).toString() == "")
                grayLabels.append(labels.at(i));
            else
                colorLabels.append(labels.at(i));
        }
        colorLabels.append(grayLabels);

        set0->setLabel(colorLabels.at(0));
        set1->setLabel(colorLabels.at(1));
        set2->setLabel(colorLabels.at(2));
        set3->setLabel(colorLabels.at(3));

        barChart->setTheme(QChart::ChartThemeQt);
        for (int i = 0; i < grayLabels.size(); i++){
            if (i == 0) set3->setBrush(QBrush(Qt::gray));
            else if (i == 1) set2->setBrush(QBrush(Qt::gray));
            else if (i == 2) set1->setBrush(QBrush(Qt::gray));
            else if (i == 3) set0->setBrush(QBrush(Qt::gray));
        }
        series->append(set0);
        series->append(set1);
        series->append(set2);
        series->append(set3);
        categories << ui->lineEdit->text();
        barChart->setTitle("Заполненность профиля " + ui->lineEdit->text() + ": " + QString::number(100 - 25 * grayLabels.size()) + "%");
    }
    barChart->removeAllSeries();
    barChart->addSeries(series);
    barChart->legend()->setAlignment(Qt::AlignTop);
    barChart->legend()->setMinimumWidth(300);
    barChart->setTitleBrush(Qt::white);
    barChart->setAnimationOptions(QChart::SeriesAnimations);
    barChart->setBackgroundBrush(QBrush(QColor("transparent")));

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->setLabelsBrush(Qt::white);
    axisX->append(categories);
    if (barChart->axes(Qt::Horizontal).empty()) barChart->addAxis(axisX, Qt::AlignBottom);
    else barChart->setAxisX(axisX);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelsBrush(Qt::white);
    if (barChart->axes(Qt::Vertical).empty()) barChart->addAxis(axisY, Qt::AlignLeft);
    else barChart->setAxisY(axisY);
    series->attachAxis(axisY);

    barChartView->setRenderHint(QPainter::Antialiasing);
    barChartView->setParent(ui->verticalBarChart);
    barChartView->resize(400, 300);
    ui->verticalBarChart->show();
}
