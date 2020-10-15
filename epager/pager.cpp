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

#include <QtGui>
#include <QButtonGroup>
#include <QToolButton>
#include <QWheelEvent>
#include <QFontMetrics>
#include <QSignalMapper>
#include "utils/stylecolors.h"
#include "utils/xdesktoputils.h"
#include "utils/x11utills.h"
#include "utils/setting.h"
#include "utils/defines.h"
#include "pager.h"


//_________________________________________________________________________________
Pager::Pager(QWidget* parent)
    : QWidget( parent),
      m_DeskCount(1),mParent(parent)
{

   if(Defines::degug()) qDebug()<<"\033[34m   [-]"<<"Pager:"<< __LINE__<<"Pager()\033[0m";
   //TODO FIX deprecated
   //1 m_pSignalMapper=new QSignalMapper;
    this->setObjectName("Pager");
    this->setWindowTitle(tr("Desktop Switch"));
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

    m_GroupBtns = new QButtonGroup(widgetContent);
  //2  connect ( m_pSignalMapper, SIGNAL(mapped(int)), this, SLOT(setDesktop(int)));

    mHBoxLayout = new QHBoxLayout(widgetContent);
    mHBoxLayout->setSpacing(0);
    mHBoxLayout->setContentsMargins(0, 0, 0, 0);
    mHBoxLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    setContentsMargins(0, 0, 0, 0);

   // m_size=QSize(16,16);

    loadSettings();
    //qApp->installNativeEventFilter(this);

    rechargeDesktop();

}

//__________________________________________________________________________________
void Pager::loadSettings()
{
   if(Defines::degug())  qDebug()<<"\033[34m   [-]"<<"Pager:"<< __LINE__<<"loadSettings()\033[0m";


    //_________________________________________________ Settings
    QString highlight=qApp->palette().highlight().color().name();
    QString highlightTxt=qApp->palette().highlightedText().color().name();

    Setting::instance()->beginGroup(MPAGER);

    m_bgColor         =Setting::background();
    m_fgColor         =Setting::foreground(mParent->palette().windowText().color().name());
    m_activeBgColor   =Setting::activeBackground(highlight);
    QString activefgColor   =Setting::activeForeground(mParent->palette().windowText().color().name());
            mActiveIcon     =Setting::activeIcon();
    int     alpha           =Setting::alpha();//
    int     activeAlpha     =Setting::activeAlpha();
    QString underline       =Setting::underline();
    QString overline        =Setting::overline();
    QString activeunderline =Setting::activeUnderline();
    QString activeoverline  =Setting::activeOverline();
    int     border          =Setting::border();
    QString borderColor      =Setting::borderColor();
    int     radius          =Setting::radius();
    QString fontName        =Setting::fontName(mParent->font().family());
    int     fontSize        =Setting::fontSize(mParent->font().pointSize());
    bool    fontBold        =Setting::fontBold(mParent->font().bold());

    //_________________________________________________ INIT
    QFont font;
    font.setPointSize(fontSize);
    font.setBold(fontBold);
    font.setFamily(fontName);
    setFont(font);

    QString dname=Setting::desktopDesplay();
    if(dname=="name")
        mDesktopType=DESKNAME;
    else if(dname=="icon"){
        mDesktopType=DESKICON;
        QStringList list= Setting::iconsList();
        listIcons.clear();
        listIcons<<"0"<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9";
        for (int i = 0; i < list.count(); ++i) {
            if(i<listIcons.count())
                listIcons.replace(i,list.at(i));
        }

    }else{
        mDesktopType=DESKINDEX;
    }

     Setting::instance()->endGroup();

    //-------------------------------------------------------STYLESHEET
        widgetContent->setContentsMargins((radius+1),0,(radius+1),0);
        QString mStylebg="QWidget#WidgetContent{";
        mStylebg+=StyleColors::style(m_bgColor,m_fgColor,QString(),QString(),border,alpha,borderColor,radius)+"\n}";

     //QtoolButton Normale
    QString mStyleSheet="QToolButton{";
    mStyleSheet+=StyleColors::style(m_bgColor,m_fgColor,underline,overline,border,alpha,borderColor,0)+"\n}";

    //QtoolButton Active
    QString activeStyleSheet="QToolButton:checked{\n";
    activeStyleSheet+=StyleColors::style(m_activeBgColor,activefgColor,activeunderline,activeoverline,border,activeAlpha,QString(),0)+"\n}";
    setStyleSheet(mStylebg+mStyleSheet+activeStyleSheet);

    setupBtns();

}

//__________________________________________________________________________________
void Pager::setNativeEventFilter(const QByteArray &eventType, void *message, long *)
{

   //  qDebug()<<"Pager:"<< __LINE__<<"nativeEventFilter()";
    if (eventType == "xcb_generic_event_t") {
        xcb_generic_event_t* event = static_cast<xcb_generic_event_t *>(message);

        switch (event->response_type & ~0x80) {

        case XCB_PROPERTY_NOTIFY:
            xcb_property_notify_event_t *property = reinterpret_cast<xcb_property_notify_event_t*>(event);
            //TODO fix atom name
            if(property->atom==XDesktop::atom("_NET_DESKTOP_NAMES"))
            {if(Defines::degug())  qDebug()<<"\033[34m   [-]"<<"Pager:"<< __LINE__<<"_NET_DESKTOP_NAMES\033[0m"<<property->atom; rechargeDesktop();}
            else if(property->atom==XDesktop::atom("_NET_CURRENT_DESKTOP"))
            {if(Defines::degug())  qDebug()<<"\033[34m   [-]"<<"Pager:"<< __LINE__<<"_NET_CURRENT_DESKTOP\033[0m"<<property->atom; actvateBtnDesktop();}
            else if(property->atom==XDesktop::atom("_NET_NUMBER_OF_DESKTOPS"))
            { if(Defines::degug()) qDebug()<<"\033[34m   [-]"<<"Pager:"<< __LINE__<<"_NET_NUMBER_OF_DESKTOPS\033[0m"<<property->atom; rechargeDesktop();}
           if (property->atom == X11UTILLS::atom("_NET_CLIENT_LIST")){
                //  qDebug()<<"DtaskbarWidget::windowPropertyChanged   _NET_CLIENT_LIST";
                refreshTaskList();
            }
            break;
        }

    }

    //return false;
}

void Pager::refreshTaskList()
{
    QList<unsigned long> listWindow = X11UTILLS::getClientList();
   listWndDesk.clear();
    foreach (unsigned long wnd, listWindow)
    {
        int dx=X11UTILLS::getWindowDesktop(wnd);
        listWndDesk.removeAll(dx);
        listWndDesk.append(dx);

    }
    qDebug()<<"=============================";
    qDebug()<<listWndDesk;
refreshTaskButton();

}

void Pager::refreshTaskButton()
{
    // int activeDesk = qMax(XDesktop::active(), 0);
     int count= listbtn.count();
    for (int i = 0; i < count; ++i) {
        //  if(i==activeDesk)continue;

          ToolButton *btn=listbtn.at(i);
        if(listWndDesk.contains(i)){

          btn->setText("°"+btn->data());

        }else{
            btn->setText(btn->data());

        }
    }

}

//__________________________________________________________________________________
void Pager::rechargeDesktop()
{
    if(Defines::degug())  qDebug()<<"\033[34m   [-]"<<"Pager:"<< __LINE__<<"rechargeDesktop()\033[0m";
    int count = qMax(XDesktop::count(), 1);

    if (m_DeskCount != count)
    {
        m_DeskCount = count;
        m_desktopNames = XDesktop::names();
        setupBtns();
    }

    if (m_desktopNames != XDesktop::names())
    {
        m_desktopNames = XDesktop::names();
        setupBtns();
    }

    int activeDesk = qMax(XDesktop::active(), 0);

    if(Defines::degug())  qDebug()<<"\033[34m   [-]"<<"Pager:"<< __LINE__<<" activeDesk: "<<activeDesk<<"DeskCountt: \033[0m"<<m_DeskCount;
    if(m_GroupBtns->buttons().count()>0 &&  activeDesk < m_GroupBtns->buttons().count()){

        m_GroupBtns->button(activeDesk)->setChecked(true);

    }


}

//__________________________________________________________________________________
void Pager::setupBtns()
{
    if(Defines::degug())  qDebug()<<"\033[34m   [-]"<<"Pager:"<< __LINE__<<"setupBtns()\033[0m";

    listbtn.clear();
    foreach (QAbstractButton * b, m_GroupBtns->buttons())
    {
        //3  m_pSignalMapper->removeMappings(b);
        m_GroupBtns->removeButton(b);

        delete b;
    }

    if(Defines::degug())  qDebug()<<"\033[34m   [-]"<<"Pager:"<< __LINE__<<"m_DeskCount<<"""<<m_DeskCount;

    int activeDesk = qMax(XDesktop::active(), 0);

    for (int i = 0; i < m_DeskCount; ++i)
    {

        ToolButton * btn = new ToolButton(this);

        switch (mDesktopType) {
        case DESKINDEX:
            btn->setText(QString::number(i+1).trimmed());
            break;
        case DESKNAME:
            btn->setText(XDesktop::name(i,"desktop").trimmed());
            break;
        case DESKICON:
            if(i<listIcons.count())
                btn->setText(listIcons.at(i).trimmed());
            else
                btn->setText(QString::number(i+1).trimmed());
            break;

        default:
            btn->setText(QString::number(i+1).trimmed());//XDesktop::name(i,"desktop")
            break;

        }

        btn->setCheckable(true);
        btn->setToolTip( tr("Desktop %1").arg(XDesktop::name(i,"desktop")));
        btn->setData(btn->text());
       if(i==activeDesk){
        if(mDesktopType==DESKICON && activeDesk<listIcons.count() && !mActiveIcon.isEmpty()){
            btn->setText(mActiveIcon);
            btn->setData(mActiveIcon);
        }
       }
        //qDebug()<<"desktop name ::"<<   XDesktop::name(i,"desktop").trimmed();
        //  btn->setText(QString::number(i+1));//XDesktop::name(i,"desktop")

        QFontMetrics mtr(btn->font());

#if QT_VERSION >= QT_VERSION_CHECK(5,10,0)
        int w=mtr.horizontalAdvance(btn->text());
#else
        int w=mtr.width(btn->text());

#endif
        btn->setMaximumWidth((w*2));
        btn->setMinimumWidth((w*2));
        //4  m_pSignalMapper->setMapping(btn, i);
        //5  connect(btn, SIGNAL(activated()), m_pSignalMapper, SLOT(map())) ;
        // btn->setMaximumWidth(btn->height());
        mHBoxLayout->addWidget(btn);
        m_GroupBtns->addButton(btn, i);
        listbtn.append(btn);
    }


    //    QAbstractButton * button = m_GroupBtns->button(activeDesk);


    //    if (button){
    //        button->setChecked(true);

    //        if(mDesktopType==DESKICON && activeDesk<listIcons.count() && !mActiveIcon.isEmpty()){
    //            button->setText(mActiveIcon);

    //        }
    //    }

    connect(m_GroupBtns, SIGNAL(buttonClicked(int)),
            this, SLOT(setDesktop(int)));
    refreshTaskList();

}

Pager::~Pager()
{

}

//_________________________________________________________________________________
void Pager::setDesktop(int desktop)
{

    XDesktop::setCurrent(desktop);
//    int activeDesk = qMax(XDesktop::active(), 0);

//    if(mDesktopType==DESKICON  && !mActiveIcon.isEmpty()){
//        for (int i = 0; i < m_DeskCount; ++i)
//        {
//            if(i<listIcons.count())
//                m_GroupBtns->button(i)->setText(listIcons.at(i).trimmed());
//            else
//                 m_GroupBtns->button(activeDesk)->setText(QString::number(i+1).trimmed());

//        }
//    }
//    QAbstractButton * button = m_GroupBtns->button(activeDesk);
//     if (button){

//         button->setChecked(true);
//         if(mDesktopType==DESKICON && activeDesk<listIcons.count() && !mActiveIcon.isEmpty())
//           button->setText(mActiveIcon);
//     }
  actvateBtnDesktop();
}

//__________________________________________________________________________________
void Pager::wheelEvent(QWheelEvent* e)
{
    int max     = XDesktop::count()- 1;
    int delta   = e->delta() < 0 ? 1 : -1;
    int current = XDesktop::active()+ delta;

    if (current > max)
        current = 0;
    else if (current < 0)
        current = max;

    XDesktop::setCurrent(current);
}

//__________________________________________________________________________________
void Pager::actvateBtnDesktop()
{
    int activeDesk = qMax(XDesktop::active(), 0);
   if(Defines::degug())  qDebug()<<"\033[34m   [-]"<<"Pager:"<< __LINE__<<"activeDesk\033[0m"<<activeDesk;

   if(mDesktopType==DESKICON  && !mActiveIcon.isEmpty() ){
       for (int i = 0; i < m_DeskCount; ++i)
       {
           ToolButton *btn=listbtn.at(i);
           if(i<listIcons.count()){
              btn->setText(listIcons.at(i).trimmed());
              btn->setData(listIcons.at(i).trimmed());

           }else{
               btn=listbtn.at(activeDesk);
               btn->setText(QString::number(i+1).trimmed());
                btn->setData(QString::number(i+1).trimmed());
           }
          // btn->setData(btn->text());
       }

   }


   ToolButton * button = listbtn.at(activeDesk);
    if (button){

        button->setChecked(true);

        if(mDesktopType==DESKICON && activeDesk<listIcons.count() && !mActiveIcon.isEmpty()){
        button->setText(mActiveIcon);
       button ->setData(mActiveIcon);
        }
    }
    else
        rechargeDesktop();

   refreshTaskButton();
}

//__________________________________________________________________________________
void Pager::goDesktop(int arg)
{

   if(Defines::degug())  qDebug()<<"\033[34m   [-]"<<"Pager:"<< __LINE__<<"goDesktop\033[0m"<<arg;
    int max = XDesktop::count()- 1;

    int current = XDesktop::active()+arg;

    if (current > max)
        current = 0;
    else if (current < 0)
        current =max;

   if(Defines::degug())  qDebug()<<"\033[34m   [-]"<<"Pager:"<< __LINE__<<"current\033[0m"<<current<<max;
    XDesktop::setCurrent(current);

}

//__________________________________________________________________________________
void Pager::setSize(QSize size)
{
   m_size= size;
    setupBtns();
}


