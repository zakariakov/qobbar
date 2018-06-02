#include "statuslabel.h"
#include "utils/stylecolors.h"
#include <QDebug>
#include <QApplication>
#include <QtConcurrent>
#include <QSettings>
#include <QMouseEvent>
QString fixCommand(QString cmd)
{
    if(cmd.startsWith("$HOME/") || cmd.startsWith("~/")){
        cmd.remove("$HOME");
        cmd.remove("~");
        cmd.insert(0,QDir::homePath());
    }
    return cmd.trimmed();
}
//Constructor
StatusLabel::StatusLabel(QString group, Setting *s, QWidget *parent,bool debug):QLabel(parent),
    mName(group),mParent(parent),mySetting(s),mdebug(debug)
{

   if(mdebug)  qDebug()<<"   [-]"<<__FILE__<< __LINE__<<"Name:"<<mName;

   mTimer=new QTimer;
     setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);

    /*_____________________________________________*/
    m_render = new QFutureWatcher< void >(this);

    connect(m_render, SIGNAL(finished()),
            SLOT(on_render_finished()));
    connect(this, SIGNAL(textReady(QString))
            , SLOT(on_textReady(QString)));
    /*_____________________________________________*/
setMargin(0);
setContentsMargins(0,0,0,0);


 loadSettings();



}
StatusLabel::~StatusLabel()
{

 // cancelRender();
  delete m_render;
  delete mTimer;


}

//________________________________________________________________Settings
void StatusLabel::loadSettings()
{
   if(mdebug) qDebug()<<"   [-]"<<__FILE__<< __LINE__<<"loadSettings:"<<mName;

    mySetting->beginGroup(mName);

             mCommand           =mySetting->command();
     int     interval           =mySetting->interval();
             mLabel             =mySetting->label();
             mSuffix            =mySetting->suffix();
             mPrefix            =mySetting->prefix();
             mMouseLeftCmd      =mySetting->clickLeft();
             mMouseRightCmd     =mySetting->clickRight();
             mMouseWheelUpCmd   =mySetting->mouseWheelUp();
             mMouseWheelDownCmd =mySetting->mouseWheelDown();
     QString bgColor            =mySetting->background();
     QString fgColor            =mySetting->foreground();
     QString underline          =mySetting->underline();
     QString overline           =mySetting->overline();
             mBoreder           =mySetting->border();
     int     alpha              =mySetting->alpha();
     QString fontName           =mySetting->fontName(mParent->font().family());
     int     fontSize           =mySetting->fontSize(mParent->font().pointSize());
     bool    fontBold           =mySetting->fontBold(mParent->font().bold());

     mySetting->endGroup();

    //_________________________________________________ INIT
       QFont font;
       font.setPointSize(fontSize);
       font.setFamily(fontName);
       font.setBold(fontBold);

       setFont(font);

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

   if(mdebug) qDebug()<<"   [-]"<<__FILE__<< __LINE__<<mName<<"Command:"<<mCommand;
   if(mdebug) qDebug()<<"   [-]"<<__FILE__<< __LINE__<<mName<<"MouseRightCmd:"<<mMouseRightCmd;
   if(mdebug) qDebug()<<"   [-]"<<__FILE__<< __LINE__<<mName<<"MouseLeftCmd:"<<mMouseLeftCmd;
   if(mdebug) qDebug()<<"   [-]"<<__FILE__<< __LINE__<<mName<<"MouseWheelUpCmd:"<<mMouseWheelUpCmd;
   if(mdebug) qDebug()<<"   [-]"<<__FILE__<< __LINE__<<mName<<"MouseWheelDownCmd:"<<mMouseWheelDownCmd;

   //
    mTimer->stop();

    if(!mCommand.isEmpty())
        startCommand(interval);
    else
       setText(mSuffix+mLabel+mPrefix);

    //_________________________________________________ STYLESHEET
    QString mystyle=StyleColors::style(bgColor,fgColor,underline,overline,mBoreder,alpha);
    setStyleSheet(mystyle);

}

//________________________________________________________________
void StatusLabel::startCommand(int interval)
{

    mTimer->setInterval(interval);
    connect(mTimer,SIGNAL(timeout()),this,SLOT(startRender()));
    mTimer->start();
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
        execCmd(MouseWheelUp);
    }else if(delta<-20){
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

}
//________________________________________________________________ QFutureWatcher
void StatusLabel::on_render_finished()
{
    // setToolTip(m_string);
    if(m_string.length()>50){
       m_string.resize(47);
       m_string+="...";
    }

    setText(QString("%1 %2 %3 %4")
            .arg(mSuffix)
            .arg(mLabel)
            .arg(m_string.trimmed())
            .arg(mPrefix));
   // qDebug()<<mName<<m_string;
}

void StatusLabel::on_textReady(QString str)
{
    if(!m_render->isCanceled()) m_string = str;
}

void StatusLabel::startRender()
{
    if(!m_render->isRunning())
        m_render->setFuture(QtConcurrent::run(this, &StatusLabel::render));
}

void StatusLabel::cancelRender()
{
       qDebug()<<"   [-]"<<__FILE__<< __LINE__<<"close"<<mName;
    mTimer->stop();
    m_render->cancel();
    m_render->blockSignals(true);
    m_string = QString();

}

void StatusLabel::render()
{

    if(m_render->isCanceled()) return;

    QString  str = updateCommand();

    if(m_render->isCanceled())return;

    emit textReady(str);

}

QString StatusLabel::updateCommand()
{
    //qDebug()<<__FILE__<< __LINE__<<"updateCommand()"<<mName<<mCommand;
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

// qDebug()<<__FILE__<< __LINE__<<"Command"<<mName<<s;
    //TODO add this
//    if(!err.isEmpty())
//        qDebug()<<"Error command"<<mName<<err;

    //  setText(QString(" %1 %2 ").arg(mLabel).arg(s.trimmed()));

    return s;

}



