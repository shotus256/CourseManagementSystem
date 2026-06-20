#include "Course.h"

Course::Course(int id, const QString& name, const QDate& startDate, const QDate& endDate)
    : id(id), name(name), startDate(startDate), endDate(endDate)
{
}

bool Course::isValid() const
{
    return !name.isEmpty() && startDate.isValid() && endDate.isValid() && endDate >= startDate;
}
