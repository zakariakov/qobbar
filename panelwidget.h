#ifndef PANELWIDGET_H
#define PANELWIDGET_H
#include "status/statuslabel.h"
#include "utils/setting.h"
//#include <QWindow>

#include <QWidget>
#include <QX11Info>
#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QDebug>
#include <QProcess>
#include <QLayout>
#include <QFileSystemWatcher>
//#include <QAbstractNativeEventFilter>
//#include "xcb/xcb.h"
namespace Ui {
class PanelWidget;
}

#include <X11/Xatom.h>
#include "etaskbar/dtaskbarwidget.h"
#include "dsystray/systray.h"
#include "epager/pager.h"

#define _SYSTRAY "Systray"
#define _PAGER "Pager"
#define _TASKBAR "Taskbar"

class PanelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PanelWidget(bool debug=false,QWidget *parent = 0);
    ~PanelWidget();
public slots:
    // void reconfigure();
    void exit(){qApp->quit();}

    void setDock ();
    //unsigned int getWindowPID(Window winID) ;


private:

    Ui::PanelWidget *ui;
    bool mdebug;
    SysTray *mSysTray;
    Pager *mPager;
    DtaskbarWidget *mTaskbar;
    Setting *mSetting;

    enum Pos{LEFT,CENTER,RIGHT};


    void loadSettings(bool charge=false);
void chargeStatus(QStringList listLeft,QStringList listCenter,QStringList listRight);
   // QList<StatusLabel *> listWidgets;
     QHash<QString , StatusLabel*> listStatus;
     QStringList listWidget;
    bool m_topPos;
    QRect mPaddingRect;
    //QWindow *tlwWindow ;
    int mBorder;
    QFileSystemWatcher *mFileSystemWatcher;


private slots:
    void reconfigure(QString);
    void resizePanel();
    void calculatSize();
    //X11
    void moveToAllDesktop();
    void setStrut(int top,  int bottom, int topStartX,int topEndX,int bottomStartX, int bottomEndX );

    void addStatus(QStringList list,int pos);
    void addWidget(QWidget *w,int pos);
    void loadIconThems();

};

#endif // PANELWIDGET_H
