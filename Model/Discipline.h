#ifndef DISCIPLINE_H
#define DISCIPLINE_H

#include <QString>

class Discipline
{
public:
    Discipline() = default;
    Discipline(int id, const QString& name);

    int id = -1;
    QString name;
    double lectureRate = 0;
    double practiceRate = 0;

    bool isValid() const;
};

#endif
