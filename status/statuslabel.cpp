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

//________________________________________________________________ Constructor
StatusLabel::StatusLabel(QString group, QWidget *parent):QLabel(parent),
    mName(group),mParent(parent)
{

    if(Defines::degug())  qDebug()<<"\033[32m   [-] statu : "<<mName<< __LINE__<<"Name:\033[0m"<<mName;
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);

    mTimer=new QTimer;
    mThread=new Thread;
    connect(mThread,SIGNAL(terminated(QString)),this,SLOT(updateCmd(QString)));
    /*_____________________________________________*/
    setMargin(0);
    setContentsMargins(0,0,0,0);

    loadSettings();

}

//________________________________________________________________
StatusLabel::~StatusLabel()
{

    delete mTimer;
    delete mThread;
}

//________________________________________________________________Settings
void StatusLabel::loadSettings()
{
    if(Defines::degug()) qDebug()<<"\033[32m   [-] statu : "<<mName<< __LINE__<<"loadSettings:\033[0m"<<mName;

    //
    mTimer->stop();

    QString groupName=mName;

    if(mName.contains(":")){
        groupName=mName.section(":",0,0);

    }

    Setting::instance()->beginGroup(groupName);

    QString mCommand           =Setting::command();
    int     interval           =Setting::interval();
    mLabel                     =Setting::label();
    //             mSuffix            =Setting::suffix();
    //             mPrefix            =Setting::prefix();
    mMouseLeftCmd              =Setting::clickLeft();
    mMouseRightCmd             =Setting::clickRight();
    mMouseWheelUpCmd           =Setting::mouseWheelUp();
    mMouseWheelDownCmd         =Setting::mouseWheelDown();
    maxSize                    =Setting::maxSize();
    QString bgColor            =Setting::background();
    QString fgColor            =Setting::foreground(mParent->palette().windowText().color().name());
    QString underline          =Setting::underline();
    QString overline           =Setting::overline();
    QString borderColor        =Setting::borderColor();
    mBoreder                   =Setting::border();
    int     alpha              =Setting::alpha();
    QString fontName           =Setting::fontName(mParent->font().family());
    int     fontSize           =Setting::fontSize(mParent->font().pointSize());
    bool    fontBold           =Setting::fontBold(mParent->font().bold());
    int     radius             =Setting::radius();
    Setting::instance()->endGroup();

    //_________________________________________________ INIT
    QFont font;
    font.setPointSize(fontSize);
    font.setFamily(fontName);
    font.setBold(fontBold);
    setFont(font);
    QFontMetrics fm(font);
    mHeight=fm.height()+(fm.leading()*2)+(mBoreder*2);

    //    if(mSuffix.contains("xrdb."))
    //        mSuffix=StyleColors::xrdbget(mSuffix);

    //    if(mPrefix.contains("xrdb."))
    //        mPrefix=StyleColors::xrdbget(mPrefix);

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
        QString txt=mLabel;
        txt.replace("$Command",QString());
        setText(txt);
    }

}

//_______________________________________________________________ Mouse
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

//_______________________________________________________________ Mouse
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

//________________________________________________________________
void StatusLabel::startCommand(int interval)
{

    mTimer->setInterval(interval);
    connect(mTimer,SIGNAL(timeout()),this,SLOT(startRender()));
    mTimer->start();
    startRender();

}

//________________________________________________________________
void StatusLabel::startRender()
{
    if(mThread->isRunning()){return;}

    mThread->start();
}

//________________________________________________________________
void StatusLabel::cancelRender()
{
    if(Defines::degug())   qDebug()<<"\033[32m   [-] statu : "<<mName<< __LINE__<<"close \033[0m"<<mName;

    mTimer->stop();
    //mThread->terminate();
    mThread->blockSignals(true);
}


//***********************  THREAD ******************************
void StatusLabel::updateCmd(QString result)
{



    setToolTip(QString());
    if(result.length()>maxSize){
        setToolTip(result);
        result.resize(maxSize-1);
        result+="…";
    }

    QString txt=mLabel;

    if(txt.contains("$Command"))
        txt=txt.replace("$Command",result.trimmed());
    else
        txt+=result.trimmed();

    setText(txt);
    if(Defines::degug())  qDebug()<<"\033[32m   [-] Statu : "<<mName<< __LINE__<<txt<<"\033[0m";

}

//________________________________________________________________
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

    if(Defines::degug() && !err.isEmpty())
        qDebug()<<"\033[31m   statu Error command:\033[0m"<<err;

    QStringList list=result.split("\n",QString::SkipEmptyParts);

    if(list.count() >0){
        emit terminated(list.last());
    }

}


