#include "database.h"

Database::Database()
{
    query = QSqlQuery(getDatabase());
}

Database::~Database()
{}

QSqlDatabase& Database::getDatabase()
{
    static QSqlDatabase database = QSqlDatabase::addDatabase("QMYSQL");
    database.setHostName("localhost");
    database.setDatabaseName("testdb");
    database.setUserName("root");
    database.setPassword("20643579");
    if (!database.isOpen()){
        if(!database.open())
            qDebug() << database.lastError().text();
    }
    return database;
}


