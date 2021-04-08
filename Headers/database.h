#ifndef DATABASE_H
#define DATABASE_H

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlTableModel>

class Database
{
public:
    Database();
    QSqlQuery query;
    QSqlRecord queryRecord;
    static QSqlDatabase& getDatabase();
    ~Database();
};

#endif // DATABASE_H
