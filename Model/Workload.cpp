#include "Workload.h"

Workload::Workload(int id, double hoursCount, int teacherId, int groupId,
                   int disciplineId, int typeId)
    : id(id), hoursCount(hoursCount), teacherId(teacherId), groupId(groupId),
    disciplineId(disciplineId), typeId(typeId)
{
}

double Workload::getTotalPayment() const
{
    return hoursCount * hourlyRate;
}

bool Workload::isValid() const
{
    return hoursCount > 0 && teacherId > 0 && groupId > 0
           && disciplineId > 0 && typeId > 0;
}
