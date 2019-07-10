#include "conkystatu.h"
#include "utils/stylecolors.h"
#include "utils/defines.h"
#include <QProcess>
#include <QDebug>
#include <QSettings>
#include <QDir>

QString fixFileName(QString cmd)
{
    if(cmd.contains("$HOME/") || cmd.contains("~/")){
        cmd.replace("$HOME/",QDir::homePath()+"/");
        cmd.replace("~/",QDir::homePath()+"/");
//        cmd.insert(0,QDir::homePath());
    }
    return cmd.trimmed();
}
conkyStatu::conkyStatu(/*Setting *s,*/ QWidget *parent):m_Parent(parent)/*,m_Setting(s)*/
{

    m_process=new QProcess;
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    setMargin(0);
    setContentsMargins(0,0,0,0);
connect(m_process,SIGNAL(readyReadStandardOutput()),this,SLOT(startConky()));
loadSettings();
}


void conkyStatu::loadSettings()
{

    m_process->close();

    Setting::instance()->beginGroup("Conky");
   // m_Setting->beginGroup("Conky");
    QString command            =Setting::command();
    int     border             =Setting::border();
    QString bgColor            =Setting::background();
    QString fgColor            =Setting::foreground(m_Parent->palette().windowText().color().name());
    QString borderColor        =Setting::borderColor();
    QString underline          =Setting::underline();
    QString overline           =Setting::overline();
    int     alpha              =Setting::alpha();
    QString fontName           =Setting::fontName(m_Parent->font().family());
    int     fontSize           =Setting::fontSize(m_Parent->font().pointSize());
    bool    fontBold           =Setting::fontBold(m_Parent->font().bold());
    int     radius             =Setting::radius();

    Setting::instance()->endGroup();
 if(Defines::degug()){
    // m_Setting->beginGroup("Conky");
        qDebug()<<"\033[32m   [-] ConkyStatu : "<< __LINE__;
        qDebug()<<"   [-] ConkyStatu : command :"<<command;
        qDebug()<<"   [-] ConkyStatu : border :"<<border;
        qDebug()<<"   [-] ConkyStatu : bgColor : "<<bgColor;
        qDebug()<<"   [-] ConkyStatu : borderColor: "<<borderColor;
        qDebug()<<"   [-] ConkyStatu : underline: "<<underline;
        qDebug()<<"   [-] ConkyStatu : overline: "<<overline;
        qDebug()<<"   [-] ConkyStatu : alpha: "<<alpha;
        qDebug()<<"   [-] ConkyStatu : fontName: "<<fontName;
        qDebug()<<"   [-] ConkyStatu : fontSize: "<<fontSize;
        qDebug()<<"   [-] ConkyStatu : birder radius: "<<radius<<"\033[0m";
}
    //_________________________________________________ Fonts
    QFont font;
    font.setPointSize(fontSize);
    font.setFamily(fontName);
    font.setBold(fontBold);
    setFont(font);

    QFontMetrics fm(font);
    m_Height=fm.height()+(fm.leading()*2)+(border*2);

    //_________________________________________________ STYLESHEET
    setContentsMargins((radius/2)+1,0,(radius/2)+1,0);
    QString mystyle=StyleColors::style(bgColor,
                                       fgColor,
                                       underline,
                                       overline,
                                       border,
                                       alpha,
                                       borderColor,
                                       radius);

    setStyleSheet(mystyle);

    command=fixFileName(command);

    m_process->start(command);

    startConky();

}

void conkyStatu::startConky()
{


    QString result=m_process->readAllStandardOutput();
    QString err=m_process->readAllStandardError();

    if(!err.isEmpty())
        qDebug()<<"\033[31m ConkyStatu Error command:"<<err;

    QStringList list=result.split("\n",QString::SkipEmptyParts);

    if(list.count() >0){
        QString out=list.last();
        if(out.contains("xrdb")){
            out=  StyleColors::xrdbget(out);
        }

        setText(out);
    }

}
