#include "LoginWindow.h"
#include "ui_LoginWindow.h"
#include "Controller/AuthController.h"
#include <QMessageBox>

LoginWindow::LoginWindow(QWidget* parent) : QWidget(parent), ui(new Ui::LoginWindow) {
    ui->setupUi(this);
    setWindowTitle("Вход в систему");
    ui->passwordEdit->setEchoMode(QLineEdit::Password);

    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
}

LoginWindow::~LoginWindow() { delete ui; }

void LoginWindow::onLoginClicked() {
    QString login    = ui->loginEdit->text().trimmed();
    QString password = ui->passwordEdit->text();

    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите логин и пароль");
        return;
    }

    AuthController auth;
    User user;
    if (auth.authenticate(login, password, user)) {
        ui->passwordEdit->clear();
        emit loginSuccess(user);
    } else {
        QMessageBox::warning(this, "Ошибка", "Неверный логин или пароль");
    }
}
