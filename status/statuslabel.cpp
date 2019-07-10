#include "statuslabel.h"
#include "utils/stylecolors.h"
#include "utils/defines.h"
#include <QDebug>
#include <QApplication>
#include <QtConcurrent>
#include <QSettings>
#include <QMouseEvent>

QString fixCommand(QString cmd)
{
    if(cmd.contains("$HOME/") || cmd.contains("~/")){
        cmd.replace("$HOME/",QDir::homePath()+"/");
        cmd.replace("~/",QDir::homePath()+"/");
//        cmd.insert(0,QDir::homePath());
    }
    return cmd.trimmed();
}
//Constructor
StatusLabel::StatusLabel(QString group, QWidget *parent):QLabel(parent),
    mName(group),mParent(parent)/*,mySetting(s),mdebug(debug)*/
{

    if(Defines::degug())  qDebug()<<"\033[32m   [-] statu : "<<mName<< __LINE__<<"Name:\033[0m"<<mName;

    mTimer=new QTimer;
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);

    /*_____________________________________________*/
    //********* old **********//
    //    m_render = new QFutureWatcher< void >(this);
    //    connect(m_render, SIGNAL(finished()),
    //            SLOT(on_render_finished()));
    //    connect(this, SIGNAL(textReady(QString))
    //            , SLOT(on_textReady(QString)));

    mThread=new Thread;
    connect(mThread,SIGNAL(terminated(QString)),this,SLOT(updateCmd(QString)));
    /*_____________________________________________*/
    setMargin(0);
    setContentsMargins(0,0,0,0);

    loadSettings();

}
StatusLabel::~StatusLabel()
{

 // cancelRender();
    //********* old **********//
  //delete m_render;
  delete mTimer;


}

//________________________________________________________________Settings
void StatusLabel::loadSettings()
{
   if(Defines::degug()) qDebug()<<"\033[32m   [-] statu : "<<mName<< __LINE__<<"loadSettings:\033[0m"<<mName;

   //
    mTimer->stop();
     //********* old **********//
    //    if(m_render->isRunning())
    //    m_render->setPaused(true);
    //    m_render->blockSignals(true);

  //  mThread->terminate();
    QString groupName=mName;

    if(mName.contains(":")){
        groupName=mName.section(":",0,0);

    }
    Setting::instance()->beginGroup(groupName);

     QString mCommand           =Setting::command();
     int     interval           =Setting::interval();
             mLabel             =Setting::label();
             mSuffix            =Setting::suffix();
             mPrefix            =Setting::prefix();
             mMouseLeftCmd      =Setting::clickLeft();
             mMouseRightCmd     =Setting::clickRight();
             mMouseWheelUpCmd   =Setting::mouseWheelUp();
             mMouseWheelDownCmd =Setting::mouseWheelDown();
             maxSize            =Setting::maxSize();
     QString bgColor            =Setting::background();
     QString fgColor            =Setting::foreground(mParent->palette().windowText().color().name());
     QString underline          =Setting::underline();
     QString overline           =Setting::overline();
     QString borderColor        =Setting::borderColor();
             mBoreder           =Setting::border();
     int     alpha              =Setting::alpha();
     QString fontName           =Setting::fontName(mParent->font().family());
     int     fontSize           =Setting::fontSize(mParent->font().pointSize());
     bool    fontBold           =Setting::fontBold(mParent->font().bold());
     int     radius             =Setting::radius();
     Setting::instance()->endGroup();

//     mySetting->beginGroup(groupName);

//      QString mCommand           =mySetting->command();
//      int     interval           =mySetting->interval();
//              mLabel             =mySetting->label();
//              mSuffix            =mySetting->suffix();
//              mPrefix            =mySetting->prefix();
//              mMouseLeftCmd      =mySetting->clickLeft();
//              mMouseRightCmd     =mySetting->clickRight();
//              mMouseWheelUpCmd   =mySetting->mouseWheelUp();
//              mMouseWheelDownCmd =mySetting->mouseWheelDown();
//              maxSize            =mySetting->maxSize();
//      QString bgColor            =mySetting->background();
//      QString fgColor            =mySetting->foreground(mParent->palette().windowText().color().name());
//      QString underline          =mySetting->underline();
//      QString overline           =mySetting->overline();
//      QString borderColor        =mySetting->borderColor();
//              mBoreder           =mySetting->border();
//      int     alpha              =mySetting->alpha();
//      QString fontName           =mySetting->fontName(mParent->font().family());
//      int     fontSize           =mySetting->fontSize(mParent->font().pointSize());
//      bool    fontBold           =mySetting->fontBold(mParent->font().bold());
//      int     radius             =mySetting->radius();
//      mySetting->endGroup();
    //_________________________________________________ INIT
       QFont font;
       font.setPointSize(fontSize);
       font.setFamily(fontName);
       font.setBold(fontBold);
       setFont(font);
       QFontMetrics fm(font);
       mHeight=fm.height()+(fm.leading()*2)+(mBoreder*2);

    if(mSuffix.contains("xrdb."))
        mSuffix=StyleColors::xrdbget(mSuffix);

    if(mPrefix.contains("xrdb."))
        mPrefix=StyleColors::xrdbget(mPrefix);

    if(mLabel.contains("xrdb."))
        mLabel=StyleColors::xrdbget(mLabel);

    if(interval<1000)interval=(60000*60)*24;//one day

    mMouseLeftCmd       =fixCommand(mMouseLeftCmd);
    mMouseRightCmd      =fixCommand(mMouseRightCmd);
    mMouseWheelUpCmd    =fixCommand(mMouseWheelUpCmd);
    mMouseWheelDownCmd  =fixCommand(mMouseWheelDownCmd);
    mCommand            =fixCommand(mCommand);

    if(Defines::degug()){

        qDebug()<<"\033[32m   [-] statu : "<<mName<< __LINE__<<"Command:"<<mCommand;
        qDebug()<<"   [-] statu : "<<mName<< __LINE__<<"MouseRightCmd:"<<mMouseRightCmd;
        qDebug()<<"   [-] statu : "<<mName<< __LINE__<<"MouseLeftCmd:"<<mMouseLeftCmd;
        qDebug()<<"   [-] statu : "<<mName<< __LINE__<<"MouseWheelUpCmd:"<<mMouseWheelUpCmd;
        qDebug()<<"   [-] statu : "<<mName<< __LINE__<<"MouseWheelDownCmd:"<<mMouseWheelDownCmd<<"\033[0m";

    }
    if(radius>0)
     setContentsMargins((radius/2)+1,0,(radius/2)+1,0);
    //_________________________________________________ STYLESHEET
    QString mystyle=StyleColors::style(bgColor,
                                       fgColor,
                                       underline,
                                       overline,
                                       mBoreder,
                                       alpha,
                                       borderColor,
                                       radius);

    setStyleSheet(mystyle);

    if(!mCommand.isEmpty()){
         mThread->setCommand(mCommand);
        startCommand(interval);
    }else{
        QString txt;
        if(!mSuffix.isEmpty())txt+=mSuffix;
        if(!mLabel.isEmpty())txt+=mLabel;
        if(!mPrefix.isEmpty())txt+=mPrefix;
        if(!txt.isEmpty()) setText(txt);

   }

}

//________________________________________________________________
void StatusLabel::startCommand(int interval)
{


    mTimer->setInterval(interval);
    connect(mTimer,SIGNAL(timeout()),this,SLOT(startRender()));
    mTimer->start();

    //********* old **********//
//    if(m_render->isPaused())
//   m_render->setPaused(false);
//    m_render->blockSignals(false);

    startRender();

}

//_______________________________________________________________Mouse
void StatusLabel::mouseReleaseEvent(QMouseEvent *ev)
{

    if (ev->button() == Qt::LeftButton){
        if(!mMouseLeftCmd.isEmpty()){
            execCmd(MouseLeft);
        }
    }else if(ev->button() == Qt::RightButton){
        if(!mMouseRightCmd.isEmpty()){
            execCmd(MouseRight);
        }
    }

}

//_______________________________________________________________ Mouse
void StatusLabel::wheelEvent(QWheelEvent* e)
{

    //  int delta = e->delta() < 0 ? 1 : -1;
    int delta = e->delta();
    if(delta>20){
        if(!mMouseWheelUpCmd.isEmpty())
            execCmd(MouseWheelUp);
    }else if(delta<-20){
        if(!mMouseWheelDownCmd.isEmpty())
            execCmd(MouseWheelDown);
    }

}

void StatusLabel::execCmd(int type)
{
    QProcess pr;

    switch (type) {
    case MouseLeft:
         pr.startDetached(mMouseLeftCmd);
        break;
    case MouseRight:
         pr.startDetached(mMouseRightCmd);
        break;
    case MouseWheelUp:
         pr.startDetached(mMouseWheelUpCmd);
        break;
    case MouseWheelDown:
         pr.startDetached(mMouseWheelDownCmd);
        break;
    default:
        break;
    }

    //تطبيق الامر اذا كان موجودا بمجرد النقر او الازاحة للعجلة
if(!mThread->command().isEmpty())
    startRender();

}

//________________________________________________________________ QFutureWatcher
//void StatusLabel::on_render_finished()
//{
//    setToolTip(QString());
//    if(m_string.length()>maxSize){
//       setToolTip(m_string);
//       m_string.resize(maxSize-3);
//       m_string+="...";
//    }

//    setText(QString(" %1 %2 %3 %4 ")
//            .arg(mSuffix)
//            .arg(mLabel)
//            .arg(m_string)
//            .arg(mPrefix));
// qDebug()<<mName<<m_string<<maxSize;
//}

//void StatusLabel::on_textReady(QString str)
//{
//    //********* old **********//
//    //if(!m_render->isCanceled()) m_string = str;
//}

void StatusLabel::startRender()
{
//    if(!m_render->isRunning())
//        m_render->setFuture(QtConcurrent::run(this, &StatusLabel::render));
    if(mThread->isRunning()){return;}

    mThread->start();
}

void StatusLabel::cancelRender()
{
    if(Defines::degug())   qDebug()<<"\033[32m   [-] statu : "<<mName<< __LINE__<<"close \033[0m"<<mName;
    mTimer->stop();
    mThread->terminate();
            //********* old **********//
//    m_render->cancel();
//    m_render->blockSignals(true);
//    m_string = QString();

}

//void StatusLabel::render()
//{

    //********* old **********//
//    if(m_render->isCanceled()) return;

//    QString  str = updateCommand();

//    if(m_render->isCanceled())return;

//    emit textReady(str);

//}

//QString StatusLabel::updateCommand()
//{
    //qDebug()<<__FILE__<< __LINE__<<"updateCommand()"<<mName<<mCommand;
 /*
    QProcess pr;
    pr.start(mCommand);
    if (!pr.waitForStarted())
        return QString();

    if (!pr.waitForFinished())
        return QString();

    QString result=pr.readAllStandardOutput();
    QString err=pr.readAllStandardError();

    QStringList list=result.split("\n",QString::SkipEmptyParts);
    QString s;
    if(list.count() >0){
        s=list.last();
      }
*/
// qDebug()<<__FILE__<< __LINE__<<"Command"<<mName<<s;
    //TODO add this
//    if(!err.isEmpty())
//        qDebug()<<"Error command"<<mName<<err;

    //  setText(QString(" %1 %2 ").arg(mLabel).arg(s.trimmed()));

 //   return s;

//}

//***********************  THREAD ******************************
void StatusLabel::updateCmd(QString result)
{
   // if(result.isEmpty())return;
    setToolTip(QString());
    if(result.length()>maxSize){
       setToolTip(result);
       result.resize(maxSize-1);
       result+="…";
    }


    setText(QString("%1 %2 %3 %4")
            .arg(mSuffix.trimmed())
            .arg(mLabel.trimmed())
            .arg(result.trimmed())
            .arg(mPrefix.trimmed()));


    if(Defines::degug())  qDebug()<<"\033[32m   [-] Statu : "<<mName<< __LINE__<<mName<<mLabel<<result<<"\033[0m";

}

void Thread::run()
{
    QProcess pr;

    pr.start(mCmd);
    if (!pr.waitForStarted())
         return ;

    if (!pr.waitForFinished())
        return ;

    QString result=pr.readAllStandardOutput();
    QString err=pr.readAllStandardError();
       if(!err.isEmpty())
         qDebug()<<"\033[31m   statu Error command:\033[0m"<<err;

    QStringList list=result.split("\n",QString::SkipEmptyParts);

    if(list.count() >0){
        emit terminated(list.last());
      }
   // return;
}


