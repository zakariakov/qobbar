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
#include <QtDebug>
#include <QFontMetrics>
#include <QSignalMapper>
#include "utils/stylecolors.h"
#include "utils/xdesktoputils.h"
#include "pager.h"

//_________________________________________________________________________________
Pager::Pager(Setting *s, QWidget* parent, bool debug)
    : QWidget( parent),
      m_DeskCount(1),mParent(parent),mSetting(s),mdebug(debug)
{

   if(mdebug) qDebug()<<"   [-]"<<__FILE__<< __LINE__<<"Pager()";
   //TODO FIX deprecated
   //1 m_pSignalMapper=new QSignalMapper;
    this->setObjectName("Pager");
    this->setWindowTitle(tr("Desktop Switch"));
    this->setContentsMargins(0,0,0,0);
    this->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Preferred);

    m_GroupBtns = new QButtonGroup(this);
  //2  connect ( m_pSignalMapper, SIGNAL(mapped(int)), this, SLOT(setDesktop(int)));

    mHBoxLayout = new QHBoxLayout(this);
    mHBoxLayout->setSpacing(0);
    mHBoxLayout->setContentsMargins(0, 0, 0, 0);
    mHBoxLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    setContentsMargins(0, 0, 0, 0);

   // m_size=QSize(16,16);

    loadSettings();
    qApp->installNativeEventFilter(this);

    rechargeDesktop();

}

//__________________________________________________________________________________
void Pager::loadSettings()
{
   if(mdebug)  qDebug()<<"   [-]"<<__FILE__<< __LINE__<<"loadSettings()";


    //_________________________________________________ Settings
    QString highlight=qApp->palette().highlight().color().name();
    QString highlightTxt=qApp->palette().highlightedText().color().name();

    mSetting->beginGroup("Pager");

    QString bgColor         =mSetting->background();
    QString fgColor         =mSetting->foreground(mParent->palette().windowText().color().name());
    QString activebgColor   =mSetting->activeBackground(highlight);
    QString activefgColor   =mSetting->activeForeground(mParent->palette().windowText().color().name());
            mActiveIcon     =mSetting->activeIcon();
    int     alpha           =mSetting->alpha();//
    int     activeAlpha     =mSetting->activeAlpha();
    QString underline       =mSetting->underline();
    QString overline        =mSetting->overline();
    QString activeunderline =mSetting->activeUnderline();
    QString activeoverline  =mSetting->activeOverline();
    int     border          =mSetting->border();
    QString fontName        =mSetting->fontName(mParent->font().family());
    int     fontSize        =mSetting->fontSize(mParent->font().pointSize());
    bool    fontBold        =mSetting->fontBold(mParent->font().bold());

    //_________________________________________________ INIT
    QFont font;
    font.setPointSize(fontSize);
    font.setBold(fontBold);
    font.setFamily(fontName);
    setFont(font);

    QString dname=mSetting->desktopDesplay();
    if(dname=="name")
        mDesktopType=DESKNAME;
    else if(dname=="icon"){
        mDesktopType=DESKICON;
        QStringList list= mSetting->iconsList();
        listIcons.clear();
        listIcons<<"0"<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9";
        for (int i = 0; i < list.count(); ++i) {
            if(i<listIcons.count())
                listIcons.replace(i,list.at(i));
        }

    }else{
        mDesktopType=DESKINDEX;
    }

    mSetting->endGroup();

    //-------------------------------------------------------STYLESHEET
    //QtoolButton Normale
    QString mStyleSheet="QToolButton{";
    mStyleSheet+=StyleColors::style(bgColor,fgColor,underline,overline,border,alpha)+"\n}";
    //QtoolButton Active
    QString activeStyleSheet="QToolButton:checked{\n";
    activeStyleSheet+=StyleColors::style(activebgColor,activefgColor,activeunderline,activeoverline,border,activeAlpha)+"\n}";
    setStyleSheet(mStyleSheet+activeStyleSheet);

    setupBtns();

}

//__________________________________________________________________________________
bool Pager::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{
    // qDebug()<<__FILE__<< __LINE__<<"nativeEventFilter()";
    if (eventType == "xcb_generic_event_t") {
        xcb_generic_event_t* event = static_cast<xcb_generic_event_t *>(message);

        switch (event->response_type & ~0x80) {

        case XCB_PROPERTY_NOTIFY:
            xcb_property_notify_event_t *property = reinterpret_cast<xcb_property_notify_event_t*>(event);
            //TODO fix atom name
            if(property->atom==XDesktop::atom("_NET_DESKTOP_NAMES"))
            {if(mdebug)  qDebug()<<"   [-]"<<__FILE__<< __LINE__<<"_NET_DESKTOP_NAMES"<<property->atom; rechargeDesktop();}
            else if(property->atom==XDesktop::atom("_NET_CURRENT_DESKTOP"))
            {if(mdebug)  qDebug()<<"   [-]"<<__FILE__<< __LINE__<<"_NET_CURRENT_DESKTOP"<<property->atom; actvateBtnDesktop();}
            else if(property->atom==XDesktop::atom("_NET_NUMBER_OF_DESKTOPS"))
            { if(mdebug) qDebug()<<"   [-]"<<__FILE__<< __LINE__<<"_NET_NUMBER_OF_DESKTOPS"<<property->atom; rechargeDesktop();}

            break;
        }

    }

    return false;
}

//__________________________________________________________________________________
void Pager::rechargeDesktop()
{
    if(mdebug)  qDebug()<<"   [-]"<<__FILE__<< __LINE__<<"rechargeDesktop()";
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

    if(mdebug)  qDebug()<<"   [-]"<<__FILE__<< __LINE__<<" activeDesk: "<<activeDesk<<"DeskCountt: "<<m_DeskCount;
    if(m_GroupBtns->buttons().count()>0 &&  activeDesk < m_GroupBtns->buttons().count()){

        m_GroupBtns->button(activeDesk)->setChecked(true);

    }


}

//__________________________________________________________________________________
void Pager::setupBtns()
{
   if(mdebug)  qDebug()<<"   [-]"<<__FILE__<< __LINE__<<"setupBtns()";
    foreach (QAbstractButton * b, m_GroupBtns->buttons())
    {
       //3  m_pSignalMapper->removeMappings(b);
        m_GroupBtns->removeButton(b);
        delete b;
    }

   if(mdebug)  qDebug()<<"   [-]"<<__FILE__<< __LINE__<<"m_DeskCount"<<m_DeskCount;

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

        //  btn->setText(QString::number(i+1));//XDesktop::name(i,"desktop")
        btn->setCheckable(true);
        btn->setToolTip( tr("Desktop %1").arg(XDesktop::name(i,"desktop")));
        btn->setData(btn->text());
        QFontMetrics mtr(btn->font());
        int w=mtr.width(btn->text());
        btn->setMaximumWidth(w+10);
       //4  m_pSignalMapper->setMapping(btn, i);
       //5  connect(btn, SIGNAL(activated()), m_pSignalMapper, SLOT(map())) ;
       // btn->setMaximumWidth(btn->height());
        mHBoxLayout->addWidget(btn);
        m_GroupBtns->addButton(btn, i);

    }

    int activeDesk = qMax(XDesktop::active(), 0);
    QAbstractButton * button = m_GroupBtns->button(activeDesk);

    if (button){
        button->setChecked(true);

        if(mDesktopType==DESKICON && activeDesk<listIcons.count() && !mActiveIcon.isEmpty())
            button->setText(mActiveIcon);
    }
    connect(m_GroupBtns, SIGNAL(buttonClicked(int)),
            this, SLOT(setDesktop(int)));

}

Pager::~Pager()
{

}

//_________________________________________________________________________________
void Pager::setDesktop(int desktop)
{

    XDesktop::setCurrent(desktop);
    int activeDesk = qMax(XDesktop::active(), 0);

    if(mDesktopType==DESKICON  && !mActiveIcon.isEmpty()){
        for (int i = 0; i < m_DeskCount; ++i)
        {
            if(i<listIcons.count())
                m_GroupBtns->button(i)->setText(listIcons.at(i).trimmed());
            else
                 m_GroupBtns->button(activeDesk)->setText(QString::number(i+1).trimmed());

        }
    }
    QAbstractButton * button = m_GroupBtns->button(activeDesk);
     if (button){

         button->setChecked(true);
         if(mDesktopType==DESKICON && activeDesk<listIcons.count() && !mActiveIcon.isEmpty())
           button->setText(mActiveIcon);
     }
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
   if(mdebug)  qDebug()<<"   [-]"<<__FILE__<< __LINE__<<"activeDesk"<<activeDesk;

   if(mDesktopType==DESKICON  && !mActiveIcon.isEmpty() ){
       for (int i = 0; i < m_DeskCount; ++i)
       {
           if(i<listIcons.count())
               m_GroupBtns->button(i)->setText(listIcons.at(i).trimmed());
           else
                m_GroupBtns->button(activeDesk)->setText(QString::number(i+1).trimmed());

       }
   }


   QAbstractButton * button = m_GroupBtns->button(activeDesk);
    if (button){

        button->setChecked(true);

        if(mDesktopType==DESKICON && activeDesk<listIcons.count() && !mActiveIcon.isEmpty())
        button->setText(mActiveIcon);
    }
    else
        rechargeDesktop();

}

//__________________________________________________________________________________
void Pager::goDesktop(int arg)
{

   if(mdebug)  qDebug()<<"   [-]"<<__FILE__<< __LINE__<<"goDesktop"<<arg;
    int max = XDesktop::count()- 1;

    int current = XDesktop::active()+arg;

    if (current > max)
        current = 0;
    else if (current < 0)
        current =max;

   if(mdebug)  qDebug()<<"   [-]"<<__FILE__<< __LINE__<<"current"<<current<<max;
    XDesktop::setCurrent(current);

}

//__________________________________________________________________________________
void Pager::setSize(QSize size)
{
   m_size= size;
    setupBtns();
}


