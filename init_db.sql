-- курсы
CREATE TABLE courses (
    id_course SERIAL PRIMARY KEY,
    course_name VARCHAR(200) NOT NULL,
    start_date DATE NOT NULL,
    end_date DATE NOT NULL,
    CONSTRAINT chk_course_dates CHECK (end_date >= start_date),
    CONSTRAINT uq_courses UNIQUE (course_name, start_date, end_date)
);

-- специальности
CREATE TABLE specialties (
    specialty VARCHAR(150) PRIMARY KEY,
    direction VARCHAR(150) NOT NULL
);

-- дисциплины
CREATE TABLE disciplines (
    id_discipline SERIAL PRIMARY KEY,
    discipline_name VARCHAR(200) NOT NULL UNIQUE
);

-- типы занятий
CREATE TABLE lesson_types (
    id_type SERIAL PRIMARY KEY,
    type_name VARCHAR(100) NOT NULL UNIQUE
);

-- роли
CREATE TABLE roles (
    id_role SERIAL PRIMARY KEY,
    role_name VARCHAR(100) NOT NULL UNIQUE
);

-- пользователи
CREATE TABLE users (
    id_user SERIAL PRIMARY KEY,
    login VARCHAR(100) NOT NULL UNIQUE,
    password_hash VARCHAR(255) NOT NULL,
    CONSTRAINT chk_login_not_empty CHECK (login <> '')
);

-- преподаватели
CREATE TABLE teachers (
    id_teacher SERIAL PRIMARY KEY,
    id_user INTEGER NOT NULL UNIQUE REFERENCES users(id_user) ON DELETE CASCADE ON UPDATE CASCADE,
    last_name VARCHAR(100) NOT NULL,
    first_name VARCHAR(100) NOT NULL,
    middle_name VARCHAR(100),
    phone VARCHAR(30) NOT NULL UNIQUE,
    experience_years INTEGER NOT NULL DEFAULT 0,
    CONSTRAINT chk_teacher_experience CHECK (experience_years >= 0)
);

-- группы
CREATE TABLE groups (
    id_group SERIAL PRIMARY KEY,
    id_course INTEGER NOT NULL REFERENCES courses(id_course) ON DELETE CASCADE ON UPDATE CASCADE,
    group_name VARCHAR(100) NOT NULL,
    specialty VARCHAR(150) NOT NULL REFERENCES specialties(specialty) ON DELETE RESTRICT ON UPDATE CASCADE,
    student_count INTEGER NOT NULL DEFAULT 0,
    CONSTRAINT chk_student_count CHECK (student_count >= 0),
    CONSTRAINT uq_group_name_course UNIQUE (group_name, id_course)
);

-- нагрузки
CREATE TABLE workloads (
    id_workload SERIAL PRIMARY KEY,
    hours_count NUMERIC(8,2) NOT NULL,
    id_teacher INTEGER NOT NULL REFERENCES teachers(id_teacher) ON DELETE CASCADE ON UPDATE CASCADE,
    id_group INTEGER NOT NULL REFERENCES groups(id_group) ON DELETE CASCADE ON UPDATE CASCADE,
    id_discipline INTEGER NOT NULL REFERENCES disciplines(id_discipline) ON DELETE CASCADE ON UPDATE CASCADE,
    id_type INTEGER NOT NULL REFERENCES lesson_types(id_type) ON DELETE CASCADE ON UPDATE CASCADE,
    CONSTRAINT chk_hours_count CHECK (hours_count > 0),
    CONSTRAINT uq_workload_business UNIQUE (id_group, id_discipline, id_type)
);

-- курсы_дисциплины
CREATE TABLE course_disciplines (
    id_course INTEGER NOT NULL REFERENCES courses(id_course) ON DELETE CASCADE ON UPDATE CASCADE,
    id_discipline INTEGER NOT NULL REFERENCES disciplines(id_discipline) ON DELETE CASCADE ON UPDATE CASCADE,
    PRIMARY KEY (id_course, id_discipline)
);

-- преподаватели_дисциплины
CREATE TABLE teacher_disciplines (
    id_teacher INTEGER NOT NULL REFERENCES teachers(id_teacher) ON DELETE CASCADE ON UPDATE CASCADE,
    id_discipline INTEGER NOT NULL REFERENCES disciplines(id_discipline) ON DELETE CASCADE ON UPDATE CASCADE,
    PRIMARY KEY (id_teacher, id_discipline)
);

-- дисциплины_типы
CREATE TABLE discipline_types (
    id_discipline INTEGER NOT NULL REFERENCES disciplines(id_discipline) ON DELETE CASCADE ON UPDATE CASCADE,
    id_type INTEGER NOT NULL REFERENCES lesson_types(id_type) ON DELETE CASCADE ON UPDATE CASCADE,
    hourly_rate NUMERIC(10,2) NOT NULL,
    PRIMARY KEY (id_discipline, id_type),
    CONSTRAINT chk_hourly_rate CHECK (hourly_rate > 0)
);

-- пользователи_роли
CREATE TABLE user_roles (
    id_user INTEGER NOT NULL REFERENCES users(id_user) ON DELETE CASCADE ON UPDATE CASCADE,
    id_role INTEGER NOT NULL REFERENCES roles(id_role) ON DELETE CASCADE ON UPDATE CASCADE,
    PRIMARY KEY (id_user, id_role)
);

-- добавляем роли
INSERT INTO roles (role_name) VALUES ('methodologist'), ('teacher');

-- добавляем типы занятий
INSERT INTO lesson_types (type_name) VALUES ('Лекция'), ('Практика');
