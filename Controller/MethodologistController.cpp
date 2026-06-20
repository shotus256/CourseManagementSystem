#include "MethodologistController.h"
#include "Database/DatabaseManager.h"
#include "Controller/AuthController.h"
#include "Model/Specialty.h"
#include <QDebug>

MethodologistController::MethodologistController(QObject* parent) : QObject(parent) {}

// вспомогательные методы
// заполняет Teacher из текущей строки запроса
static Teacher teacherFromQuery(QSqlQuery& q) {
    Teacher t;
    t.id             = q.value(0).toInt();
    t.userId         = q.value(1).toInt();
    t.lastName       = q.value(2).toString();
    t.firstName      = q.value(3).toString();
    t.middleName     = q.value(4).toString();
    t.phone          = q.value(5).toString();
    t.experienceYears = q.value(6).toInt();
    return t;
}

int MethodologistController::getTypeId(const QString& typeName) {
    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery("SELECT id_type FROM lesson_types WHERE type_name=$1", {typeName});
    return q.next() ? q.value(0).toInt() : -1;
}

int MethodologistController::getRoleId(const QString& roleName) {
    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery("SELECT id_role FROM roles WHERE role_name=$1", {roleName});
    return q.next() ? q.value(0).toInt() : -1;
}

void MethodologistController::loadDisciplineRates(Discipline& d) {
    auto& db = DatabaseManager::instance();
    int lectureType  = getTypeId("Лекция");
    int practiceType = getTypeId("Практика");

    if (lectureType != -1) {
        QSqlQuery q = db.executeQuery(
            "SELECT hourly_rate FROM discipline_types WHERE id_discipline=$1 AND id_type=$2",
            {d.id, lectureType});
        if (q.next()) d.lectureRate = q.value(0).toDouble();
    }
    if (practiceType != -1) {
        QSqlQuery q = db.executeQuery(
            "SELECT hourly_rate FROM discipline_types WHERE id_discipline=$1 AND id_type=$2",
            {d.id, practiceType});
        if (q.next()) d.practiceRate = q.value(0).toDouble();
    }
}

// преподаватели
QList<Teacher> MethodologistController::getAllTeachers() {
    QList<Teacher> teachers;
    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery(
        "SELECT id_teacher, id_user, last_name, first_name, middle_name, phone, experience_years "
        "FROM teachers ORDER BY last_name");
    while (q.next())
        teachers.append(teacherFromQuery(q));
    return teachers;
}

bool MethodologistController::addTeacher(const Teacher& t) {
    if (!t.isValid()) { emit error("Некорректные данные"); return false; }
    auto& db = DatabaseManager::instance();
    return db.executeNonQuery(
        "INSERT INTO teachers (id_user, last_name, first_name, middle_name, phone, experience_years) "
        "VALUES ($1, $2, $3, $4, $5, $6)",
        {t.userId, t.lastName, t.firstName, t.middleName, t.phone, t.experienceYears});
}

bool MethodologistController::updateTeacher(const Teacher& t) {
    if (!t.isValid() || t.id <= 0) { emit error("Некорректные данные"); return false; }
    auto& db = DatabaseManager::instance();
    return db.executeNonQuery(
        "UPDATE teachers SET last_name=$1, first_name=$2, middle_name=$3, phone=$4, experience_years=$5 "
        "WHERE id_teacher=$6",
        {t.lastName, t.firstName, t.middleName, t.phone, t.experienceYears, t.id});
}

bool MethodologistController::deleteTeacher(int teacherId) {
    auto& db = DatabaseManager::instance();
    return db.executeNonQuery("DELETE FROM teachers WHERE id_teacher=$1", {teacherId});
}

Teacher MethodologistController::getTeacherByUserId(int userId) {
    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery(
        "SELECT id_teacher, id_user, last_name, first_name, middle_name, phone, experience_years "
        "FROM teachers WHERE id_user=$1", {userId});
    return q.next() ? teacherFromQuery(q) : Teacher{};
}

// нагрузка
QList<Workload> MethodologistController::getAllWorkloads() {
    QList<Workload> workloads;
    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery(
        "SELECT w.id_workload, w.hours_count, w.id_teacher, w.id_group, w.id_discipline, w.id_type, "
        "       t.last_name || ' ' || t.first_name || COALESCE(' ' || NULLIF(t.middle_name,''), '') AS teacher_name, "
        "       g.group_name, d.discipline_name, lt.type_name, dt.hourly_rate "
        "FROM workloads w "
        "JOIN teachers t       ON t.id_teacher    = w.id_teacher "
        "JOIN groups g         ON g.id_group      = w.id_group "
        "JOIN disciplines d    ON d.id_discipline = w.id_discipline "
        "JOIN lesson_types lt  ON lt.id_type      = w.id_type "
        "JOIN discipline_types dt ON dt.id_discipline = w.id_discipline AND dt.id_type = w.id_type "
        "ORDER BY t.last_name, g.group_name, d.discipline_name");
    while (q.next()) {
        Workload w;
        w.id             = q.value(0).toInt();
        w.hoursCount     = q.value(1).toDouble();
        w.teacherId      = q.value(2).toInt();
        w.groupId        = q.value(3).toInt();
        w.disciplineId   = q.value(4).toInt();
        w.typeId         = q.value(5).toInt();
        w.teacherName    = q.value(6).toString();
        w.groupName      = q.value(7).toString();
        w.disciplineName = q.value(8).toString();
        w.typeName       = q.value(9).toString();
        w.hourlyRate     = q.value(10).toDouble();
        workloads.append(w);
    }
    return workloads;
}

QList<Workload> MethodologistController::getWorkloadsByTeacher(int teacherId) {
    QList<Workload> workloads;
    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery(
        "SELECT w.id_workload, w.hours_count, w.id_teacher, w.id_group, w.id_discipline, w.id_type, "
        "       t.last_name || ' ' || t.first_name || COALESCE(' ' || NULLIF(t.middle_name,''), '') AS teacher_name, "
        "       g.group_name, d.discipline_name, lt.type_name, dt.hourly_rate "
        "FROM workloads w "
        "JOIN teachers t       ON t.id_teacher    = w.id_teacher "
        "JOIN groups g         ON g.id_group      = w.id_group "
        "JOIN disciplines d    ON d.id_discipline = w.id_discipline "
        "JOIN lesson_types lt  ON lt.id_type      = w.id_type "
        "JOIN discipline_types dt ON dt.id_discipline = w.id_discipline AND dt.id_type = w.id_type "
        "WHERE w.id_teacher=$1 "
        "ORDER BY g.group_name, d.discipline_name", {teacherId});
    while (q.next()) {
        Workload w;
        w.id             = q.value(0).toInt();
        w.hoursCount     = q.value(1).toDouble();
        w.teacherId      = q.value(2).toInt();
        w.groupId        = q.value(3).toInt();
        w.disciplineId   = q.value(4).toInt();
        w.typeId         = q.value(5).toInt();
        w.teacherName    = q.value(6).toString();
        w.groupName      = q.value(7).toString();
        w.disciplineName = q.value(8).toString();
        w.typeName       = q.value(9).toString();
        w.hourlyRate     = q.value(10).toDouble();
        workloads.append(w);
    }
    return workloads;
}

bool MethodologistController::addWorkload(const Workload& w) {
    if (!w.isValid()) { emit error("Некорректные данные нагрузки"); return false; }
    auto& db = DatabaseManager::instance();
    bool result = db.executeNonQuery(
        "INSERT INTO workloads (hours_count, id_teacher, id_group, id_discipline, id_type) "
        "VALUES ($1, $2, $3, $4, $5)",
        {w.hoursCount, w.teacherId, w.groupId, w.disciplineId, w.typeId});
    if (result) emit dataChanged();
    return result;
}

bool MethodologistController::updateWorkload(const Workload& w) {
    if (!w.isValid() || w.id <= 0) { emit error("Некорректные данные"); return false; }
    auto& db = DatabaseManager::instance();
    bool result = db.executeNonQuery(
        "UPDATE workloads SET hours_count=$1, id_teacher=$2, id_group=$3, id_discipline=$4, id_type=$5 "
        "WHERE id_workload=$6",
        {w.hoursCount, w.teacherId, w.groupId, w.disciplineId, w.typeId, w.id});
    if (result) emit dataChanged();
    return result;
}

bool MethodologistController::deleteWorkload(int workloadId) {
    auto& db = DatabaseManager::instance();
    bool result = db.executeNonQuery("DELETE FROM workloads WHERE id_workload=$1", {workloadId});
    if (result) emit dataChanged();
    return result;
}

double MethodologistController::getTeacherTotalPayment(int teacherId) {
    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery(
        "SELECT SUM(w.hours_count * dt.hourly_rate) "
        "FROM workloads w "
        "JOIN discipline_types dt ON dt.id_discipline = w.id_discipline AND dt.id_type = w.id_type "
        "WHERE w.id_teacher=$1", {teacherId});
    return (q.next() && !q.value(0).isNull()) ? q.value(0).toDouble() : 0;
}

// курсы
QList<Course> MethodologistController::getAllCourses() {
    QList<Course> courses;
    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery(
        "SELECT id_course, course_name, start_date, end_date FROM courses ORDER BY start_date DESC");
    while (q.next()) {
        Course c;
        c.id        = q.value(0).toInt();
        c.name      = q.value(1).toString();
        c.startDate = q.value(2).toDate();
        c.endDate   = q.value(3).toDate();
        courses.append(c);
    }
    return courses;
}

bool MethodologistController::addCourse(const Course& c) {
    if (!c.isValid()) { emit error("Некорректные данные курса"); return false; }
    auto& db = DatabaseManager::instance();
    bool result = db.executeNonQuery(
        "INSERT INTO courses (course_name, start_date, end_date) VALUES ($1, $2, $3)",
        {c.name, c.startDate, c.endDate});
    if (result) emit dataChanged();
    return result;
}

bool MethodologistController::updateCourse(const Course& c) {
    if (!c.isValid() || c.id <= 0) { emit error("Некорректные данные"); return false; }
    auto& db = DatabaseManager::instance();
    bool result = db.executeNonQuery(
        "UPDATE courses SET course_name=$1, start_date=$2, end_date=$3 WHERE id_course=$4",
        {c.name, c.startDate, c.endDate, c.id});
    if (result) emit dataChanged();
    return result;
}

bool MethodologistController::deleteCourse(int courseId) {
    auto& db = DatabaseManager::instance();
    bool result = db.executeNonQuery("DELETE FROM courses WHERE id_course=$1", {courseId});
    if (result) emit dataChanged();
    return result;
}

// группы
QList<Group> MethodologistController::getAllGroups() {
    QList<Group> groups;
    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery(
        "SELECT id_group, id_course, group_name, specialty, student_count "
        "FROM groups ORDER BY group_name");
    while (q.next()) {
        Group g;
        g.id           = q.value(0).toInt();
        g.courseId     = q.value(1).toInt();
        g.name         = q.value(2).toString();
        g.specialty    = q.value(3).toString();
        g.studentCount = q.value(4).toInt();
        groups.append(g);
    }
    return groups;
}

QList<Group> MethodologistController::getGroupsByCourse(int courseId) {
    QList<Group> groups;
    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery(
        "SELECT id_group, id_course, group_name, specialty, student_count "
        "FROM groups WHERE id_course=$1 ORDER BY group_name", {courseId});
    while (q.next()) {
        Group g;
        g.id           = q.value(0).toInt();
        g.courseId     = q.value(1).toInt();
        g.name         = q.value(2).toString();
        g.specialty    = q.value(3).toString();
        g.studentCount = q.value(4).toInt();
        groups.append(g);
    }
    return groups;
}

bool MethodologistController::addGroup(const Group& g) {
    if (!g.isValid()) { emit error("Некорректные данные группы"); return false; }
    auto& db = DatabaseManager::instance();
    bool result = db.executeNonQuery(
        "INSERT INTO groups (id_course, group_name, specialty, student_count) VALUES ($1, $2, $3, $4)",
        {g.courseId, g.name, g.specialty, g.studentCount});
    if (result) emit dataChanged();
    return result;
}

bool MethodologistController::updateGroup(const Group& g) {
    if (!g.isValid() || g.id <= 0) { emit error("Некорректные данные"); return false; }
    auto& db = DatabaseManager::instance();
    bool result = db.executeNonQuery(
        "UPDATE groups SET id_course=$1, group_name=$2, specialty=$3, student_count=$4 WHERE id_group=$5",
        {g.courseId, g.name, g.specialty, g.studentCount, g.id});
    if (result) emit dataChanged();
    return result;
}

bool MethodologistController::deleteGroup(int groupId) {
    auto& db = DatabaseManager::instance();
    bool result = db.executeNonQuery("DELETE FROM groups WHERE id_group=$1", {groupId});
    if (result) emit dataChanged();
    return result;
}

// дисциплины
QList<Discipline> MethodologistController::getAllDisciplines() {
    QList<Discipline> disciplines;
    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery(
        "SELECT id_discipline, discipline_name FROM disciplines ORDER BY discipline_name");
    while (q.next()) {
        Discipline d;
        d.id   = q.value(0).toInt();
        d.name = q.value(1).toString();
        loadDisciplineRates(d);
        disciplines.append(d);
    }
    return disciplines;
}

bool MethodologistController::addDiscipline(const Discipline& d) {
    if (!d.isValid()) { emit error("Некорректные данные дисциплины"); return false; }
    auto& db = DatabaseManager::instance();
    db.beginTransaction();

    QSqlQuery q = db.executeQuery(
        "INSERT INTO disciplines (discipline_name) VALUES ($1) RETURNING id_discipline", {d.name});
    if (!q.next()) { db.rollbackTransaction(); return false; }

    int id           = q.value(0).toInt();
    int lectureType  = getTypeId("Лекция");
    int practiceType = getTypeId("Практика");

    if (d.lectureRate > 0 && lectureType != -1)
        db.executeNonQuery(
            "INSERT INTO discipline_types (id_discipline, id_type, hourly_rate) VALUES ($1, $2, $3)",
            {id, lectureType, d.lectureRate});
    if (d.practiceRate > 0 && practiceType != -1)
        db.executeNonQuery(
            "INSERT INTO discipline_types (id_discipline, id_type, hourly_rate) VALUES ($1, $2, $3)",
            {id, practiceType, d.practiceRate});

    db.commitTransaction();
    emit dataChanged();
    return true;
}

bool MethodologistController::updateDiscipline(const Discipline& d) {
    if (!d.isValid() || d.id <= 0) { emit error("Некорректные данные"); return false; }
    auto& db = DatabaseManager::instance();

    db.beginTransaction();
    if (!db.executeNonQuery("UPDATE disciplines SET discipline_name=$1 WHERE id_discipline=$2",
                            {d.name, d.id})) {
        db.rollbackTransaction();
        return false;
    }

    db.executeNonQuery("DELETE FROM discipline_types WHERE id_discipline=$1", {d.id});

    int lectureType  = getTypeId("Лекция");
    int practiceType = getTypeId("Практика");

    if (d.lectureRate > 0 && lectureType != -1)
        db.executeNonQuery(
            "INSERT INTO discipline_types (id_discipline, id_type, hourly_rate) VALUES ($1, $2, $3)",
            {d.id, lectureType, d.lectureRate});
    if (d.practiceRate > 0 && practiceType != -1)
        db.executeNonQuery(
            "INSERT INTO discipline_types (id_discipline, id_type, hourly_rate) VALUES ($1, $2, $3)",
            {d.id, practiceType, d.practiceRate});

    db.commitTransaction();
    emit dataChanged();
    return true;
}

bool MethodologistController::deleteDiscipline(int disciplineId) {
    auto& db = DatabaseManager::instance();
    bool result = db.executeNonQuery("DELETE FROM disciplines WHERE id_discipline=$1", {disciplineId});
    if (result) emit dataChanged();
    return result;
}

double MethodologistController::getHourlyRate(int disciplineId, int typeId) {
    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery(
        "SELECT hourly_rate FROM discipline_types WHERE id_discipline=$1 AND id_type=$2",
        {disciplineId, typeId});
    return q.next() ? q.value(0).toDouble() : 0;
}

// специальности
QList<Specialty> MethodologistController::getAllSpecialties() {
    QList<Specialty> specialties;
    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery("SELECT specialty, direction FROM specialties ORDER BY specialty");
    while (q.next()) {
        Specialty s;
        s.name      = q.value(0).toString();
        s.direction = q.value(1).toString();
        specialties.append(s);
    }
    return specialties;
}

bool MethodologistController::addSpecialty(const Specialty& specialty) {
    if (!specialty.isValid()) { emit error("Некорректные данные специальности"); return false; }
    auto& db = DatabaseManager::instance();
    return db.executeNonQuery(
        "INSERT INTO specialties (specialty, direction) VALUES ($1, $2)",
        {specialty.name, specialty.direction});
}

bool MethodologistController::updateSpecialty(const Specialty& specialty) {
    if (!specialty.isValid()) { emit error("Некорректные данные"); return false; }
    auto& db = DatabaseManager::instance();
    return db.executeNonQuery(
        "UPDATE specialties SET direction=$1 WHERE specialty=$2",
        {specialty.direction, specialty.name});
}

bool MethodologistController::deleteSpecialty(const QString& specialtyName) {
    auto& db = DatabaseManager::instance();
    return db.executeNonQuery("DELETE FROM specialties WHERE specialty=$1", {specialtyName});
}

// пользователи и роли
QList<User> MethodologistController::getAllUsers() {
    QList<User> users;
    auto& db = DatabaseManager::instance();

    // сначала получаем всех пользователей
    QSqlQuery q = db.executeQuery("SELECT id_user, login FROM users ORDER BY login");
    QList<QPair<int,QString>> rawUsers;
    while (q.next())
        rawUsers.append({q.value(0).toInt(), q.value(1).toString()});

    // один запрос для всех ролей
    QSqlQuery rq = db.executeQuery(
        "SELECT ur.id_user, r.role_name FROM user_roles ur JOIN roles r ON r.id_role = ur.id_role");
    QMap<int, QList<QString>> rolesMap;
    while (rq.next())
        rolesMap[rq.value(0).toInt()].append(rq.value(1).toString());

    for (const auto& [id, login] : rawUsers) {
        User user(id, login);
        for (const QString& role : rolesMap.value(id))
            user.addRole(role);
        users.append(user);
    }
    return users;
}

User MethodologistController::getUserById(int userId) {
    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery("SELECT id_user, login FROM users WHERE id_user=$1", {userId});
    if (!q.next()) return User();

    User user(q.value(0).toInt(), q.value(1).toString());
    QSqlQuery roleQuery = db.executeQuery(
        "SELECT r.role_name FROM user_roles ur JOIN roles r ON r.id_role = ur.id_role WHERE ur.id_user=$1",
        {user.getId()});
    while (roleQuery.next()) user.addRole(roleQuery.value(0).toString());
    return user;
}

User MethodologistController::getUserByLogin(const QString& login) {
    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery("SELECT id_user, login FROM users WHERE login=$1", {login});
    if (!q.next()) return User();

    User user(q.value(0).toInt(), q.value(1).toString());
    QSqlQuery roleQuery = db.executeQuery(
        "SELECT r.role_name FROM user_roles ur JOIN roles r ON r.id_role = ur.id_role WHERE ur.id_user=$1",
        {user.getId()});
    while (roleQuery.next()) user.addRole(roleQuery.value(0).toString());
    return user;
}

bool MethodologistController::assignRole(int userId, const QString& roleName) {
    int roleId = getRoleId(roleName);
    if (roleId == -1) { emit error("Роль не найдена: " + roleName); return false; }
    auto& db = DatabaseManager::instance();
    return db.executeNonQuery(
        "INSERT INTO user_roles (id_user, id_role) VALUES ($1, $2) ON CONFLICT DO NOTHING",
        {userId, roleId});
}

bool MethodologistController::removeRole(int userId, const QString& roleName) {
    int roleId = getRoleId(roleName);
    if (roleId == -1) { emit error("Роль не найдена: " + roleName); return false; }
    auto& db = DatabaseManager::instance();
    return db.executeNonQuery(
        "DELETE FROM user_roles WHERE id_user=$1 AND id_role=$2", {userId, roleId});
}

bool MethodologistController::hasRole(int userId, const QString& roleName) {
    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery(
        "SELECT COUNT(*) FROM user_roles ur JOIN roles r ON r.id_role = ur.id_role "
        "WHERE ur.id_user=$1 AND r.role_name=$2", {userId, roleName});
    return q.next() && q.value(0).toInt() > 0;
}

QList<QString> MethodologistController::getUserRoles(int userId) {
    QList<QString> roles;
    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery(
        "SELECT r.role_name FROM user_roles ur JOIN roles r ON r.id_role = ur.id_role WHERE ur.id_user=$1",
        {userId});
    while (q.next()) roles.append(q.value(0).toString());
    return roles;
}

bool MethodologistController::makeMethodologist(int userId)  { return assignRole(userId, "methodologist"); }
bool MethodologistController::removeMethodologist(int userId){ return removeRole(userId, "methodologist"); }
bool MethodologistController::makeTeacher(int userId)        { return assignRole(userId, "teacher"); }
bool MethodologistController::removeTeacher(int userId)      { return removeRole(userId, "teacher"); }

bool MethodologistController::createUserWithRoles(const QString& login, const QString& password,
                                                  const QList<QString>& roles) {
    AuthController auth;
    if (!auth.registerUser(login, password)) { emit error("Не удалось создать пользователя"); return false; }
    User user;
    auth.authenticate(login, password, user);
    for (const QString& role : roles) {
        if (!assignRole(user.getId(), role)) {
            emit error("Не удалось назначить роль: " + role);
            return false;
        }
    }
    emit dataChanged();
    return true;
}

QList<User> MethodologistController::getUsersWithRole(const QString& roleName) {
    QList<User> users;
    auto& db = DatabaseManager::instance();
    QSqlQuery q = db.executeQuery(
        "SELECT u.id_user, u.login FROM users u "
        "JOIN user_roles ur ON ur.id_user = u.id_user "
        "JOIN roles r ON r.id_role = ur.id_role "
        "WHERE r.role_name=$1 ORDER BY u.login", {roleName});
    while (q.next())
        users.append(User(q.value(0).toInt(), q.value(1).toString()));
    return users;
}

bool MethodologistController::deleteUser(int userId) {
    auto& db = DatabaseManager::instance();
    return db.executeNonQuery("DELETE FROM users WHERE id_user=$1", {userId});
}
