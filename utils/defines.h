#ifndef DEFINES_H
#define DEFINES_H
#include <QObject>
#include <QDebug>
#endif // DEFINES_H


class Defines
{

public:

    Defines(){}

    static Defines *instance();
    static bool degug(){return instance()-> m_debug;}
    static void setDeguging(bool debug){instance()->m_debug=debug;}
private:
    bool m_debug;

};
