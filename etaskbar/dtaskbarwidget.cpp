/********************************************************************
 *(c)GPL3+

original code  http://razor-qt.org
 modified by abou zakaria
*********************************************************************/
#include "dtaskbarwidget.h"
#include "dactiontaskbar.h"
//#include "taskbarsettingdialog.h"
#include "utils/stylecolors.h"
#include <QtGui>
#include <QX11Info>
#include <QDesktopWidget>
#include <QMenu>
#include <QDebug>
#include "utils/x11utills.h"
#include "xcb/xcb.h"

#define TEXTBICON 0
#define ICONONLY 1

DtaskbarWidget::DtaskbarWidget(Setting *s, QWidget *parent, bool debug):
    mSetting(s),QWidget(parent),mDebug(debug)
{
m_parent=parent;
    QFont font=parent->font();
    font.setPointSize(parent->font().pointSize());
    setFont(font);
    this->setObjectName("dtaskbar");
    this->setContentsMargins(0,0,0,0);
    this->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Preferred);
    m_rootWindow = QX11Info::appRootWindow();
    m_horizontalLayout = new QHBoxLayout(this);
    m_horizontalLayout->setSpacing(0);
    m_horizontalLayout->setContentsMargins(0,0,0,0);
    m_horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
   // m_horizontalLayout->addSpacing(3);
//setMinimumHeight(m_parent->height());
    loadSettings();

    qApp->installNativeEventFilter(this);


}

//_________________________________________________________________________________________
bool DtaskbarWidget::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{

    if (eventType == "xcb_generic_event_t") {
        xcb_generic_event_t* event = static_cast<xcb_generic_event_t *>(message);

        switch (event->response_type & ~0x80) {

        case XCB_PROPERTY_NOTIFY: {
            xcb_property_notify_event_t *property = reinterpret_cast<xcb_property_notify_event_t*>(event);

            windowPropertyChanged(property->window, property->atom);

            break;
        }

        default: break;
        }

    }

    return false;
}

//_________________________________________________________________________________________
void DtaskbarWidget::windowPropertyChanged(unsigned long window, unsigned long atom)
{
    if (window ==m_rootWindow) {

        if (atom == X11UTILLS::atom("_NET_CLIENT_LIST")){
            //  qDebug()<<"DtaskbarWidget::windowPropertyChanged   _NET_CLIENT_LIST";
            refreshTaskList();
        }

        if(atom == X11UTILLS::atom("_NET_ACTIVE_WINDOW")){
           // qDebug()<<"DtaskbarWidget::windowPropertyChanged   _NET_ACTIVE_WINDOW";

            activeWindowChanged();
        }

        return;
    }

}

//_________________________________________________________________________________________
void DtaskbarWidget::loadSettings()
{
    if(mDebug)  qDebug()<<"   [-]"<<__FILE__<< __LINE__<<" loadSettings()";

    //_________________________________________________ Settings
    QString highlight=qApp->palette().highlight().color().name();
    QString highlightTxt=qApp->palette().highlightedText().color().name();

    mSetting->beginGroup("Taskbar");

    QString bgColor         =mSetting->background();
    QString fgColor         =mSetting->foreground(m_parent->palette().windowText().color().name());
    QString activebgColor   =mSetting->activeBackground(highlight);
    QString activefgColor   =mSetting->activeForeground(highlightTxt);
    int     alpha           =mSetting->alpha();//
    int     activeAlpha     =mSetting->activeAlpha();
    QString underline       =mSetting->underline();
    QString overline        =mSetting->overline();
    QString activeunderline =mSetting->activeUnderline();
    QString activeoverline  =mSetting->activeOverline();
    int     border          =mSetting->border();
    QString fontName        =mSetting->fontName(m_parent->font().family());
    int     fontSize        =mSetting->fontSize(m_parent->font().pointSize());
    bool    fontBold        =mSetting->fontBold(m_parent->font().bold());

      mSetting->endGroup();
    //_________________________________________________ INIT
    QFont font;
    font.setPointSize(fontSize);
    font.setBold(fontBold);
    font.setFamily(fontName);
    setFont(font);

    //-------------------------------------------------------STYLESHEET
    //QtoolButton Normale
    QString mStyleSheet="QToolButton{";
    mStyleSheet+=StyleColors::style(bgColor,fgColor,underline,overline,border,alpha)+"\n}";
    //QtoolButton Active
    QString activeStyleSheet="QToolButton:checked{\n";
    activeStyleSheet+=StyleColors::style(activebgColor,activefgColor,activeunderline,activeoverline,border,activeAlpha)+"\n}";


    setStyleSheet(mStyleSheet+activeStyleSheet);

    refreshTaskList();
}

//_________________________________________________________________________تحديث النوافذ
void DtaskbarWidget::refreshTaskList()
{
    if(mDebug)  qDebug()<<"   [-]"<<__FILE__<< __LINE__<<" refreshTaskList()";



    //              قائمة مؤقة بجميع معرفات النوافذ الحاضرة
    QList<unsigned long> listWindow = X11UTILLS::getClientList();


    //                  حذف جميع الازرار ومعرفاتها السابقة
    if(mButtonsHash.count()>0){
        //              قائمة بجميع معرفات النولفذ والازرار
        QMutableHashIterator<unsigned long, DActionTaskbar*> i(mButtonsHash);
        while (i.hasNext())
        {

            i.next();

            int     r = listWindow.removeAll(i.key());

            if (!r)
            {
                delete i.value();
                i.remove();
            }

        }
    }

    foreach (unsigned long wnd, listWindow)
    {


        //            انشاء زر جديد للنوافذ الحاضرة
        DActionTaskbar *btn=new DActionTaskbar(wnd,this);

        //            اضافة ازر للقائمة
        mButtonsHash.insert(wnd, btn);

       //        اضافة الزر للبنال
        m_horizontalLayout->addWidget(btn);
        if(mDebug)  qDebug()<<"   [-]"<<__FILE__<< __LINE__<<" addWidget()"<<btn->text();

    }

    foreach (DActionTaskbar *btn,mButtonsHash){
         btn->setMaximumWidth(this->height()+6);
    }

    activeWindowChanged();

}


//____________________________________________________________________________النافذة المفعلة
void DtaskbarWidget::activeWindowChanged()
{
    if(mDebug)  qDebug()<<"   [-]"<<__FILE__<< __LINE__<<" activeWindowChanged()";

    //        البحث عن النافذة المفعلة
    unsigned long window =X11UTILLS::getActiveAppWindow();

    if( ! window)
        return;


    foreach (DActionTaskbar *btn,mButtonsHash){

      //  btn->setText(X11UTILLS::getWindowTitle(mButtonsHash.key(btn)));
        btn->setChecked(false);
        btn->setActiveWin(false);
        btn->getText();
    }


    //         التاكد من وجودها مع الازرا
    DActionTaskbar* toolbtn =0;
    if (mButtonsHash.contains(window)){
        toolbtn=mButtonsHash.value(window);
        m_activeWindow=window;
        toolbtn->setActiveWin(true);
        // toolbtn->setChecked(true);

    }



}

//__________________________________________________________________________mouse wheel
void DtaskbarWidget::wheelEvent(QWheelEvent* event)
{

    QList<unsigned long> winList = X11UTILLS::getClientList();
    int current = winList.indexOf(X11UTILLS::getActiveAppWindow());
    int delta = event->delta() < 0 ? 1 : -1;

    for (int ix = current + delta; 0 <= ix && ix < winList.size(); ix += delta)
    {
        unsigned long window = winList.at(ix);

        X11UTILLS::raiseWindow(window);
        break;

    }

}


