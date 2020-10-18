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
    static bool hinDonum(){return instance()-> m_hindoNum;}
    static void setHindoNum(bool hindo){instance()->m_hindoNum=hindo;}

    static QString replaceNum(QString str)
    {
        QStringList list;
        list<<"٠"<<"١"<<"٢"<<"٣"<<"٤"<<"٥"<<"٦"<<"٧"<<"٨"<<"٩";

        for (int i=0;i<10 ;i++ ) {
            QString B=list.at(i);
            str=str.replace(QString::number(i),B);
        }
        return str;
    }

private:
    bool m_debug;
    bool m_hindoNum;

};
#endif // DEFINES_H
