#ifndef DEFINES_H
#define DEFINES_H
#include <QObject>
#include <QDebug>

#define MSYSTRAY "Systray"
#define MPAGER "Pager"
#define MTASKBAR "Taskbar"
#define MCONKY   "Conky"
#define MWINDOW   "ActiveWindow"

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
#endif // DEFINES_H
