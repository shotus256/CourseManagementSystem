#ifndef AUTHCONTROLLER_H
#define AUTHCONTROLLER_H

#include <QString>
#include "Model/User.h"

class AuthController {
public:
    bool authenticate(const QString& login, const QString& password, User& outUser);
    bool registerUser(const QString& login, const QString& password);

    bool changePassword(int userId, const QString& oldPassword, const QString& newPassword);
    bool resetPassword(int userId, const QString& newPassword);

    static bool validatePassword(const QString& password, QString& error);

private:
    QString hashPassword(const QString& password) const;
    bool verifyPassword(const QString& plain, const QString& hashed) const;
};

#endif
