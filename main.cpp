#include "panelwidget.h"

#include <QApplication>
#include "panel_adaptor.h"
//#include <QAbstractNativeEventFilter>
#include <QDBusConnection>

void help()
{
    printf("Usage: qobbar [OPTION]\n");
    puts("qobbar v: 0.2 \n" );
    puts("OPTION:\n");
    puts(" -h  --help                      Print this help.");
    puts(" -c  --config     <string>       config file name.");
    puts("                                 ex: create file in $HOME/.config/qobbar/top-bar.conf ");
    puts("                                 run \"qobbar -c top-bar\"  .");
    puts(" -d  --debug                     Print debug in termminal.");
    puts(" -r  --right                     right-to-left layout direction.");
    puts(" -R  --reconfig                  Reconfigure the bar ex:\"qobbar -R -c top-bar\".");
    puts(" -x  --exit                      close the bar ex:\"qobbar -x -c top-bar\".");
    puts(" -s  --showhide                  show or hide bar. ex: \"qobbar -s -c top-bar\".");
    puts(" -l  --list                      Print list of available modules.");

}

void mylist()
{
    puts("Panel         configured using this name 'Panel'.");
    puts("Pager         configured using this name 'Pager'.");
    puts("Taskbar       configured using this name 'Taskbar'.");
    puts("Conky         configured using this name 'Conky'.");
    puts("Statu         configured using any name ex: 'Cpu' 'Mem'.");

}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("qobbar");
    a.setApplicationName("qobbar");
    a.setApplicationDisplayName("QobBar");
    a.setLayoutDirection(Qt::LeftToRight);

    QStringList args = a.arguments();
    bool hide=false;
    bool exit=false;
    bool reconfig=false;
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

            else if (arg == "-d" || arg == "--debug" )   { debug=true;}
            else if (arg == "-s" || arg == "--showhide") { hide=true; }
            else if (arg == "-x" || arg == "--exit" )    {exit=true; }
            else if (arg == "-R" || arg == "--reconfig") {reconfig=true;}
            else if (arg == "-r" || arg == "--right" )   { a.setLayoutDirection(Qt::RightToLeft);}
            else if (arg == "-l" || arg == "--list" )    { mylist(); return 1;}

        }//for
    }//if

    QDBusConnection connection = QDBusConnection::sessionBus();
    if (! QDBusConnection::sessionBus().registerService("org.elokab.panel."+a.applicationName()))
    {

        printf ("Unable to register 'org.elokab.qobbar' service - is another instance of elokab-qobbar running?\n");

        QDBusInterface dbus("org.elokab.panel."+a.applicationName(),
                            "/org/elokab/panel/"+a.applicationName(),
                            "org.elokab.panel.Interface");

        if (!dbus.isValid()) { printf ("QDBusInterface is not valid!");return 0; }
        if (hide)            {dbus.call("showHide"); return 0;}
        if (exit)            {dbus.call("exit"); return 0;}
        if (reconfig)        {dbus.call("reconfigure"); return 0;}

        return 0;
    }

    PanelWidget w(debug);

    new panel_adaptor(&w);

    connection.registerObject("/org/elokab/panel/"+a.applicationName(), &w);

    w.show();

    return a.exec();

}
