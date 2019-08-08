/***************************************************************************
 *   elokab Copyright (C) 2014 AbouZakaria <yahiaui@gmail.com>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef PANELWIDGET_H
#define PANELWIDGET_H
#include "status/statuslabel.h"
#include "status/conkystatu.h"


#include <QWidget>
//#include <QX11Info>
#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QProcess>
#include <QLayout>
#include <QFileSystemWatcher>

//#include "xcb/xcb.h"
namespace Ui {
class PanelWidget;
}

#include <X11/Xatom.h>
#include "etaskbar/dtaskbarwidget.h"
#include "dsystray/systray.h"
#include "ewindow/activewindow.h"
#include "epager/pager.h"
#include  <QTimer>

#include <QAbstractNativeEventFilter>
class PanelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PanelWidget(QWidget *parent = nullptr);
    // bool nativeEventFilter(const QByteArray &eventType, void *message, long *);
   void setNativeEventFilter(const QByteArray &eventType, void *message, long *result);
    ~PanelWidget();
public slots:
    // void reconfigure();
    void exit(){qApp->quit();}

    //    void setDock ();
    //unsigned int getWindowPID(Window winID) ;

    void showHide();
private:

    Ui::PanelWidget *ui;
    //  bool mdebug;
    SysTray *mSysTray;
    Pager *mPager;
    conkyStatu  *mConky ;
    DtaskbarWidget *mTaskbar;
    ActiveWindow *mWindow;

    QFileSystemWatcher *mFileSystemWatcher;
    QTimer *m_timer;

    enum Pos{LEFT,CENTER,RIGHT};
    void loadSettings(bool charge=false);
    void chargeStatus(QStringList listLeft,QStringList listCenter,QStringList listRight);
    // QList<StatusLabel *> listWidgets;
    QHash<QString , StatusLabel*> listStatus;
    QStringList listWidget;

QStringList m_listLeft;
QStringList m_listCenter;
QStringList m_listRight;

    QRect m_PaddingRect;
    QRect m_MarginRect;

    //QWindow *tlwWindow ;



    bool m_topPos;
    bool m_isCoposite;

    int m_Screen=0;
    int m_height=0;
    int m_Border=0;



private slots:
    void compositorChanged();
    void reconfigure();
    void resizePanel();
    int calculatSize();
    //X11
    void moveToAllDesktop();
    void setStrut(int top,  int bottom, int topStartX,int topEndX,int bottomStartX, int bottomEndX );

    void addStatus(QStringList list,int pos);
    void addWidget(QWidget *w,int pos);
    void loadIconThems();
    QRect desktopRect();

    //    void  moveToShow();
    //       void  moveToHide();
};

#endif // PANELWIDGET_H
