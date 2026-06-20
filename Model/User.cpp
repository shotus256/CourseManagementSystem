#include "User.h"

User::User(int id, const QString& login)
    : m_id(id), m_login(login)
{
}

int User::getId() const
{
    return m_id;
}

QString User::getLogin() const
{
    return m_login;
}

QList<QString> User::getRoles() const
{
    return m_roles;
}

void User::setId(int id)
{
    m_id = id;
}

void User::setLogin(const QString& login)
{
    m_login = login;
}

void User::addRole(const QString& role)
{
    if (!m_roles.contains(role)) {
        m_roles.append(role);
    }
}

bool User::isMethodologist() const
{
    return m_roles.contains("methodologist");
}

bool User::isTeacher() const
{
    return m_roles.contains("teacher");
}

bool User::hasMultipleRoles() const
{
    return m_roles.size() > 1;
}
