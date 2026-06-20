#ifndef GROUP_H
#define GROUP_H

#include <QString>

class Group
{
public:
    Group() = default;
    Group(int id, int courseId, const QString& name, const QString& specialty, int studentCount);

    int id = -1;
    int courseId = -1;
    QString name;
    QString specialty;
    int studentCount = 0;

    bool isValid() const;
};

#endif
