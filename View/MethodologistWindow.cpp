#include "MethodologistWindow.h"
#include "ui_MethodologistWindow.h"
#include "Controller/MethodologistController.h"
#include "Controller/AuthController.h"
#include "Model/Specialty.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QPushButton>
#include <QLabel>
#include <QDialog>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDateEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QMenu>
#include <QLineEdit>
#include <QRegularExpression>

MethodologistWindow::MethodologistWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MethodologistWindow) {
    ui->setupUi(this);
    setupUI();
}

MethodologistWindow::~MethodologistWindow() { delete ui; }

void MethodologistWindow::setupUI() {
    setWindowTitle("Панель методиста - Управление курсами");

    setupTeachersTab();
    setupWorkloadsTab();
    setupCoursesTab();
    setupGroupsTab();
    setupDisciplinesTab();
    setupSpecialtiesTab();

    if (!ui->menubar) return;
    ui->menubar->clear();

    auto* fileMenu = ui->menubar->addMenu("Файл");
    fileMenu->addAction("Обновить", this, &MethodologistWindow::refreshData);
    fileMenu->addAction("Выход", this, &MethodologistWindow::onLogout);

    auto* editMenu = ui->menubar->addMenu("Правка");
    editMenu->addAction("Добавить преподавателя", this, &MethodologistWindow::onAddTeacher);
    editMenu->addAction("Удалить преподавателя", this, &MethodologistWindow::onDeleteTeacher);
    editMenu->addSeparator();
    editMenu->addAction("Добавить нагрузку", this, &MethodologistWindow::onAddWorkload);
    editMenu->addAction("Удалить нагрузку", this, &MethodologistWindow::onDeleteWorkload);
    editMenu->addSeparator();
    editMenu->addAction("Добавить курс", this, &MethodologistWindow::onAddCourse);
    editMenu->addAction("Удалить курс", this, &MethodologistWindow::onDeleteCourse);
    editMenu->addSeparator();
    editMenu->addAction("Добавить группу", this, &MethodologistWindow::onAddGroup);
    editMenu->addAction("Удалить группу", this, &MethodologistWindow::onDeleteGroup);
    editMenu->addSeparator();
    editMenu->addAction("Добавить дисциплину", this, &MethodologistWindow::onAddDiscipline);
    editMenu->addAction("Удалить дисциплину", this, &MethodologistWindow::onDeleteDiscipline);
    editMenu->addSeparator();
    editMenu->addAction("Добавить специальность", this, &MethodologistWindow::onAddSpecialty);
    editMenu->addAction("Удалить специальность", this, &MethodologistWindow::onDeleteSpecialty);

    auto* userMenu = ui->menubar->addMenu("Пользователи");
    userMenu->addAction("Создать пользователя", this, &MethodologistWindow::onCreateUser);
    userMenu->addAction("Назначить роль", this, &MethodologistWindow::onAssignRole);
    userMenu->addAction("Сменить пароль", this, &MethodologistWindow::onChangePassword);
    userMenu->addAction("Удалить пользователя", this, &MethodologistWindow::onDeleteUser);
    userMenu->addSeparator();
    userMenu->addAction("Список пользователей", this, &MethodologistWindow::onUserManagement);
}

void MethodologistWindow::setUser(const User& user) {
    m_user = user;
    statusBar()->showMessage("Методист: " + user.getLogin());
    refreshData();
}

void MethodologistWindow::setupTable(QTableWidget* table, const QStringList& headers) {
    table->setColumnCount(headers.size());
    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MethodologistWindow::setupTeachersTab() {
    setupTable(ui->teachersTable, {"ID", "ФИО", "Телефон", "Стаж", "ID пользователя", ""});
}

void MethodologistWindow::setupWorkloadsTab() {
    setupTable(ui->workloadsTable, {"ID", "Преподаватель", "Группа", "Дисциплина", "Тип", "Часы", "Оплата", ""});
}

void MethodologistWindow::setupCoursesTab() {
    setupTable(ui->coursesTable, {"ID", "Название", "Начало", "Окончание", ""});
}

void MethodologistWindow::setupGroupsTab() {
    setupTable(ui->groupsTable, {"ID", "Название", "Курс", "Специальность", "Студентов", ""});
}

void MethodologistWindow::setupDisciplinesTab() {
    setupTable(ui->disciplinesTable, {"ID", "Название", "Ставка (лекция)", "Ставка (практика)", ""});
}

void MethodologistWindow::setupSpecialtiesTab() {
    setupTable(ui->specialtiesTable, {"Название", "Направление", ""});
}

//загрузка данных

void MethodologistWindow::loadTeachers() {
    MethodologistController controller;
    auto teachers = controller.getAllTeachers();
    ui->teachersTable->setRowCount(teachers.size());
    for (int i = 0; i < teachers.size(); ++i) {
        const auto& t = teachers[i];
        ui->teachersTable->setItem(i, 0, new QTableWidgetItem(QString::number(t.id)));
        ui->teachersTable->setItem(i, 1, new QTableWidgetItem(t.getFullName()));
        ui->teachersTable->setItem(i, 2, new QTableWidgetItem(t.phone));
        ui->teachersTable->setItem(i, 3, new QTableWidgetItem(QString::number(t.experienceYears)));
        ui->teachersTable->setItem(i, 4, new QTableWidgetItem(QString::number(t.userId)));

        auto* btn = new QPushButton("Изменить");
        connect(btn, &QPushButton::clicked, [this, t]() { onEditTeacher(t); });
        ui->teachersTable->setCellWidget(i, 5, btn);
    }
}

void MethodologistWindow::loadWorkloads() {
    MethodologistController controller;
    auto workloads = controller.getAllWorkloads();
    ui->workloadsTable->setRowCount(workloads.size());
    for (int i = 0; i < workloads.size(); ++i) {
        const auto& w = workloads[i];
        ui->workloadsTable->setItem(i, 0, new QTableWidgetItem(QString::number(w.id)));
        ui->workloadsTable->setItem(i, 1, new QTableWidgetItem(w.teacherName));
        ui->workloadsTable->setItem(i, 2, new QTableWidgetItem(w.groupName));
        ui->workloadsTable->setItem(i, 3, new QTableWidgetItem(w.disciplineName));
        ui->workloadsTable->setItem(i, 4, new QTableWidgetItem(w.typeName));
        ui->workloadsTable->setItem(i, 5, new QTableWidgetItem(QString::number(w.hoursCount)));
        ui->workloadsTable->setItem(i, 6, new QTableWidgetItem(
                                              QString::number(w.getTotalPayment(), 'f', 2) + " руб."));

        auto* btn = new QPushButton("Изменить");
        connect(btn, &QPushButton::clicked, [this, w]() { onEditWorkload(w); });
        ui->workloadsTable->setCellWidget(i, 7, btn);
    }
}

void MethodologistWindow::loadCourses() {
    MethodologistController controller;
    auto courses = controller.getAllCourses();
    ui->coursesTable->setRowCount(courses.size());
    for (int i = 0; i < courses.size(); ++i) {
        const auto& c = courses[i];
        ui->coursesTable->setItem(i, 0, new QTableWidgetItem(QString::number(c.id)));
        ui->coursesTable->setItem(i, 1, new QTableWidgetItem(c.name));
        ui->coursesTable->setItem(i, 2, new QTableWidgetItem(c.startDate.toString("dd.MM.yyyy")));
        ui->coursesTable->setItem(i, 3, new QTableWidgetItem(c.endDate.toString("dd.MM.yyyy")));

        auto* btn = new QPushButton("Изменить");
        connect(btn, &QPushButton::clicked, [this, c]() { onEditCourse(c); });
        ui->coursesTable->setCellWidget(i, 4, btn);
    }
}

void MethodologistWindow::loadGroups() {
    MethodologistController controller;
    auto groups = controller.getAllGroups();
    QMap<int, QString> courseNames;
    for (const auto& c : controller.getAllCourses())
        courseNames[c.id] = c.name;

    ui->groupsTable->setRowCount(groups.size());
    for (int i = 0; i < groups.size(); ++i) {
        const auto& g = groups[i];
        ui->groupsTable->setItem(i, 0, new QTableWidgetItem(QString::number(g.id)));
        ui->groupsTable->setItem(i, 1, new QTableWidgetItem(g.name));
        ui->groupsTable->setItem(i, 2, new QTableWidgetItem(
                                           courseNames.value(g.courseId, QString::number(g.courseId))));
        ui->groupsTable->setItem(i, 3, new QTableWidgetItem(g.specialty));
        ui->groupsTable->setItem(i, 4, new QTableWidgetItem(QString::number(g.studentCount)));

        auto* btn = new QPushButton("Изменить");
        connect(btn, &QPushButton::clicked, [this, g]() { onEditGroup(g); });
        ui->groupsTable->setCellWidget(i, 5, btn);
    }
}

void MethodologistWindow::loadDisciplines() {
    MethodologistController controller;
    auto disciplines = controller.getAllDisciplines();
    ui->disciplinesTable->setRowCount(disciplines.size());
    for (int i = 0; i < disciplines.size(); ++i) {
        const auto& d = disciplines[i];
        ui->disciplinesTable->setItem(i, 0, new QTableWidgetItem(QString::number(d.id)));
        ui->disciplinesTable->setItem(i, 1, new QTableWidgetItem(d.name));
        ui->disciplinesTable->setItem(i, 2, new QTableWidgetItem(QString::number(d.lectureRate, 'f', 2)));
        ui->disciplinesTable->setItem(i, 3, new QTableWidgetItem(QString::number(d.practiceRate, 'f', 2)));

        auto* btn = new QPushButton("Изменить");
        connect(btn, &QPushButton::clicked, [this, d]() { onEditDiscipline(d); });
        ui->disciplinesTable->setCellWidget(i, 4, btn);
    }
}

void MethodologistWindow::loadSpecialties() {
    MethodologistController controller;
    auto specialties = controller.getAllSpecialties();
    ui->specialtiesTable->setRowCount(specialties.size());
    for (int i = 0; i < specialties.size(); ++i) {
        const auto& s = specialties[i];
        ui->specialtiesTable->setItem(i, 0, new QTableWidgetItem(s.name));
        ui->specialtiesTable->setItem(i, 1, new QTableWidgetItem(s.direction));

        auto* btn = new QPushButton("Изменить");
        connect(btn, &QPushButton::clicked, [this, s]() { onEditSpecialty(s); });
        ui->specialtiesTable->setCellWidget(i, 2, btn);
    }
}

void MethodologistWindow::refreshData() {
    loadTeachers();
    loadWorkloads();
    loadCourses();
    loadGroups();
    loadDisciplines();
    loadSpecialties();
    statusBar()->showMessage("Данные обновлены", 3000);
}

void MethodologistWindow::onLogout() {
    if (QMessageBox::question(this, "Выход", "Вы уверены?") == QMessageBox::Yes)
        emit logout();
}

// преподаватели

void MethodologistWindow::onAddTeacher() {
    MethodologistController controller;
    auto users = controller.getUsersWithRole("teacher");
    if (users.isEmpty()) {
        QMessageBox::information(this, "Информация",
                                 "Нет пользователей с ролью 'преподаватель'.\n"
                                 "Сначала создайте пользователя через:\n"
                                 "Пользователи → Создать пользователя");
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Добавить преподавателя");
    dialog.setModal(true);
    dialog.resize(400, 300);
    QFormLayout* layout = new QFormLayout(&dialog);

    QComboBox* userCombo = new QComboBox(&dialog);
    for (const auto& u : users)
        userCombo->addItem(u.getLogin(), u.getId());

    QLineEdit* lastNameEdit  = new QLineEdit(&dialog);
    QLineEdit* firstNameEdit = new QLineEdit(&dialog);
    QLineEdit* middleNameEdit= new QLineEdit(&dialog);
    QLineEdit* phoneEdit     = new QLineEdit(&dialog);
    QSpinBox*  expSpin       = new QSpinBox(&dialog);
    expSpin->setRange(0, 60);

    layout->addRow("Пользователь:", userCombo);
    layout->addRow("Фамилия:",      lastNameEdit);
    layout->addRow("Имя:",          firstNameEdit);
    layout->addRow("Отчество:",     middleNameEdit);
    layout->addRow("Телефон:",      phoneEdit);
    layout->addRow("Стаж (лет):",   expSpin);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    if (lastNameEdit->text().trimmed().isEmpty() || firstNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Фамилия и имя обязательны");
        return;
    }

    Teacher t;
    t.userId        = userCombo->currentData().toInt();
    t.lastName      = lastNameEdit->text().trimmed();
    t.firstName     = firstNameEdit->text().trimmed();
    t.middleName    = middleNameEdit->text().trimmed();
    t.phone         = phoneEdit->text().trimmed();
    t.experienceYears = expSpin->value();

    if (controller.addTeacher(t)) {
        QMessageBox::information(this, "Успех", "Преподаватель добавлен!");
        loadTeachers();
    }
}

void MethodologistWindow::onDeleteTeacher() {
    int row = ui->teachersTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "Ошибка", "Выберите преподавателя"); return; }

    int id = ui->teachersTable->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Удаление", "Удалить преподавателя?") == QMessageBox::Yes) {
        MethodologistController controller;
        if (controller.deleteTeacher(id)) loadTeachers();
    }
}

void MethodologistWindow::onEditTeacher(const Teacher& teacher) {
    QDialog dialog(this);
    dialog.setWindowTitle("Изменить преподавателя");
    dialog.setModal(true);
    dialog.resize(400, 280);
    QFormLayout* layout = new QFormLayout(&dialog);

    QLineEdit* lastNameEdit   = new QLineEdit(teacher.lastName,   &dialog);
    QLineEdit* firstNameEdit  = new QLineEdit(teacher.firstName,  &dialog);
    QLineEdit* middleNameEdit = new QLineEdit(teacher.middleName, &dialog);
    QLineEdit* phoneEdit      = new QLineEdit(teacher.phone,      &dialog);
    QSpinBox*  expSpin        = new QSpinBox(&dialog);
    expSpin->setRange(0, 60);
    expSpin->setValue(teacher.experienceYears);

    layout->addRow("Фамилия:",    lastNameEdit);
    layout->addRow("Имя:",        firstNameEdit);
    layout->addRow("Отчество:",   middleNameEdit);
    layout->addRow("Телефон:",    phoneEdit);
    layout->addRow("Стаж (лет):", expSpin);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    if (lastNameEdit->text().trimmed().isEmpty() || firstNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Фамилия и имя обязательны");
        return;
    }

    Teacher updated = teacher;
    updated.lastName      = lastNameEdit->text().trimmed();
    updated.firstName     = firstNameEdit->text().trimmed();
    updated.middleName    = middleNameEdit->text().trimmed();
    updated.phone         = phoneEdit->text().trimmed();
    updated.experienceYears = expSpin->value();

    MethodologistController controller;
    if (controller.updateTeacher(updated)) {
        QMessageBox::information(this, "Успех", "Преподаватель обновлен!");
        loadTeachers();
    }
}

// нагрузка

void MethodologistWindow::onAddWorkload() {
    MethodologistController controller;
    auto teachers   = controller.getAllTeachers();
    auto groups     = controller.getAllGroups();
    auto disciplines = controller.getAllDisciplines();

    if (teachers.isEmpty())   { QMessageBox::warning(this, "Ошибка", "Нет преподавателей"); return; }
    if (groups.isEmpty())     { QMessageBox::warning(this, "Ошибка", "Нет групп");          return; }
    if (disciplines.isEmpty()){ QMessageBox::warning(this, "Ошибка", "Нет дисциплин");      return; }

    QDialog dialog(this);
    dialog.setWindowTitle("Добавить нагрузку");
    dialog.setModal(true);
    dialog.resize(420, 300);
    QFormLayout* layout = new QFormLayout(&dialog);

    QComboBox* teacherCombo    = new QComboBox(&dialog);
    QComboBox* groupCombo      = new QComboBox(&dialog);
    QComboBox* disciplineCombo = new QComboBox(&dialog);
    QComboBox* typeCombo       = new QComboBox(&dialog);
    QDoubleSpinBox* hoursSpin  = new QDoubleSpinBox(&dialog);
    hoursSpin->setRange(2, 2000);
    hoursSpin->setSingleStep(2);
    hoursSpin->setValue(2);
    hoursSpin->setSuffix(" ч.");

    for (const auto& t : teachers)
        teacherCombo->addItem(t.getFullName(), t.id);
    for (const auto& g : groups)
        groupCombo->addItem(g.name, g.id);
    for (const auto& d : disciplines)
        disciplineCombo->addItem(d.name, d.id);

    {
        auto& db = DatabaseManager::instance();
        QSqlQuery q = db.executeQuery("SELECT id_type, type_name FROM lesson_types ORDER BY type_name");
        while (q.next())
            typeCombo->addItem(q.value(1).toString(), q.value(0).toInt());
    }

    if (typeCombo->count() == 0) {
        QMessageBox::warning(this, "Ошибка", "Нет типов занятий в базе данных");
        return;
    }

    layout->addRow("Преподаватель:", teacherCombo);
    layout->addRow("Группа:",        groupCombo);
    layout->addRow("Дисциплина:",    disciplineCombo);
    layout->addRow("Тип занятия:",   typeCombo);
    layout->addRow("Часов:",         hoursSpin);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    Workload w;
    w.hoursCount   = hoursSpin->value();
    w.teacherId    = teacherCombo->currentData().toInt();
    w.groupId      = groupCombo->currentData().toInt();
    w.disciplineId = disciplineCombo->currentData().toInt();
    w.typeId       = typeCombo->currentData().toInt();

    if (controller.addWorkload(w)) {
        loadWorkloads();
        QMessageBox::information(this, "Успех", "Нагрузка добавлена!");
    }
}

void MethodologistWindow::onDeleteWorkload() {
    int row = ui->workloadsTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "Ошибка", "Выберите нагрузку"); return; }

    int id = ui->workloadsTable->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Удаление", "Удалить нагрузку?") == QMessageBox::Yes) {
        MethodologistController controller;
        if (controller.deleteWorkload(id)) loadWorkloads();
    }
}

void MethodologistWindow::onEditWorkload(const Workload& workload) {
    MethodologistController controller;
    auto teachers    = controller.getAllTeachers();
    auto groups      = controller.getAllGroups();
    auto disciplines = controller.getAllDisciplines();

    QDialog dialog(this);
    dialog.setWindowTitle("Изменить нагрузку");
    dialog.setModal(true);
    dialog.resize(420, 300);
    QFormLayout* layout = new QFormLayout(&dialog);

    QComboBox* teacherCombo    = new QComboBox(&dialog);
    QComboBox* groupCombo      = new QComboBox(&dialog);
    QComboBox* disciplineCombo = new QComboBox(&dialog);
    QComboBox* typeCombo       = new QComboBox(&dialog);
    QDoubleSpinBox* hoursSpin  = new QDoubleSpinBox(&dialog);
    hoursSpin->setRange(2, 2000);
    hoursSpin->setSingleStep(2);
    hoursSpin->setSuffix(" ч.");
    hoursSpin->setValue(workload.hoursCount);

    for (const auto& t : teachers) {
        teacherCombo->addItem(t.getFullName(), t.id);
        if (t.id == workload.teacherId)
            teacherCombo->setCurrentIndex(teacherCombo->count() - 1);
    }
    for (const auto& g : groups) {
        groupCombo->addItem(g.name, g.id);
        if (g.id == workload.groupId)
            groupCombo->setCurrentIndex(groupCombo->count() - 1);
    }
    for (const auto& d : disciplines) {
        disciplineCombo->addItem(d.name, d.id);
        if (d.id == workload.disciplineId)
            disciplineCombo->setCurrentIndex(disciplineCombo->count() - 1);
    }

    {
        auto& db = DatabaseManager::instance();
        QSqlQuery q = db.executeQuery("SELECT id_type, type_name FROM lesson_types ORDER BY type_name");
        while (q.next()) {
            int tid = q.value(0).toInt();
            typeCombo->addItem(q.value(1).toString(), tid);
            if (tid == workload.typeId)
                typeCombo->setCurrentIndex(typeCombo->count() - 1);
        }
    }

    layout->addRow("Преподаватель:", teacherCombo);
    layout->addRow("Группа:",        groupCombo);
    layout->addRow("Дисциплина:",    disciplineCombo);
    layout->addRow("Тип занятия:",   typeCombo);
    layout->addRow("Часов:",         hoursSpin);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    Workload updated = workload;
    updated.hoursCount   = hoursSpin->value();
    updated.teacherId    = teacherCombo->currentData().toInt();
    updated.groupId      = groupCombo->currentData().toInt();
    updated.disciplineId = disciplineCombo->currentData().toInt();
    updated.typeId       = typeCombo->currentData().toInt();

    if (controller.updateWorkload(updated)) {
        QMessageBox::information(this, "Успех", "Нагрузка обновлена!");
        loadWorkloads();
    }
}

// курсы

void MethodologistWindow::onAddCourse() {
    QDialog dialog(this);
    dialog.setWindowTitle("Добавить курс");
    dialog.setModal(true);
    dialog.resize(400, 250);

    QFormLayout* layout = new QFormLayout(&dialog);
    QLineEdit* nameEdit  = new QLineEdit(&dialog);
    QDateEdit* startDate = new QDateEdit(&dialog);
    startDate->setCalendarPopup(true);
    startDate->setDate(QDate::currentDate());
    QDateEdit* endDate = new QDateEdit(&dialog);
    endDate->setCalendarPopup(true);
    endDate->setDate(QDate::currentDate().addMonths(3));

    layout->addRow("Название:",     nameEdit);
    layout->addRow("Дата начала:",  startDate);
    layout->addRow("Дата окончания:", endDate);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    QString name = nameEdit->text().trimmed();
    if (name.isEmpty()) { QMessageBox::warning(this, "Ошибка", "Введите название курса"); return; }

    Course c;
    c.name      = name;
    c.startDate = startDate->date();
    c.endDate   = endDate->date();

    MethodologistController controller;
    if (controller.addCourse(c)) {
        QMessageBox::information(this, "Успех", "Курс добавлен!");
        loadCourses();
    }
}

void MethodologistWindow::onDeleteCourse() {
    int row = ui->coursesTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "Ошибка", "Выберите курс"); return; }

    int id = ui->coursesTable->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Удаление", "Удалить курс?") == QMessageBox::Yes) {
        MethodologistController controller;
        if (controller.deleteCourse(id)) { loadCourses(); loadGroups(); }
    }
}

void MethodologistWindow::onEditCourse(const Course& course) {
    QDialog dialog(this);
    dialog.setWindowTitle("Изменить курс");
    dialog.setModal(true);
    dialog.resize(400, 250);

    QFormLayout* layout = new QFormLayout(&dialog);
    QLineEdit* nameEdit  = new QLineEdit(course.name, &dialog);
    QDateEdit* startDate = new QDateEdit(&dialog);
    startDate->setCalendarPopup(true);
    startDate->setDate(course.startDate);
    QDateEdit* endDate = new QDateEdit(&dialog);
    endDate->setCalendarPopup(true);
    endDate->setDate(course.endDate);

    layout->addRow("Название:",     nameEdit);
    layout->addRow("Дата начала:",  startDate);
    layout->addRow("Дата окончания:", endDate);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    QString name = nameEdit->text().trimmed();
    if (name.isEmpty()) { QMessageBox::warning(this, "Ошибка", "Введите название курса"); return; }

    Course c = course;
    c.name      = name;
    c.startDate = startDate->date();
    c.endDate   = endDate->date();

    MethodologistController controller;
    if (controller.updateCourse(c)) {
        QMessageBox::information(this, "Успех", "Курс обновлен!");
        loadCourses();
    }
}

// группы
void MethodologistWindow::onAddGroup() {
    MethodologistController controller;
    auto courses    = controller.getAllCourses();
    auto specialties = controller.getAllSpecialties();

    if (courses.isEmpty())     { QMessageBox::warning(this, "Ошибка", "Сначала создайте курс"); return; }
    if (specialties.isEmpty()) { QMessageBox::warning(this, "Ошибка", "Сначала добавьте специальности"); return; }

    QDialog dialog(this);
    dialog.setWindowTitle("Добавить группу");
    dialog.setModal(true);
    dialog.resize(400, 250);

    QFormLayout* layout = new QFormLayout(&dialog);

    QComboBox* courseCombo    = new QComboBox(&dialog);
    QComboBox* specialtyCombo = new QComboBox(&dialog);
    QLineEdit* nameEdit       = new QLineEdit(&dialog);
    QSpinBox*  countSpin      = new QSpinBox(&dialog);
    countSpin->setRange(0, 100);
    countSpin->setValue(10);

    for (const auto& c : courses)     courseCombo->addItem(c.name, c.id);
    for (const auto& s : specialties) specialtyCombo->addItem(s.name + " (" + s.direction + ")", s.name);

    layout->addRow("Курс:",                courseCombo);
    layout->addRow("Название группы:",     nameEdit);
    layout->addRow("Специальность:",       specialtyCombo);
    layout->addRow("Количество студентов:", countSpin);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    QString name = nameEdit->text().trimmed();
    if (name.isEmpty()) { QMessageBox::warning(this, "Ошибка", "Введите название группы"); return; }

    Group g;
    g.courseId    = courseCombo->currentData().toInt();
    g.name        = name;
    g.specialty   = specialtyCombo->currentData().toString();
    g.studentCount = countSpin->value();

    if (controller.addGroup(g)) {
        QMessageBox::information(this, "Успех", "Группа добавлена!");
        loadGroups();
    }
}

void MethodologistWindow::onDeleteGroup() {
    int row = ui->groupsTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "Ошибка", "Выберите группу"); return; }

    int id = ui->groupsTable->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Удаление", "Удалить группу?") == QMessageBox::Yes) {
        MethodologistController controller;
        if (controller.deleteGroup(id)) loadGroups();
    }
}

void MethodologistWindow::onEditGroup(const Group& group) {
    MethodologistController controller;
    auto courses    = controller.getAllCourses();
    auto specialties = controller.getAllSpecialties();

    QDialog dialog(this);
    dialog.setWindowTitle("Изменить группу");
    dialog.setModal(true);
    dialog.resize(400, 250);

    QFormLayout* layout = new QFormLayout(&dialog);

    QComboBox* courseCombo    = new QComboBox(&dialog);
    QComboBox* specialtyCombo = new QComboBox(&dialog);
    QLineEdit* nameEdit       = new QLineEdit(group.name, &dialog);
    QSpinBox*  countSpin      = new QSpinBox(&dialog);
    countSpin->setRange(0, 100);
    countSpin->setValue(group.studentCount);

    for (const auto& c : courses) {
        courseCombo->addItem(c.name, c.id);
        if (c.id == group.courseId) courseCombo->setCurrentIndex(courseCombo->count() - 1);
    }
    for (const auto& s : specialties) {
        specialtyCombo->addItem(s.name + " (" + s.direction + ")", s.name);
        if (s.name == group.specialty) specialtyCombo->setCurrentIndex(specialtyCombo->count() - 1);
    }

    layout->addRow("Курс:",                courseCombo);
    layout->addRow("Название группы:",     nameEdit);
    layout->addRow("Специальность:",       specialtyCombo);
    layout->addRow("Количество студентов:", countSpin);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    QString name = nameEdit->text().trimmed();
    if (name.isEmpty()) { QMessageBox::warning(this, "Ошибка", "Введите название группы"); return; }

    Group g = group;
    g.courseId     = courseCombo->currentData().toInt();
    g.name         = name;
    g.specialty    = specialtyCombo->currentData().toString();
    g.studentCount = countSpin->value();

    if (controller.updateGroup(g)) {
        QMessageBox::information(this, "Успех", "Группа обновлена!");
        loadGroups();
    }
}

// дисциплины

void MethodologistWindow::onAddDiscipline() {
    QDialog dialog(this);
    dialog.setWindowTitle("Добавить дисциплину");
    dialog.setModal(true);
    dialog.resize(400, 200);

    QFormLayout* layout = new QFormLayout(&dialog);
    QLineEdit*      nameEdit    = new QLineEdit(&dialog);
    QDoubleSpinBox* lectureRate = new QDoubleSpinBox(&dialog);
    lectureRate->setRange(0, 10000);
    lectureRate->setValue(1000);
    lectureRate->setPrefix("₽ ");
    QDoubleSpinBox* practiceRate = new QDoubleSpinBox(&dialog);
    practiceRate->setRange(0, 10000);
    practiceRate->setValue(800);
    practiceRate->setPrefix("₽ ");

    layout->addRow("Название:",         nameEdit);
    layout->addRow("Ставка (лекция):",  lectureRate);
    layout->addRow("Ставка (практика):", practiceRate);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    QString name = nameEdit->text().trimmed();
    if (name.isEmpty()) { QMessageBox::warning(this, "Ошибка", "Введите название дисциплины"); return; }

    Discipline d;
    d.name        = name;
    d.lectureRate = lectureRate->value();
    d.practiceRate = practiceRate->value();

    MethodologistController controller;
    if (controller.addDiscipline(d)) {
        QMessageBox::information(this, "Успех", "Дисциплина добавлена!");
        loadDisciplines();
    }
}

void MethodologistWindow::onDeleteDiscipline() {
    int row = ui->disciplinesTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "Ошибка", "Выберите дисциплину"); return; }

    int id = ui->disciplinesTable->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Удаление", "Удалить дисциплину?") == QMessageBox::Yes) {
        MethodologistController controller;
        if (controller.deleteDiscipline(id)) loadDisciplines();
    }
}

void MethodologistWindow::onEditDiscipline(const Discipline& discipline) {
    QDialog dialog(this);
    dialog.setWindowTitle("Изменить дисциплину");
    dialog.setModal(true);
    dialog.resize(400, 200);

    QFormLayout* layout = new QFormLayout(&dialog);
    QLineEdit*      nameEdit    = new QLineEdit(discipline.name, &dialog);
    QDoubleSpinBox* lectureRate = new QDoubleSpinBox(&dialog);
    lectureRate->setRange(0, 10000);
    lectureRate->setValue(discipline.lectureRate);
    lectureRate->setPrefix("₽ ");
    QDoubleSpinBox* practiceRate = new QDoubleSpinBox(&dialog);
    practiceRate->setRange(0, 10000);
    practiceRate->setValue(discipline.practiceRate);
    practiceRate->setPrefix("₽ ");

    layout->addRow("Название:",         nameEdit);
    layout->addRow("Ставка (лекция):",  lectureRate);
    layout->addRow("Ставка (практика):", practiceRate);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    QString name = nameEdit->text().trimmed();
    if (name.isEmpty()) { QMessageBox::warning(this, "Ошибка", "Введите название дисциплины"); return; }

    Discipline d = discipline;
    d.name        = name;
    d.lectureRate = lectureRate->value();
    d.practiceRate = practiceRate->value();

    MethodologistController controller;
    if (controller.updateDiscipline(d)) {
        QMessageBox::information(this, "Успех", "Дисциплина обновлена!");
        loadDisciplines();
    }
}

// специальности

void MethodologistWindow::onAddSpecialty() {
    QDialog dialog(this);
    dialog.setWindowTitle("Добавить специальность");
    dialog.setModal(true);
    dialog.resize(400, 150);

    QFormLayout* layout = new QFormLayout(&dialog);
    QLineEdit* nameEdit      = new QLineEdit(&dialog);
    QLineEdit* directionEdit = new QLineEdit(&dialog);

    layout->addRow("Название:",    nameEdit);
    layout->addRow("Направление:", directionEdit);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    QString name      = nameEdit->text().trimmed();
    QString direction = directionEdit->text().trimmed();
    if (name.isEmpty() || direction.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля");
        return;
    }

    Specialty s;
    s.name      = name;
    s.direction = direction;

    MethodologistController controller;
    if (controller.addSpecialty(s)) {
        QMessageBox::information(this, "Успех", "Специальность добавлена!");
        loadSpecialties();
    }
}

void MethodologistWindow::onDeleteSpecialty() {
    int row = ui->specialtiesTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "Ошибка", "Выберите специальность"); return; }

    QString name = ui->specialtiesTable->item(row, 0)->text();
    if (QMessageBox::question(this, "Удаление", "Удалить специальность '" + name + "'?") == QMessageBox::Yes) {
        MethodologistController controller;
        if (controller.deleteSpecialty(name)) {
            loadSpecialties();
            QMessageBox::information(this, "Успех", "Специальность удалена");
        }
    }
}

void MethodologistWindow::onEditSpecialty(const Specialty& specialty) {
    QDialog dialog(this);
    dialog.setWindowTitle("Изменить специальность");
    dialog.setModal(true);
    dialog.resize(400, 150);

    QFormLayout* layout = new QFormLayout(&dialog);
    QLineEdit* nameEdit      = new QLineEdit(specialty.name, &dialog);
    nameEdit->setEnabled(false);
    QLineEdit* directionEdit = new QLineEdit(specialty.direction, &dialog);

    layout->addRow("Название:",    nameEdit);
    layout->addRow("Направление:", directionEdit);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    QString direction = directionEdit->text().trimmed();
    if (direction.isEmpty()) { QMessageBox::warning(this, "Ошибка", "Введите направление"); return; }

    Specialty s = specialty;
    s.direction = direction;

    MethodologistController controller;
    if (controller.updateSpecialty(s)) {
        QMessageBox::information(this, "Успех", "Специальность обновлена!");
        loadSpecialties();
    }
}

// пользователи

void MethodologistWindow::onCreateUser() {
    QDialog dialog(this);
    dialog.setWindowTitle("Создание пользователя");
    dialog.setModal(true);
    dialog.resize(400, 250);

    QFormLayout* layout = new QFormLayout(&dialog);
    QLineEdit* loginEdit    = new QLineEdit(&dialog);
    QLineEdit* passwordEdit = new QLineEdit(&dialog);
    passwordEdit->setEchoMode(QLineEdit::Password);
    QComboBox* roleCombo = new QComboBox(&dialog);
    roleCombo->addItem("Преподаватель", "teacher");
    roleCombo->addItem("Методист",      "methodologist");
    roleCombo->addItem("Без роли",      "");

    layout->addRow("Логин:",  loginEdit);
    layout->addRow("Пароль:", passwordEdit);
    layout->addRow("Роль:",   roleCombo);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    QString login    = loginEdit->text().trimmed();
    QString password = passwordEdit->text();
    QString role     = roleCombo->currentData().toString();

    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля");
        return;
    }

    QString err;
    if (!AuthController::validatePassword(password, err)) {
        QMessageBox::warning(this, "Ошибка", "Пароль: " + err);
        return;
    }

    AuthController auth;
    if (!auth.registerUser(login, password)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось создать пользователя");
        return;
    }

    User user;
    auth.authenticate(login, password, user);

    MethodologistController controller;
    QString msg;
    if (role == "teacher") {
        controller.makeTeacher(user.getId());
        msg = "Преподаватель создан!";
    } else if (role == "methodologist") {
        controller.makeMethodologist(user.getId());
        msg = "Методист создан!";
    } else {
        msg = "Пользователь создан без ролей!";
    }
    QMessageBox::information(this, "Успех", msg + "\nЛогин: " + login);
    refreshData();
}

void MethodologistWindow::onAssignRole() {
    MethodologistController controller;
    auto users = controller.getAllUsers();
    if (users.isEmpty()) { QMessageBox::information(this, "Информация", "Нет пользователей"); return; }

    QDialog dialog(this);
    dialog.setWindowTitle("Назначить роль");
    dialog.setModal(true);
    dialog.resize(400, 150);
    QFormLayout* layout = new QFormLayout(&dialog);

    QComboBox* userCombo = new QComboBox(&dialog);
    for (const auto& u : users) {
        QString roles = u.getRoles().join(", ");
        if (roles.isEmpty()) roles = "(нет ролей)";
        userCombo->addItem(QString("%1 [%2]").arg(u.getLogin(), roles), u.getId());
    }

    QComboBox* roleCombo = new QComboBox(&dialog);
    roleCombo->addItem("Преподаватель", "teacher");
    roleCombo->addItem("Методист",      "methodologist");

    layout->addRow("Пользователь:", userCombo);
    layout->addRow("Роль:",         roleCombo);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    int userId  = userCombo->currentData().toInt();
    QString role = roleCombo->currentData().toString();

    if (controller.assignRole(userId, role)) {
        QMessageBox::information(this, "Успех", "Роль назначена!");
        refreshData();
    }
}

void MethodologistWindow::onChangePassword() {
    MethodologistController controller;
    auto users = controller.getAllUsers();
    if (users.isEmpty()) { QMessageBox::information(this, "Информация", "Нет пользователей"); return; }

    QDialog dialog(this);
    dialog.setWindowTitle("Сменить пароль");
    dialog.setModal(true);
    dialog.resize(400, 150);
    QFormLayout* layout = new QFormLayout(&dialog);

    QComboBox* userCombo    = new QComboBox(&dialog);
    QLineEdit* passwordEdit = new QLineEdit(&dialog);
    passwordEdit->setEchoMode(QLineEdit::Password);

    for (const auto& u : users)
        userCombo->addItem(u.getLogin(), u.getId());

    layout->addRow("Пользователь:", userCombo);
    layout->addRow("Новый пароль:", passwordEdit);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    int userId      = userCombo->currentData().toInt();
    QString newPass = passwordEdit->text();

    QString err;
    if (!AuthController::validatePassword(newPass, err)) {
        QMessageBox::warning(this, "Ошибка", "Пароль: " + err);
        return;
    }

    AuthController auth;
    if (auth.resetPassword(userId, newPass)) {
        QMessageBox::information(this, "Успех", "Пароль изменён!");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось изменить пароль");
    }
}

void MethodologistWindow::onDeleteUser() {
    MethodologistController controller;
    auto users = controller.getAllUsers();
    if (users.isEmpty()) { QMessageBox::information(this, "Информация", "Нет пользователей"); return; }

    QDialog dialog(this);
    dialog.setWindowTitle("Удалить пользователя");
    dialog.setModal(true);
    dialog.resize(400, 120);
    QFormLayout* layout = new QFormLayout(&dialog);

    QComboBox* userCombo = new QComboBox(&dialog);
    for (const auto& u : users) {
        if (u.getId() == m_user.getId()) continue;
        userCombo->addItem(u.getLogin(), u.getId());
    }

    if (userCombo->count() == 0) {
        QMessageBox::information(this, "Информация", "Нет пользователей для удаления");
        return;
    }

    layout->addRow("Пользователь:", userCombo);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    int userId = userCombo->currentData().toInt();
    if (QMessageBox::question(this, "Удаление",
                              "Удалить пользователя '" + userCombo->currentText() + "'?") == QMessageBox::Yes) {
        if (controller.deleteUser(userId)) {
            QMessageBox::information(this, "Успех", "Пользователь удалён");
            refreshData();
        }
    }
}

void MethodologistWindow::onUserManagement() {
    MethodologistController controller;
    auto users = controller.getAllUsers();
    if (users.isEmpty()) { QMessageBox::information(this, "Информация", "Нет пользователей"); return; }

    QStringList items;
    for (const auto& u : users) {
        QString roles = u.getRoles().join(", ");
        if (roles.isEmpty()) roles = "(нет ролей)";
        items << QString("ID: %1 | %2 | Роли: %3").arg(u.getId()).arg(u.getLogin(), roles);
    }
    QMessageBox::information(this, "Список пользователей", items.join("\n"));
}
