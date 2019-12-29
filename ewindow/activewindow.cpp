#include "activewindow.h"
#include "utils/stylecolors.h"
#include "utils/defines.h"
#include "utils/setting.h"


#include <QLayout>
#include <QX11Info>
#include "utils/x11utills.h"
#include "xcb/xcb.h"
ActiveWindow::ActiveWindow(QWidget *parent) : QWidget(parent),mParent(parent)
{
    QBoxLayout *hLayout = new QHBoxLayout;
    QBoxLayout *layout = new QHBoxLayout;
    hLayout->setMargin(0);
    hLayout->setSpacing(0);
    layout->setMargin(0);
    layout->setSpacing(0);

    mWidBgr=new QWidget;
    mWidBgr->setObjectName("WidgetBgr");
    btnClose=new MToolButton(this);
    btnClose->setObjectName("BClose");
    btnMax=new MToolButton(this);
    btnMax->setObjectName("BMax");

    btnMin=new MToolButton(this);
    btnMin->setObjectName("BMin");
    labelTitle=new QLabel;

    hLayout->addWidget(btnClose);
    hLayout->addWidget(btnMax);
    hLayout->addWidget(btnMin);
    hLayout->addSpacing(5);
    hLayout->addWidget(labelTitle);

    mWidBgr->setLayout(hLayout);

    layout->addWidget(mWidBgr);
    setLayout(layout);

    mTimer=new QTimer;
    connect(btnClose,SIGNAL(clicked()),this,SLOT(closeActiveWindow()));
    connect(btnMax,SIGNAL(clicked()),this,SLOT(maxRestoreActiveWindow()));
    connect(btnMin,SIGNAL(clicked()),this,SLOT(minRestoreActiveWindow()));
    connect(mTimer,SIGNAL(timeout()),this,SLOT(updateTitle()));

    loadSettings();

  //  qApp->installNativeEventFilter(this);
    activeWindowChanged();

   mTimer->start(3000);
}

//_________________________________________________________________________________________
void ActiveWindow::setNativeEventFilter(const QByteArray &eventType, void *message, long *)
{

    if (eventType == "xcb_generic_event_t") {
        xcb_generic_event_t* event = static_cast<xcb_generic_event_t *>(message);

        switch (event->response_type & ~0x80) {

        case XCB_PROPERTY_NOTIFY: {
            xcb_property_notify_event_t *property = reinterpret_cast<xcb_property_notify_event_t*>(event);

            if(property->atom == X11UTILLS::atom("_NET_ACTIVE_WINDOW")){
                // qDebug()<<"DtaskbarWidget::windowPropertyChanged   _NET_ACTIVE_WINDOW";
               // qDebug()<<"activeWindowChanged->atom"<<property->atom;
                activeWindowChanged();
            }
            break;
        }

        default: break;
        }

    }

   // return false;
}

void ActiveWindow::updateTitle()
{
    if( ! m_window)return;
    QString result=X11UTILLS::getWindowTitle(m_window);
    if(mTitle==result)return;

   setTitle( result);

}

void ActiveWindow::setTitle(QString result)
{
    mTitle=result;
   labelTitle-> setToolTip(QString());
   //qDebug()<<"ActiveWindow::maxSize"<<maxSize;
    if(result.length()>maxSize){
        setToolTip(result);
        result.resize(maxSize-1);
        result+="…";
    }

    labelTitle->setText(result );
}
//____________________________________________________________________________النافذة المفعلة
void ActiveWindow::activeWindowChanged()
{
    if(Defines::degug())  qDebug()<<"   [-]"<<__FILE__<< __LINE__<<" activeWindowChanged()";

    //        البحث عن النافذة المفعلة
    m_window =X11UTILLS::getActiveAppWindow();

    if( ! m_window){
        btnClose->setVisible(false);
        btnMax->setVisible(false);
        btnMin->setVisible(false);
        labelTitle->clear();
        return;
    }


   wState=X11UTILLS::states(m_window);
   wAllow=X11UTILLS::allowed(m_window);

    btnClose->setVisible(true);

    btnMin->setVisible(wAllow["Minimize"] /*&& !wState["Hidden"]*/);
    btnMax->setVisible(wAllow["MaximizeHoriz"] && wAllow["MaximizeVert"]);
    QString result=X11UTILLS::getWindowTitle(m_window);
    setTitle( result);


}


void ActiveWindow::closeActiveWindow()
{
  //  unsigned long window =X11UTILLS::getActiveAppWindow();
    if( ! m_window){
        activeWindowChanged();
        return;
    }
    X11UTILLS::closeWindow(m_window);
}

void ActiveWindow::maxRestoreActiveWindow()
{

   // unsigned long window =X11UTILLS::getActiveAppWindow();
    if( ! m_window){
        activeWindowChanged();
        return;
    }

//    QHash<QString ,bool>wState=X11UTILLS::states(m_window);
//    QHash<QString ,bool>wAllow=X11UTILLS::allowed(m_window);

    if((wAllow["MaximizeHoriz"] && wAllow["MaximizeVert"]) &&
            (!wState["MaximizedHoriz"] || !wState["MaximizedVert"] /*|| wState["Hidden"]*/)){
        X11UTILLS::maximizeWindow(m_window,2);
    }else
        if(wState["Hidden"] || wState["MaximizedHoriz"] || wState["MaximizedVert"]){
            X11UTILLS::deMaximizeWindow(m_window);
        }

 activeWindowChanged();
 X11UTILLS::raiseWindow(m_window);
}

void ActiveWindow::minRestoreActiveWindow()
{

   // unsigned long window =X11UTILLS::getActiveAppWindow();
    if( ! m_window){
        activeWindowChanged();
        return;
    }

//    QHash<QString ,bool>wState=X11UTILLS::states(m_window);
//    QHash<QString ,bool>wAllow=X11UTILLS::allowed(m_window);


    if(wAllow["Minimize"] && !wState["Hidden"]){
        X11UTILLS::minimizeWindow(m_window);
    }
}


//__________________________________________________________________________________
void ActiveWindow::loadSettings()
{
    if(Defines::degug())  qDebug()<<"\033[34m   [-]"<<"ActiveWindow:"<< __LINE__<<"loadSettings()\033[0m";


    //_________________________________________________ Settings
    QString highlight=qApp->palette().highlight().color().name();
    QString highlightTxt=qApp->palette().highlightedText().color().name();

    Setting::instance()->beginGroup(MWINDOW);

    QString bgColor         =Setting::background();
    QString fgColor         =Setting::foreground(mParent->palette().windowText().color().name());
    QString closeColor      =Setting::closeColor(mParent->palette().windowText().color().name());
    QString maxColor        =Setting::maxColor(mParent->palette().windowText().color().name());
    QString minColor        =Setting::minColor(mParent->palette().windowText().color().name());
    QString closeText       =Setting::closeText("x");
    QString maxText         =Setting::maxText("+");
    QString minText         =Setting::minText("-");
    maxSize                 =Setting::maxSize();
    int     alpha           =Setting::alpha();//
    QString underline       =Setting::underline();
    QString overline        =Setting::overline();

    int     border          =Setting::border();
    QString borderColor     =Setting::borderColor();
    int     radius          =Setting::radius();
    QString fontName        =Setting::fontName(mParent->font().family());
    int     fontSize        =Setting::fontSize(mParent->font().pointSize());
    bool    fontBold        =Setting::fontBold(mParent->font().bold());

    //_________________________________________________ INIT
    QFont font;
    font.setFamily(fontName);
    font.setPointSize(fontSize);
    font.setBold(fontBold);
    setFont(font);
    labelTitle->setFont(font);
    btnClose->setFont(font);
    btnMax->setFont(font);
    btnMin->setFont(font);

    btnClose->setText(closeText);
    btnMax->setText(maxText);
    btnMin->setText(minText);
    labelTitle->setText("title test");
    Setting::instance()->endGroup();

    QFontMetrics mtr(font);
    #if QT_VERSION >= QT_VERSION_CHECK(5,10,0)
    int w=mtr.horizontalAdvance(closeText);
#else
     int w=mtr.width(closeText);
#endif
    btnClose->setMaximumWidth(w+5);
    btnMax->setMaximumWidth(w+5);
    btnMin->setMaximumWidth(w+5);
    //-------------------------------------------------------STYLESHEET
    mWidBgr->setContentsMargins((radius+1),1,(radius+1),1);
    QString mStylebg="QWidget#WidgetBgr{";
    mStylebg+=StyleColors::style(bgColor,fgColor,underline,overline,border,alpha,borderColor,radius)+"}\n";

    QString mStyleLab="QLabel{";
    mStyleLab+=StyleColors::style(bgColor,fgColor,QString(),QString(),0,alpha,QString(),0)+"}\n";
    QString mStyleClose="QToolButton{";
    mStyleClose+=StyleColors::style(bgColor,closeColor,QString(),QString(),0,alpha,QString(),0)+"}\n";
    QString mStyleMax="QToolButton{";
    mStyleMax+=StyleColors::style(bgColor,maxColor,QString(),QString(),0,alpha,QString(),0)+"}\n";
    QString mStyleMin="QToolButton{";
    mStyleMin+=StyleColors::style(bgColor,minColor,QString(),QString(),border,alpha,QString(),0)+"}\n";

    setStyleSheet(mStylebg);
    btnClose->setStyleSheet(mStyleClose);
    btnMax->setStyleSheet(mStyleMax);
    btnMin->setStyleSheet(mStyleMin);
    labelTitle->setStyleSheet(mStyleLab);
   // qDebug()<<styleSheet();

    activeWindowChanged();
}
