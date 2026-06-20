#ifndef METHODOLOGISTWINDOW_H
#define METHODOLOGISTWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include "Database/DatabaseManager.h"
#include "Model/User.h"
#include "Model/Teacher.h"
#include "Model/Workload.h"
#include "Model/Course.h"
#include "Model/Group.h"
#include "Model/Discipline.h"
#include "Model/Specialty.h"

namespace Ui { class MethodologistWindow; }

class MethodologistWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MethodologistWindow(QWidget* parent = nullptr);
    ~MethodologistWindow();
    void setUser(const User& user);

signals:
    void logout();

private slots:
    void onLogout();
    void refreshData();

    // Преподаватели
    void onAddTeacher();
    void onDeleteTeacher();
    void onEditTeacher(const Teacher& teacher);

    // Нагрузка
    void onAddWorkload();
    void onDeleteWorkload();
    void onEditWorkload(const Workload& workload);

    // Курсы
    void onAddCourse();
    void onDeleteCourse();
    void onEditCourse(const Course& course);

    // Группы
    void onAddGroup();
    void onDeleteGroup();
    void onEditGroup(const Group& group);

    // Дисциплины
    void onAddDiscipline();
    void onDeleteDiscipline();
    void onEditDiscipline(const Discipline& discipline);

    // Специальности
    void onAddSpecialty();
    void onDeleteSpecialty();
    void onEditSpecialty(const Specialty& specialty);

    // Пользователи
    void onCreateUser();
    void onAssignRole();
    void onChangePassword();
    void onDeleteUser();
    void onUserManagement();

private:
    void setupUI();
    void setupTable(QTableWidget* table, const QStringList& headers);

    void setupTeachersTab();
    void setupWorkloadsTab();
    void setupCoursesTab();
    void setupGroupsTab();
    void setupDisciplinesTab();
    void setupSpecialtiesTab();

    void loadTeachers();
    void loadWorkloads();
    void loadCourses();
    void loadGroups();
    void loadDisciplines();
    void loadSpecialties();

    Ui::MethodologistWindow* ui;
    User m_user;
};

#endif
