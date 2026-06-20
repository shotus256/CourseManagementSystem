#include "DatabaseManager.h"
#include <QDebug>
#include <QSqlError>

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::connect(const QString& host, const QString& dbName,
                              const QString& user, const QString& password, int port) {
    if (m_connected) disconnect();

    m_db = QSqlDatabase::addDatabase("QPSQL");
    m_db.setHostName(host);
    m_db.setPort(port);
    m_db.setDatabaseName(dbName);
    m_db.setUserName(user);
    m_db.setPassword(password);

    if (!m_db.open()) {
        qDebug() << "DB error:" << m_db.lastError().text();
        return false;
    }
    m_connected = true;
    return true;
}

void DatabaseManager::disconnect() {
    if (m_connected) {
        m_db.close();
        m_connected = false;
    }
}

bool DatabaseManager::isConnected() const { return m_connected; }

QSqlQuery DatabaseManager::executeQuery(const QString& query, const QVariantList& params) {
    QSqlQuery q(m_db);
    q.prepare(query);
    for (int i = 0; i < params.size(); ++i) q.bindValue(i, params[i]);
    q.exec();
    return q;
}

bool DatabaseManager::executeNonQuery(const QString& query, const QVariantList& params) {
    QSqlQuery q = executeQuery(query, params);
    return q.isActive() && q.numRowsAffected() > 0;
}

bool DatabaseManager::beginTransaction() { return m_db.transaction(); }
bool DatabaseManager::commitTransaction() { return m_db.commit(); }
bool DatabaseManager::rollbackTransaction() { return m_db.rollback(); }
