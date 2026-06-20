#ifndef TEACHERWINDOW_H
#define TEACHERWINDOW_H

#include <QMainWindow>
#include "Model/User.h"

namespace Ui { class TeacherWindow; }

class TeacherWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit TeacherWindow(QWidget* parent = nullptr);
    ~TeacherWindow();

    void setUser(const User& user);

signals:
    void logout();

private slots:
    void onLogout();
    void refreshData();
    void onChangeMyPassword();

private:
    void setupUI();
    void loadMyInfo();
    void loadMyWorkloads();

    Ui::TeacherWindow* ui;
    User m_user;
    int m_teacherId;
};

#endif
