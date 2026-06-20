#include "AuthController.h"
#include "Database/DatabaseManager.h"
#include <QCryptographicHash>
#include <QRegularExpression>
#include <QDebug>

bool AuthController::validatePassword(const QString& password, QString& error) {
    if (password.length() < 8) {
        error = "Минимум 8 символов";
        return false;
    }
    if (!password.contains(QRegularExpression("\\d"))) {
        error = "Нужна цифра";
        return false;
    }
    if (!password.contains(QRegularExpression("[A-Z]"))) {
        error = "Нужна заглавная буква";
        return false;
    }
    if (!password.contains(QRegularExpression("[!@#$%^&*()_+\\-=\\[\\]{};':\"\\\\|,.<>/?]"))) {
        error = "Нужен спецсимвол";
        return false;
    }
    return true;
}

QString AuthController::hashPassword(const QString& password) const {
    return QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();
}

bool AuthController::verifyPassword(const QString& plain, const QString& hashed) const {
    return hashPassword(plain) == hashed;
}

bool AuthController::authenticate(const QString& login, const QString& password, User& outUser) {
    auto& db = DatabaseManager::instance();
    if (!db.isConnected()) { qDebug() << "DB not connected"; return false; }

    QSqlQuery q = db.executeQuery(
        "SELECT id_user, login, password_hash FROM users WHERE login = $1", {login});
    if (!q.next()) { qDebug() << "User not found:" << login; return false; }

    int userId = q.value(0).toInt();
    if (!verifyPassword(password, q.value(2).toString())) {
        qDebug() << "Password mismatch:" << login;
        return false;
    }

    outUser = User(userId, q.value(1).toString());

    QSqlQuery roles = db.executeQuery(
        "SELECT r.role_name FROM user_roles ur "
        "JOIN roles r ON r.id_role = ur.id_role WHERE ur.id_user = $1",
        {userId});
    while (roles.next()) outUser.addRole(roles.value(0).toString());

    qDebug() << "User" << login << "auth OK, roles:" << outUser.getRoles().size();
    return true;
}

bool AuthController::registerUser(const QString& login, const QString& password) {
    QString error;
    if (!validatePassword(password, error)) { qDebug() << error; return false; }

    auto& db = DatabaseManager::instance();
    if (!db.isConnected()) return false;

    db.beginTransaction();
    QString hashed = hashPassword(password);
    QSqlQuery q = db.executeQuery(
        "INSERT INTO users (login, password_hash) VALUES ($1, $2) RETURNING id_user",
        {login.trimmed(), hashed});

    if (!q.next()) { db.rollbackTransaction(); return false; }

    db.commitTransaction();
    return true;
}

bool AuthController::changePassword(int userId, const QString& oldPassword, const QString& newPassword) {
    QString error;
    if (!validatePassword(newPassword, error)) { qDebug() << error; return false; }

    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery("SELECT password_hash FROM users WHERE id_user = $1", {userId});
    if (!q.next() || !verifyPassword(oldPassword, q.value(0).toString())) return false;

    return db.executeNonQuery("UPDATE users SET password_hash = $1 WHERE id_user = $2",
                              {hashPassword(newPassword), userId});
}

bool AuthController::resetPassword(int userId, const QString& newPassword) {
    QString error;
    if (!validatePassword(newPassword, error)) { qDebug() << error; return false; }

    auto& db = DatabaseManager::instance();
    return db.executeNonQuery("UPDATE users SET password_hash = $1 WHERE id_user = $2",
                              {hashPassword(newPassword), userId});
}
