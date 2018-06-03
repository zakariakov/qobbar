#include "panelwidget.h"

#include <QApplication>
#include "panel_adaptor.h"
//#include <QAbstractNativeEventFilter>
#include <QDBusConnection>

void help()
{
    printf("Usage: qobbar [OPTION]\n");
    puts("qobbar v: 0.1 \n" );
    puts("OPTION:\n");
    puts(" -h  --help                      Print this help.");
    puts(" -c  --config     <string>       config file name.");
    puts("                                 ex: create file in $HOME/.config/qobbar/top-bar.conf ");
    puts("                                 run \"qobbar -c top-bar\"  .");
    puts(" -d  --debug                     Print debug in termminal.");
    puts(" -r  --right                     right-to-left layout direction.");
    puts(" -l  --list                      Print list of available modules.");

}

void mylist()
{
    puts("Panel         configured using this name 'Panel'.");
    puts("Pager         configured using this name 'Pager'.");
    puts("Systray       configured using this name 'Systray'.");
    puts("Taskbar       configured using this name 'Taskbar'.");
    puts("Statu         configured using any name ex: 'Cpu' 'Mem'.");

}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("qobbar");
    a.setApplicationName("qobbar");
    a.setApplicationDisplayName("QobBar");


    QStringList args = a.arguments();

    bool debug=false;
    if(args.count()>1)
    {
        for (int i = 0; i < args.count(); ++i) {
            QString arg = args.at(i);

             if (arg == "-h" || arg == "--help" ) {help();return 1; }

             else if (arg == "-c" || arg == "--config" )  {
                 QString conf;
                 if(i+1>args.count()-1){help();return 1;}
                 conf=QString(args.at(i+1));
                 conf.remove(".conf");
                 if(QFile::exists(QDir::homePath()+"/.config/qobbar/"+conf+".conf")){
                     a.setApplicationName(conf);
                 }
             }

             else if (arg == "-d" || arg == "--debug" )  {
                 debug=true;
             }

             else if (arg == "-r" || arg == "--right" )  {
                a.setLayoutDirection(Qt::RightToLeft);
             }

             else if (arg == "-l" || arg == "--list" )  {
                {
                     mylist();
                     return 1;}
                 ;
             }
        }//for
    }//if

    QDBusConnection connection = QDBusConnection::sessionBus();
    if (! QDBusConnection::sessionBus().registerService("org.elokab.panel."+a.applicationName()))
    {
        printf ("Unable to register 'org.elokab.qobbar' service - is another instance of elokab-qobbar running?\n");
        return 1;
    }

    PanelWidget w(debug);

    new panel_adaptor(&w);

    connection.registerObject("/org/elokab/panel/"+a.applicationName(), &w);

    w.show();

    return a.exec();

}
