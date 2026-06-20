#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include "Model/User.h"

namespace Ui { class LoginWindow; }

class LoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(QWidget* parent = nullptr);
    ~LoginWindow();

signals:
    void loginSuccess(const User& user);

private slots:
    void onLoginClicked();

private:
    Ui::LoginWindow* ui;
};

#endif
