#ifndef COURSE_H
#define COURSE_H

#include <QString>
#include <QDate>

class Course
{
public:
    Course() = default;
    Course(int id, const QString& name, const QDate& startDate, const QDate& endDate);

    int id = -1;
    QString name;
    QDate startDate;
    QDate endDate;

    bool isValid() const;
};

#endif
