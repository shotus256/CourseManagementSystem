#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>

class DatabaseManager
{
public:
    static DatabaseManager& instance();

    bool connect(const QString& host = "localhost",
                 const QString& dbName = "training_courses",
                 const QString& user = "mariaivanova",
                 const QString& password = "123456",
                 int port = 5432);
    void disconnect();
    bool isConnected() const;

    QSqlQuery executeQuery(const QString& query, const QVariantList& params = {});
    bool executeNonQuery(const QString& query, const QVariantList& params = {});
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

private:
    DatabaseManager() = default;
    ~DatabaseManager() = default;
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase m_db;
    bool m_connected = false;
};

#endif
