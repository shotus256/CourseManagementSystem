#include "Teacher.h"

Teacher::Teacher(int id, int userId, const QString& lastName, const QString& firstName,
                 const QString& middleName, const QString& phone, int experienceYears)
    : id(id), userId(userId), lastName(lastName), firstName(firstName),
    middleName(middleName), phone(phone), experienceYears(experienceYears)
{
}

QString Teacher::getFullName() const
{
    QString fullName = lastName + " " + firstName;
    if (!middleName.isEmpty()) {
        fullName += " " + middleName;
    }
    return fullName;
}

bool Teacher::isValid() const
{
    return userId > 0 && !lastName.isEmpty() && !firstName.isEmpty()
    && !phone.isEmpty() && experienceYears >= 0;
}
