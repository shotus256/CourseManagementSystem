#ifndef WORKLOAD_H
#define WORKLOAD_H

#include <QString>

class Workload
{
public:
    Workload() = default;
    Workload(int id, double hoursCount, int teacherId, int groupId,
             int disciplineId, int typeId);

    int id = -1;
    double hoursCount = 0;
    int teacherId = -1;
    int groupId = -1;
    int disciplineId = -1;
    int typeId = -1;

    // Для отображения
    QString teacherName;
    QString groupName;
    QString disciplineName;
    QString typeName;
    double hourlyRate = 0;

    double getTotalPayment() const;
    bool isValid() const;
};

#endif
