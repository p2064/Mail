#ifndef ADMIN_H
#define ADMIN_H

#include <cstring>
#include <QDate>
#include "database.h"
#include "statistics.h"
#include "login.h"

class Statistics;

namespace Ui { class Admin; }

class Admin : public QWidget
{
    Q_OBJECT
public:
    explicit Admin(QWidget *parent = nullptr);
    ~Admin();

private:
    Ui::Admin *ui;
    QSqlTableModel *model;
    Database *database;
    Statistics *statistics;
    QMenu *backUp;
    QMenu *import;
    QAction *fullBackUp;
    QAction *structBackUp;
    QAction *overwiteImport;
    QAction *addImport;
    std::pair <QString, QString> intermediateImport();


private slots:
    void on_getTable_clicked();
    void on_deleteSelectedRows_clicked();
    void on_updatePage_clicked();
    void on_submitChanges_clicked();
    void on_unblockAll_clicked();
    void on_verifyAll_clicked();
    void on_deleteAllMessages_clicked();
    void updateTable();
    QString on_backUp_clicked();
    void slotBackup();
    void slotImport();
};

#endif // ADMIN_H
