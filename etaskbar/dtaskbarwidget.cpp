/********************************************************************
 *(c)GPL3+

original code  http://razor-qt.org
 modified by abou zakaria
*********************************************************************/

//#include "taskbarsettingdialog.h"
#include "utils/stylecolors.h"
#include "utils/defines.h"
#include "utils/setting.h"
#include <QtGui>
#include <QX11Info>
#include <QDesktopWidget>
#include <QMenu>
#include "dtaskbarwidget.h"
#include "dactiontaskbar.h"
#include "utils/x11utills.h"
#include "xcb/xcb.h"

#define TEXTBICON 0
#define ICONONLY 1


DtaskbarWidget::DtaskbarWidget(/*Setting *s,*/ QWidget *parent/*, bool debug*/):
    QWidget(parent)/*,mSetting(s),mDebug(debug)*/
{
m_parent=parent;
    QFont font=parent->font();
    font.setPointSize(parent->font().pointSize());
    setFont(font);
    this->setObjectName("dtaskbar");
    this->setContentsMargins(0,0,0,0);
    this->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Preferred);

    widgetContent=new QWidget;
    widgetContent->setObjectName("WidgetContent");
    widgetContent->setWindowTitle(tr("Desktop Switch"));
    widgetContent->setContentsMargins(0,0,0,0);
    widgetContent->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Preferred);

    QHBoxLayout *Layout = new QHBoxLayout(this);
    Layout->setSpacing(0);
    Layout->setContentsMargins(0, 0, 0, 0);
    Layout->setObjectName(QString::fromUtf8("horizontalLayout"));

    Layout->addWidget(widgetContent);

    m_horizontalLayout = new QHBoxLayout(widgetContent);
    m_horizontalLayout->setSpacing(0);
    m_horizontalLayout->setContentsMargins(0,0,0,0);
    m_horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));

      m_rootWindow = QX11Info::appRootWindow();
    loadSettings();

    //qApp->installNativeEventFilter(this);


}

//_________________________________________________________________________________________
void DtaskbarWidget::setNativeEventFilter(const QByteArray &eventType, void *message, long *)
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

    //return false;
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
    if(Defines::degug())  qDebug()<<"   [-]"<<__FILE__<< __LINE__<<" loadSettings()";

    //_________________________________________________ Settings
    QString highlight=qApp->palette().highlight().color().name();
    QString highlightTxt=qApp->palette().highlightedText().color().name();

    Setting::instance()->beginGroup(MTASKBAR);

    QString bgColor         =Setting::background();
    QString fgColor         =Setting::foreground(m_parent->palette().windowText().color().name());
    QString activebgColor   =Setting::activeBackground(highlight);
    QString activefgColor   =Setting::activeForeground(highlightTxt);
    QString borderColor     =Setting::borderColor();
    int     alpha           =Setting::alpha();//
    int     activeAlpha     =Setting::activeAlpha();
    QString underline       =Setting::underline();
    QString overline        =Setting::overline();
    QString activeunderline =Setting::activeUnderline();
    QString activeoverline  =Setting::activeOverline();
    int     border          =Setting::border();
    int     radius          =Setting::radius();

    Setting::instance()->endGroup();

//    mSetting->beginGroup("Taskbar");

//    QString bgColor         =mSetting->background();
//    QString fgColor         =mSetting->foreground(m_parent->palette().windowText().color().name());
//    QString activebgColor   =mSetting->activeBackground(highlight);
//    QString activefgColor   =mSetting->activeForeground(highlightTxt);
//    QString borderColor     =mSetting->borderColor();
//    int     alpha           =mSetting->alpha();//
//    int     activeAlpha     =mSetting->activeAlpha();
//    QString underline       =mSetting->underline();
//    QString overline        =mSetting->overline();
//    QString activeunderline =mSetting->activeUnderline();
//    QString activeoverline  =mSetting->activeOverline();
//    int     border          =mSetting->border();
//    int     radius          =mSetting->radius();

//    mSetting->endGroup();

    //_________________________________________________ INIT

    //-------------------------------------------------------STYLESHEET
    widgetContent->setContentsMargins((radius+1),0,(radius+1),0);
    QString mStylebg="QWidget#WidgetContent{";
    mStylebg+=StyleColors::style(bgColor,fgColor,QString(),QString(),border,alpha,borderColor,radius)+"\n}";

    //QtoolButton Normale
    QString mStyleSheet="QToolButton{";
    mStyleSheet+=StyleColors::style(bgColor,fgColor,underline,overline,border,alpha,borderColor,0)+"\n}";

    //QtoolButton Active
    QString activeStyleSheet="QToolButton:checked{\n";
    activeStyleSheet+=StyleColors::style(activebgColor,activefgColor,activeunderline,activeoverline,border,activeAlpha,QString(),0)+"\n}";
    setStyleSheet(mStylebg+mStyleSheet+activeStyleSheet);
if(Defines::degug()) qDebug()<<"   [-]"<<__FILE__<< __LINE__<<" loadSettings()\n"<<mStylebg+mStyleSheet+activeStyleSheet;
    foreach (DActionTaskbar *btn,mButtonsHash){
         btn->setStyleSheet(mStyleSheet+activeStyleSheet);
    }

    refreshTaskList();
}

//_________________________________________________________________________تحديث النوافذ
void DtaskbarWidget::refreshTaskList()
{
    if(Defines::degug())  qDebug()<<"   [-]"<<__FILE__<< __LINE__<<" refreshTaskList()";



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

//===========================================
        //            اضافة ازر للقائمة
        mButtonsHash.insert(wnd, btn);

       //        اضافة الزر للبنال
        m_horizontalLayout->addWidget(btn);
        if(Defines::degug())  qDebug()<<"   [-]"<<__FILE__<< __LINE__<<" addWidget()"<<btn->text();

    }

    foreach (DActionTaskbar *btn,mButtonsHash){
         btn->setMaximumWidth(btn->height());
    btn->setMinimumWidth(btn->height());
    int size=btn->height()-6;
    if(size<16) size=16;
     if(size>48) size=48;

   btn->setIconSize(QSize(size,size));
    }

    activeWindowChanged();

}


//____________________________________________________________________________النافذة المفعلة
void DtaskbarWidget::activeWindowChanged()
{
    if(Defines::degug())  qDebug()<<"   [-]"<<__FILE__<< __LINE__<<" activeWindowChanged()";

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
    DActionTaskbar* toolbtn =nullptr;
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


