QT += core gui widgets sql

CONFIG += c++17

TEMPLATE = app
TARGET = CourseManagementSystem

INCLUDEPATH += Database Model Controller View

HEADERS += \
    Database/DatabaseManager.h \
    Model/Specialty.h \
    Model/User.h \
    Model/Teacher.h \
    Model/Workload.h \
    Model/Course.h \
    Model/Group.h \
    Model/Discipline.h \
    Controller/AuthController.h \
    Controller/MethodologistController.h \
    Controller/TeacherController.h \
    View/LoginWindow.h \
    View/RoleSelectionWindow.h \
    View/MethodologistWindow.h \
    View/TeacherWindow.h

SOURCES += \
    Model/Specialty.cpp \
    main.cpp \
    Database/DatabaseManager.cpp \
    Model/User.cpp \
    Model/Teacher.cpp \
    Model/Workload.cpp \
    Model/Course.cpp \
    Model/Group.cpp \
    Model/Discipline.cpp \
    Controller/AuthController.cpp \
    Controller/MethodologistController.cpp \
    Controller/TeacherController.cpp \
    View/LoginWindow.cpp \
    View/RoleSelectionWindow.cpp \
    View/MethodologistWindow.cpp \
    View/TeacherWindow.cpp

FORMS += \
    View/LoginWindow.ui \
    View/RoleSelectionWindow.ui \
    View/MethodologistWindow.ui \
    View/TeacherWindow.ui

