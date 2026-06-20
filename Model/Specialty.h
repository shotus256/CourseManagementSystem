#ifndef SPECIALTY_H
#define SPECIALTY_H

#include <QString>

class Specialty {
public:
    QString name;
    QString direction;

    bool isValid() const {
        return !name.isEmpty() && !direction.isEmpty();
    }
};

#endif
