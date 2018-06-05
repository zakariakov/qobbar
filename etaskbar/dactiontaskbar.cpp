/********************************************************************
 *(c)GPL3+

 original code  http://razor-qt.org
 modified by abou zakaria
*********************************************************************/


#include "dactiontaskbar.h"


#include <QtGui/QContextMenuEvent>

#include <QDir>
#include<QWidget>
#include<QToolButton>
#include<QMenu>
#include<QStyle>
#include "utils/x11utills.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#define  ABOVE 0
#define  NORMAL 1
#define  BELOW 2

#define _MaximizeHoriz 0
#define _MaximizeVert 1
#define _MaximizeBoth 2
DActionTaskbar::DActionTaskbar(const Window window, QWidget *parent) :
          QToolButton(parent),
          m_Window(window)
{


    this->setCheckable(true);
   setContentsMargins(0,0,0,0);

   this->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
   this->setToolButtonStyle(Qt::ToolButtonIconOnly);

    QFont font=parent->font();
   font.setPointSize(parent->font().pointSize());

   setFont(font);

   QFontMetrics fm(font);
   int size=fm.height()-2;
   if(size<16) size=16;
  // qDebug()<<"DActionTaskbar::DActionTaskbar()================================iconSize()"<<size;
  setIconSize(QSize(size,size));

          getText();
          getIcon();
        this->setMaximumWidth(parent->height()+6);

}

QString getIconFromDesktopApp(QString fileName )
{



          QFile file(QDir::homePath()+".config/elokab/applictions");

          if (!file.open(QFile::ReadOnly)){
                    return "";
          }
          QTextStream textStream(&file);
          QString line;

          line= textStream.readLine();//premier line
          while (!line.isNull()) {

                    QString exec=line.section("|",1,1);

                    if(exec.contains(fileName)){
                            //  qDebug()<<"contain :"<<exec;
                            //  QString name=line.section("|",0,0);
                              QString icon=line.section("|",3,3);
                              file.close();
                              return icon;

                    }
                    line= textStream.readLine();
          }
          file.close();
          return "";
}

void DActionTaskbar::getIcon()
{
    QIcon icon;

    //from appname
    icon=QIcon::fromTheme(appName());
    if(!icon.isNull()){
        QPixmap pixmap = icon.pixmap(QSize(32, 32),QIcon::Active,QIcon::Off);
        setIcon(QIcon(pixmap));
       // qDebug()<<"getIcon from appname-------------";
        return;
    }
    //from classe name
    icon=QIcon::fromTheme(classeName().toLower());
    if(!icon.isNull()){
        QPixmap pixmap = icon.pixmap(QSize(32, 32),QIcon::Active,QIcon::Off);
        setIcon(QIcon(pixmap));
       // qDebug()<<"getIcon from classename-------------";
        return;
    }


     //from xdgappname
    QString xdgicon=getIconFromDesktopApp(appName());
    // qDebug()<<"getIconFromDesktopAppappName xdgicon------------"<<xdgicon;
    icon=QIcon::fromTheme(xdgicon);
    if(!icon.isNull()){
        QPixmap pixmap = icon.pixmap(QSize(32, 32),QIcon::Active,QIcon::Off);
        setIcon(QIcon(pixmap));
         //qDebug()<<"getIconFromDesktopAppappName------------";
        return;
    }
    //from xdgclassename
    xdgicon=getIconFromDesktopApp(classeName());
   icon=QIcon::fromTheme(xdgicon);
   if(!icon.isNull()){
       QPixmap pixmap = icon.pixmap(QSize(32, 32),QIcon::Active,QIcon::Off);
       setIcon(QIcon(pixmap));
      //  qDebug()<<"getIconFromDesktopAppclasseName-------------";
       return;
   }

          QPixmap pix;
          if (X11UTILLS::getClientIcon(m_Window, pix))
                    setIcon(QIcon(pix));
          else
                    setIcon(QIcon::fromTheme("application-x-executable"));
}
QString DActionTaskbar::appName()
{

    return  X11UTILLS::getApplicationName(m_Window);

}
QString DActionTaskbar::classeName()
{

    return  X11UTILLS::getApplicationClasseName(m_Window);

}
void DActionTaskbar::getText()
{
          QString title = X11UTILLS::getWindowTitle(m_Window);
         // setText(title.replace("&", "&&"));
          setToolTip(title);
          //     qDebug()<<"title-------------------------------------"<<title;
}

void DActionTaskbar::contextMenuEvent(QContextMenuEvent* event)
{


          QHash<QString ,bool>wState=X11UTILLS::states(m_Window);
          QHash<QString ,bool>wAllow=X11UTILLS::allowed(m_Window);




          QMenu menu(tr("Application"));
          QAction* a;



          // ** قوائم اسطح المكتب **************************
          int deskNum = X11UTILLS::getNumDesktop();
          if (deskNum > 1)
          {
                    int winDesk = X11UTILLS::getWindowDesktop(m_Window);
                    QMenu* deskMenu = menu.addMenu(tr("To &Desktop"));

                    a = deskMenu->addAction(tr("&All Desktops"));
                    a->setData(-1);
                    a->setEnabled(winDesk != -1);
                    connect(a, SIGNAL(triggered(bool)), this, SLOT(moveWindowToDesktop()));
                    deskMenu->addSeparator();

                    for (int i=0; i<deskNum; ++i)
                    {
                              a = deskMenu->addAction(tr("Desktop &%1").arg(i+1));
                              a->setData(i);
                              a->setEnabled( i != winDesk );
                              connect(a, SIGNAL(triggered(bool)), this, SLOT(moveWindowToDesktop()));
                    }

                    int curDesk = X11UTILLS::getActiveDesktop();
                    a = menu.addAction(tr("&To Current Desktop"));
                    a->setData(curDesk);
                    a->setEnabled( curDesk != winDesk );
                    connect(a, SIGNAL(triggered(bool)), this, SLOT(moveWindowToDesktop()));
          }

          // **قوائم اوامر النوافذ ****************************
          menu.addSeparator();

          a = menu.addAction(QIcon(style()->standardPixmap(QStyle::SP_TitleBarMaxButton)),tr("Ma&ximize"));
          a->setEnabled((wAllow["MaximizeHoriz"] && wAllow["MaximizeVert"]) &&
                        (!wState["MaximizedHoriz"] || !wState["MaximizedVert"] /*|| wState["Hidden"]*/));
       a->setData(_MaximizeBoth);
          connect(a, SIGNAL(triggered(bool)), this, SLOT(maximizeWindow()));

          if (event->modifiers() & Qt::ShiftModifier)
          {

                    a = menu.addAction(QIcon(style()->standardPixmap(QStyle::SP_TitleBarMaxButton)),tr("Maximize vertically"));
              a->setEnabled((wAllow["MaximizeVert"]) && (!wState["MaximizedVert"] || wState["Hidden"]));
                    a->setData(_MaximizeVert);
                    connect(a, SIGNAL(triggered(bool)), this, SLOT(maximizeWindow()));

                    a = menu.addAction(QIcon(style()->standardPixmap(QStyle::SP_TitleBarMaxButton)),tr("Maximize horizontally"));
            a->setEnabled((wAllow["MaximizeHoriz"]) && (!wState["MaximizedHoriz"] || wState["Hidden"]));
                    a->setData(_MaximizeHoriz);
                    connect(a, SIGNAL(triggered(bool)), this, SLOT(maximizeWindow()));
          }

          a = menu.addAction(QIcon(style()->standardPixmap(QStyle::SP_TitleBarNormalButton)),tr("&Restore"));
        a->setEnabled(wState["Hidden"] || wState["MaximizedHoriz"] || wState["MaximizedVert"]);
          connect(a, SIGNAL(triggered(bool)), this, SLOT(unMaximizeWindow()));

          a = menu.addAction(QIcon(style()->standardPixmap(QStyle::SP_TitleBarMinButton)),tr("Mi&nimize"));
      a->setEnabled(wAllow["Minimize"] && !wState["Hidden"]);
          connect(a, SIGNAL(triggered(bool)), this, SLOT(minimizeWindow()));

          if (wState["Shaded"])
          {
                    a = menu.addAction(QIcon(style()->standardPixmap(QStyle::SP_TitleBarUnshadeButton)),tr("Roll down"));
                    a->setEnabled(wAllow["Shade"] && !wState["Hidden"]);
                    connect(a, SIGNAL(triggered(bool)), this, SLOT(unShadeWindow()));
          }
          else
          {
                    a = menu.addAction(QIcon(style()->standardPixmap(QStyle::SP_TitleBarShadeButton)),tr("Roll up"));
                    a->setEnabled(wAllow["Shade"] && !wState["Hidden"]);
                    connect(a, SIGNAL(triggered(bool)), this, SLOT(shadeWindow()));
          }


          // **قوائم وضعية النوافذ ****************************
          menu.addSeparator();

          QMenu* layerMenu = menu.addMenu(tr("&Layer"));

          a = layerMenu->addAction(tr("Always on &top"));
          a->setEnabled(wAllow["AboveLayer"] && !wState["AboveLayer"]);
          a->setData(ABOVE);
          connect(a, SIGNAL(triggered(bool)), this, SLOT(setApplicationLayer()));

          a = layerMenu->addAction(tr("&Normal"));
          a->setEnabled(wState["AboveLayer"] || wState["BelowLayer"]);
          a->setData( NORMAL);
          connect(a, SIGNAL(triggered(bool)), this, SLOT(setApplicationLayer()));

          a = layerMenu->addAction(tr("Always on &bottom"));
          a->setEnabled(wAllow["BelowLayer"] && !wState["BelowLayer"]);
          a->setData(BELOW);
          connect(a, SIGNAL(triggered(bool)), this, SLOT(setApplicationLayer()));


          // ** امر اغلاق النافذة *****************************
          menu.addSeparator();
          a = menu.addAction(QIcon::fromTheme("process-stop",QIcon(style()->standardPixmap(QStyle::SP_TitleBarCloseButton))), tr("&Close"));
          connect(a, SIGNAL(triggered(bool)), this, SLOT(closeApplication()));


          menu.exec(event->globalPos());

}
/************************************************
                   عملية سحب الملفات
 ************************************************/
void DActionTaskbar::dragEnterEvent(QDragEnterEvent *event)
{
    mimeData = event->mimeData();
//    qDebug()<<"mimeData"<<mimeData->text();
    QTimer::singleShot(1000, this, SLOT(activateWithDraggable()));
}

void DActionTaskbar::dragLeaveEvent(QDragLeaveEvent */*event*/)
{
    mimeData = NULL;
}

void DActionTaskbar::activateWithDraggable()
{
    if (!mimeData/* || mimeData->text().isEmpty()*/)
        return;

   // raiseApplication();
    int winDesktop = X11UTILLS::getWindowDesktop(m_Window);
    if (X11UTILLS::getActiveDesktop() != winDesktop)
        X11UTILLS::setActiveDesktop(winDesktop);

    X11UTILLS::raiseWindow(m_Window);
}



//**********************************عمليات النوافذ***************************
void DActionTaskbar::mousePressEvent(QMouseEvent *event)
{
          if (event->button() == Qt::LeftButton)
          {
//              qDebug()<<"mActive"<<mActive;
                    if (mActive) {
                              minimizeWindow();
                              setActiveWin(false);
                    }else {
                              restoreWindow();
                                X11UTILLS::raiseWindow(m_Window);
                               setActiveWin(true);
                    }
          }

}
//****************تصغير النافذة*********************
void DActionTaskbar::minimizeWindow()
{
          X11UTILLS::minimizeWindow(m_Window);
}
//****************استعادة النافذة*********************
void DActionTaskbar::restoreWindow()
{
          int winDesktop =  X11UTILLS::getWindowDesktop(m_Window);
          if ( X11UTILLS::getActiveDesktop() != winDesktop)
                    X11UTILLS::setActiveDesktop(winDesktop);
           X11UTILLS::raiseWindow(m_Window);
}
//************نقل النافذة الى سطح مكتب****************
void DActionTaskbar::moveWindowToDesktop()
{
          QAction* act = qobject_cast<QAction*>(sender());
          if (!act)
                    return;

          bool ok;
          int desk = act->data().toInt(&ok);

          if (!ok)
                    return;

           X11UTILLS::moveWindowToDesktop(m_Window, desk);
}
//************استعادة النافذة من التكبير****************
void DActionTaskbar::unMaximizeWindow()
{
           X11UTILLS::deMaximizeWindow(m_Window);
}
//************تحجيم النافذة اي نظليلها****************
void DActionTaskbar::shadeWindow()
{
           X11UTILLS::shadeWindow(m_Window, true);
}
//************استعادة النافذة من التحجيم****************
void DActionTaskbar::unShadeWindow()
{
           X11UTILLS::shadeWindow(m_Window, false);
}
//******************غلق النافذة**********************
void DActionTaskbar::closeApplication()
{
           X11UTILLS::closeWindow(m_Window);
}
//******************تكبير النافذة**********************
void DActionTaskbar::maximizeWindow()
{
          QAction* act = qobject_cast<QAction*>(sender());
          if (!act)
                    return;



          X11UTILLS::maximizeWindow(m_Window,act->data().toInt() );
}
//******************وضع النافذة فوق او تحت او عادي*************
void DActionTaskbar::setApplicationLayer()
{
          QAction* act = qobject_cast<QAction*>(sender());
          if (!act)
                    return;

          X11UTILLS::setWindowLayer(m_Window,act->data().toInt());
}

//********************

void DActionTaskbar::windowPropertyChanged(unsigned long /*atom*/)
{
    getText();

//    if(atom == X11UTILLS::atom("_NET_WM_WINDOW_TYPE") || atom == X11UTILLS::atom("_NET_WM_STATE"))
//    {
//    qDebug()<<    "updateVisibility();";
//    }

//    if(atom == X11UTILLS::atom("_NET_WM_VISIBLE_NAME") || atom == X11UTILLS::atom("_NET_WM_NAME") || atom == X11UTILLS::atom("WM_NAME"))
//    {
//    qDebug()<<        "updateName();";
//    }

//    if(atom == X11UTILLS::atom("_NET_WM_ICON"))
//    {
//       qDebug()<< "updateIcon();";
//    }

//    if(atom == X11UTILLS::atom("WM_HINTS"))
//    {
//     qDebug()<<   "updateUrgency();";
//    }
}
void DActionTaskbar::setActiveWin(bool arg)
{setChecked(arg);
    mActive=arg;}
