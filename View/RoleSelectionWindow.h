#ifndef ROLESELECTIONWINDOW_H
#define ROLESELECTIONWINDOW_H

#include <QWidget>
#include <QList>

namespace Ui { class RoleSelectionWindow; }

class RoleSelectionWindow : public QWidget {
    Q_OBJECT

public:
    explicit RoleSelectionWindow(QWidget* parent = nullptr);
    ~RoleSelectionWindow();

    void setUser(int userId, const QString& login, const QList<QString>& roles);

signals:
    void roleSelected(int userId, const QString& role);
    void logout();

private slots:
    void onRoleClicked();
    void onLogoutClicked();

private:
    Ui::RoleSelectionWindow* ui;
    int m_userId;
    QString m_login;
    QList<QString> m_roles;
};

#endif
