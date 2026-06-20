#ifndef TEACHER_H
#define TEACHER_H

#include <QString>

class Teacher
{
public:
    Teacher() = default;
    Teacher(int id, int userId, const QString& lastName, const QString& firstName,
            const QString& middleName, const QString& phone, int experienceYears);

    int id = -1;
    int userId = -1;
    QString lastName;
    QString firstName;
    QString middleName;
    QString phone;
    int experienceYears = 0;

    QString getFullName() const;
    bool isValid() const;
};

#endif
