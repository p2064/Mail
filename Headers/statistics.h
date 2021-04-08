#ifndef STATISTICS_H
#define STATISTICS_H

#include <QChartView>
#include <QPieSeries>
#include <QPieSlice>
#include <QBarSet>
#include <QStackedBarSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include "database.h"
#include "admin.h"

using namespace QtCharts;

namespace Ui { class Admin; }

class Statistics
{
public:
    Statistics(Ui::Admin *ui);
    ~Statistics();
    QString acc_ID;
    void updateStats();
    void getDonutPieChart();
    void getVerticalBarChart();

private:
    Ui::Admin *ui;
    Database *database;
    QChart *chart;
    QChartView *chartView;
    QChart *barChart;
    QChartView *barChartView;

};

#endif // STATISTICS_H
