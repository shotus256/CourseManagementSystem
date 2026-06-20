#include "TeacherWindow.h"
#include "ui_TeacherWindow.h"
#include "Controller/TeacherController.h"
#include "Controller/AuthController.h"
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QDialog>

TeacherWindow::TeacherWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::TeacherWindow), m_teacherId(-1) {
    ui->setupUi(this);
    setupUI();
}

TeacherWindow::~TeacherWindow() { delete ui; }

void TeacherWindow::setupUI() {
    setWindowTitle("Личный кабинет преподавателя");

    ui->workloadsTable->setColumnCount(5);
    ui->workloadsTable->setHorizontalHeaderLabels({"Группа", "Дисциплина", "Тип", "Часы", "Ставка"});
    ui->workloadsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->actionLogout, &QAction::triggered, this, &TeacherWindow::onLogout);
    connect(ui->actionRefresh, &QAction::triggered, this, &TeacherWindow::refreshData);
    connect(ui->actionChangePassword, &QAction::triggered, this, &TeacherWindow::onChangeMyPassword);
}

void TeacherWindow::setUser(const User& user) {
    m_user = user;
    statusBar()->showMessage("Преподаватель: " + user.getLogin());

    TeacherController controller;
    Teacher teacher = controller.getMyInfo(user.getId());
    m_teacherId = teacher.id;

    if (m_teacherId > 0) {
        loadMyInfo();
        loadMyWorkloads();
    } else {
        QMessageBox::warning(this, "Ошибка", "Не найден профиль преподавателя");
    }
}

void TeacherWindow::loadMyInfo() {
    TeacherController controller;
    Teacher teacher = controller.getMyInfo(m_user.getId());

    ui->nameLabel->setText("ФИО: " + teacher.getFullName());
    ui->phoneLabel->setText("Телефон: " + teacher.phone);
    ui->experienceLabel->setText("Стаж: " + QString::number(teacher.experienceYears) + " лет");

    double totalHours = controller.getMyTotalHours(m_teacherId);
    double totalPayment = controller.getMyTotalPayment(m_teacherId);

    ui->hoursLabel->setText("Всего часов: " + QString::number(totalHours));
    ui->paymentLabel->setText("Общая оплата: " + QString::number(totalPayment) + " руб.");
}

void TeacherWindow::loadMyWorkloads() {
    if (m_teacherId <= 0) return;

    TeacherController controller;
    auto workloads = controller.getMyWorkloads(m_teacherId);

    ui->workloadsTable->setRowCount(workloads.size());
    for (int i = 0; i < workloads.size(); ++i) {
        const auto& w = workloads[i];
        ui->workloadsTable->setItem(i, 0, new QTableWidgetItem(w.groupName));
        ui->workloadsTable->setItem(i, 1, new QTableWidgetItem(w.disciplineName));
        ui->workloadsTable->setItem(i, 2, new QTableWidgetItem(w.typeName));
        ui->workloadsTable->setItem(i, 3, new QTableWidgetItem(QString::number(w.hoursCount)));
        ui->workloadsTable->setItem(i, 4, new QTableWidgetItem(QString::number(w.hourlyRate) + " руб."));
    }
}

void TeacherWindow::refreshData() {
    loadMyInfo();
    loadMyWorkloads();
    statusBar()->showMessage("Данные обновлены", 3000);
}

void TeacherWindow::onLogout() {
    if (QMessageBox::question(this, "Выход", "Вы уверены?") == QMessageBox::Yes)
        emit logout();
}

void TeacherWindow::onChangeMyPassword() {
    QDialog dialog(this);
    dialog.setWindowTitle("Сменить пароль");
    dialog.setModal(true);
    dialog.resize(400, 180);

    QFormLayout* layout = new QFormLayout(&dialog);

    QLineEdit* oldPassEdit = new QLineEdit(&dialog);
    oldPassEdit->setEchoMode(QLineEdit::Password);
    QLineEdit* newPassEdit = new QLineEdit(&dialog);
    newPassEdit->setEchoMode(QLineEdit::Password);
    QLineEdit* confirmEdit = new QLineEdit(&dialog);
    confirmEdit->setEchoMode(QLineEdit::Password);

    layout->addRow("Старый пароль:", oldPassEdit);
    layout->addRow("Новый пароль:", newPassEdit);
    layout->addRow("Подтверждение:", confirmEdit);

    QDialogButtonBox* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addRow(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    QString oldPass = oldPassEdit->text();
    QString newPass = newPassEdit->text();
    QString confirm = confirmEdit->text();

    if (newPass != confirm) {
        QMessageBox::warning(this, "Ошибка", "Пароли не совпадают");
        return;
    }

    QString error;
    if (!AuthController::validatePassword(newPass, error)) {
        QMessageBox::warning(this, "Ошибка", "Пароль: " + error);
        return;
    }

    AuthController auth;
    if (auth.changePassword(m_user.getId(), oldPass, newPass)) {
        QMessageBox::information(this, "Успех", "Пароль изменён!");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось изменить пароль. Проверьте правильность старого пароля.");
    }
}
