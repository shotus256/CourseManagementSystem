#ifndef METHODOLOGISTCONTROLLER_H
#define METHODOLOGISTCONTROLLER_H

#include <QObject>
#include <QList>
#include "Model/Teacher.h"
#include "Model/Workload.h"
#include "Model/Course.h"
#include "Model/Group.h"
#include "Model/Discipline.h"
#include "Model/User.h"
#include "Model/Specialty.h"

class MethodologistController : public QObject {
    Q_OBJECT

public:
    explicit MethodologistController(QObject* parent = nullptr);

    // Преподаватели
    QList<Teacher> getAllTeachers();
    bool addTeacher(const Teacher& teacher);
    bool updateTeacher(const Teacher& teacher);
    bool deleteTeacher(int teacherId);
    Teacher getTeacherByUserId(int userId);

    // Нагрузка
    QList<Workload> getAllWorkloads();
    QList<Workload> getWorkloadsByTeacher(int teacherId);
    bool addWorkload(const Workload& workload);
    bool updateWorkload(const Workload& workload);
    bool deleteWorkload(int workloadId);
    double getTeacherTotalPayment(int teacherId);

    // Курсы
    QList<Course> getAllCourses();
    bool addCourse(const Course& course);
    bool updateCourse(const Course& course);
    bool deleteCourse(int courseId);

    // Группы
    QList<Group> getAllGroups();
    QList<Group> getGroupsByCourse(int courseId);
    bool addGroup(const Group& group);
    bool updateGroup(const Group& group);
    bool deleteGroup(int groupId);

    // Дисциплины
    QList<Discipline> getAllDisciplines();
    bool addDiscipline(const Discipline& discipline);
    bool updateDiscipline(const Discipline& discipline);
    bool deleteDiscipline(int disciplineId);
    double getHourlyRate(int disciplineId, int typeId);

    // Специальности
    QList<Specialty> getAllSpecialties();
    bool addSpecialty(const Specialty& specialty);
    bool updateSpecialty(const Specialty& specialty);
    bool deleteSpecialty(const QString& specialtyName);

    // Пользователи и роли
    QList<User> getAllUsers();
    User getUserById(int userId);
    User getUserByLogin(const QString& login);
    bool assignRole(int userId, const QString& roleName);
    bool removeRole(int userId, const QString& roleName);
    bool hasRole(int userId, const QString& roleName);
    QList<QString> getUserRoles(int userId);
    bool makeMethodologist(int userId);
    bool removeMethodologist(int userId);
    bool makeTeacher(int userId);
    bool removeTeacher(int userId);
    bool createUserWithRoles(const QString& login, const QString& password, const QList<QString>& roles);
    QList<User> getUsersWithRole(const QString& roleName);
    bool deleteUser(int userId);

signals:
    void error(const QString& msg);
    void dataChanged();

private:
    void loadDisciplineRates(Discipline& d);
    int getTypeId(const QString& typeName);
    int getRoleId(const QString& roleName);
};

#endif
