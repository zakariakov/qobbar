#include "defines.h"


Q_GLOBAL_STATIC(Defines, DefinesInstance)

Defines *Defines::instance()
{
    return DefinesInstance();
}

