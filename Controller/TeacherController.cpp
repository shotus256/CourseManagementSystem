#include "TeacherController.h"
#include "Database/DatabaseManager.h"

TeacherController::TeacherController(QObject* parent) : QObject(parent) {}

Teacher TeacherController::getMyInfo(int userId) {
    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery(
        "SELECT id_teacher, last_name, first_name, middle_name, phone, experience_years "
        "FROM teachers WHERE id_user=$1", {userId});
    Teacher t;
    if (q.next()) {
        t.id = q.value(0).toInt();
        t.userId = userId;
        t.lastName = q.value(1).toString();
        t.firstName = q.value(2).toString();
        t.middleName = q.value(3).toString();
        t.phone = q.value(4).toString();
        t.experienceYears = q.value(5).toInt();
    }
    return t;
}

QList<Workload> TeacherController::getMyWorkloads(int teacherId) {
    QList<Workload> workloads;
    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery(
        "SELECT w.id_workload, w.hours_count, w.id_teacher, w.id_group, w.id_discipline, w.id_type, "
        "g.group_name, d.discipline_name, lt.type_name, dt.hourly_rate "
        "FROM workloads w "
        "JOIN groups g ON g.id_group = w.id_group "
        "JOIN disciplines d ON d.id_discipline = w.id_discipline "
        "JOIN lesson_types lt ON lt.id_type = w.id_type "
        "JOIN discipline_types dt ON dt.id_discipline = w.id_discipline AND dt.id_type = w.id_type "
        "WHERE w.id_teacher=$1 ORDER BY g.group_name, d.discipline_name", {teacherId});
    while (q.next()) {
        Workload w;
        w.id = q.value(0).toInt();
        w.hoursCount = q.value(1).toDouble();
        w.teacherId = q.value(2).toInt();
        w.groupId = q.value(3).toInt();
        w.disciplineId = q.value(4).toInt();
        w.typeId = q.value(5).toInt();
        w.groupName = q.value(6).toString();
        w.disciplineName = q.value(7).toString();
        w.typeName = q.value(8).toString();
        w.hourlyRate = q.value(9).toDouble();
        workloads.append(w);
    }
    return workloads;
}

double TeacherController::getMyTotalHours(int teacherId) {
    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery("SELECT SUM(hours_count) FROM workloads WHERE id_teacher=$1", {teacherId});
    return q.next() ? q.value(0).toDouble() : 0;
}

double TeacherController::getMyTotalPayment(int teacherId) {
    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery(
        "SELECT SUM(w.hours_count * dt.hourly_rate) "
        "FROM workloads w JOIN discipline_types dt ON dt.id_discipline = w.id_discipline AND dt.id_type = w.id_type "
        "WHERE w.id_teacher=$1", {teacherId});
    return q.next() ? q.value(0).toDouble() : 0;
}

double TeacherController::getHourlyRate(int disciplineId, int typeId) {
    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery(
        "SELECT hourly_rate FROM discipline_types WHERE id_discipline=$1 AND id_type=$2",
        {disciplineId, typeId});
    return q.next() ? q.value(0).toDouble() : 0;
}
