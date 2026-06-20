#ifndef USER_H
#define USER_H

#include <QString>
#include <QList>

class User
{
public:
    User() = default;
    User(int id, const QString& login);

    int getId() const;
    QString getLogin() const;
    QList<QString> getRoles() const;

    void setId(int id);
    void setLogin(const QString& login);
    void addRole(const QString& role);

    bool isMethodologist() const;
    bool isTeacher() const;
    bool hasMultipleRoles() const;

private:
    int m_id = -1;
    QString m_login;
    QList<QString> m_roles;
};

#endif
