#include "RoleSelectionWindow.h"
#include "ui_RoleSelectionWindow.h"
#include <QPushButton>
#include <QVBoxLayout>

RoleSelectionWindow::RoleSelectionWindow(QWidget* parent)
    : QWidget(parent), ui(new Ui::RoleSelectionWindow), m_userId(-1) {
    ui->setupUi(this);
    setWindowTitle("Выбор роли");
    connect(ui->logoutButton, &QPushButton::clicked, this, &RoleSelectionWindow::onLogoutClicked);
}

RoleSelectionWindow::~RoleSelectionWindow() { delete ui; }

void RoleSelectionWindow::setUser(int userId, const QString& login, const QList<QString>& roles) {
    m_userId = userId;
    m_login = login;
    m_roles = roles;
    ui->userLabel->setText("Пользователь: " + login);

    auto* layout = ui->buttonsLayout;
    while (layout->count() > 0) delete layout->takeAt(0)->widget();

    for (const QString& role : roles) {
        auto* btn = new QPushButton(this);
        QString displayName = (role == "methodologist") ? "Методист" :
                                  (role == "teacher") ? "Преподаватель" : role;
        btn->setText(displayName);
        btn->setProperty("role", role);
        btn->setMinimumHeight(50);
        layout->addWidget(btn);
        connect(btn, &QPushButton::clicked, this, &RoleSelectionWindow::onRoleClicked);
    }
}

void RoleSelectionWindow::onRoleClicked() {
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (btn) emit roleSelected(m_userId, btn->property("role").toString());
}

void RoleSelectionWindow::onLogoutClicked() {
    emit logout();
}
