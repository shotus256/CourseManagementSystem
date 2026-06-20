#include "Group.h"

Group::Group(int id, int courseId, const QString& name, const QString& specialty, int studentCount)
    : id(id), courseId(courseId), name(name), specialty(specialty), studentCount(studentCount)
{
}

bool Group::isValid() const
{
    return courseId > 0 && !name.isEmpty() && !specialty.isEmpty() && studentCount >= 0;
}
