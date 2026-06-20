#include "Discipline.h"

Discipline::Discipline(int id, const QString& name)
    : id(id), name(name)
{
}

bool Discipline::isValid() const
{
    return !name.isEmpty();
}
