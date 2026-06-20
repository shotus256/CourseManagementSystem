#ifndef TEACHERCONTROLLER_H
#define TEACHERCONTROLLER_H

#include <QObject>
#include <QList>
#include "Model/Workload.h"
#include "Model/Teacher.h"

class TeacherController : public QObject {
    Q_OBJECT

public:
    explicit TeacherController(QObject* parent = nullptr);

    Teacher getMyInfo(int userId);
    QList<Workload> getMyWorkloads(int teacherId);
    double getMyTotalHours(int teacherId);
    double getMyTotalPayment(int teacherId);
    double getHourlyRate(int disciplineId, int typeId);

signals:
    void error(const QString& msg);
};

#endif
