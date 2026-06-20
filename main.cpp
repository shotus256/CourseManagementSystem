#include <QApplication>
#include <QMessageBox>
#include "View/LoginWindow.h"
#include "View/RoleSelectionWindow.h"
#include "View/MethodologistWindow.h"
#include "View/TeacherWindow.h"
#include "Database/DatabaseManager.h"
#include "Controller/AuthController.h"
#include "Model/User.h"

class ApplicationController : public QObject {
    Q_OBJECT

public:
    ApplicationController(QObject* parent = nullptr) : QObject(parent) {
        auto& db = DatabaseManager::instance();
        if (!db.connect("localhost", "training_courses", "mariaivanova", "123456", 5432)) {
            QMessageBox::critical(nullptr, "Ошибка", "Не удалось подключиться к БД");
            QCoreApplication::exit(1);
            return;
        }

        m_login        = new LoginWindow();
        m_roleSelect   = new RoleSelectionWindow();
        m_methodologist = new MethodologistWindow();
        m_teacher      = new TeacherWindow();

        connect(m_login,         &LoginWindow::loginSuccess,
                this,            &ApplicationController::onLoginSuccess);
        connect(m_roleSelect,    &RoleSelectionWindow::roleSelected,
                this,            &ApplicationController::onRoleSelected);
        connect(m_roleSelect,    &RoleSelectionWindow::logout,
                this,            &ApplicationController::onLogout);
        connect(m_methodologist, &MethodologistWindow::logout,
                this,            &ApplicationController::onLogout);
        connect(m_teacher,       &TeacherWindow::logout,
                this,            &ApplicationController::onLogout);

        m_login->show();
    }

private slots:
    void onLoginSuccess(const User& user) {
        m_currentUser = user;
        m_login->hide();

        if (m_currentUser.getRoles().isEmpty()) {
            QMessageBox::warning(nullptr, "Доступ запрещён",
                                 "У вас нет назначенных ролей.\nОбратитесь к методисту.");
            m_login->show();
            return;
        }

        showWindowForUser();
    }

    void onRoleSelected(int /*userId*/, const QString& role) {
        m_roleSelect->hide();

        if (role == "methodologist") {
            m_methodologist->setUser(m_currentUser);
            m_methodologist->show();
        } else if (role == "teacher") {
            m_teacher->setUser(m_currentUser);
            m_teacher->show();
        }
    }

    void onLogout() {
        m_methodologist->hide();
        m_teacher->hide();
        m_roleSelect->hide();
        m_login->show();
    }

private:
    void showWindowForUser() {
        if (m_currentUser.hasMultipleRoles()) {
            m_roleSelect->setUser(m_currentUser.getId(),
                                  m_currentUser.getLogin(),
                                  m_currentUser.getRoles());
            m_roleSelect->show();
        } else if (m_currentUser.isMethodologist()) {
            m_methodologist->setUser(m_currentUser);
            m_methodologist->show();
        } else if (m_currentUser.isTeacher()) {
            m_teacher->setUser(m_currentUser);
            m_teacher->show();
        } else {
            QMessageBox::warning(nullptr, "Ошибка", "Неизвестная роль");
            m_login->show();
        }
    }

    LoginWindow*          m_login         = nullptr;
    RoleSelectionWindow*  m_roleSelect    = nullptr;
    MethodologistWindow*  m_methodologist = nullptr;
    TeacherWindow*        m_teacher       = nullptr;
    User                  m_currentUser;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ApplicationController controller;
    return app.exec();
}

#include "main.moc"
