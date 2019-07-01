#include "conkystatu.h"
#include "utils/stylecolors.h"
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
conkyStatu::conkyStatu(Setting *s, QWidget *parent):m_Parent(parent),m_Setting(s)
{

    m_process=new QProcess;
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);

connect(m_process,SIGNAL(readyReadStandardOutput()),this,SLOT(startConky()));
loadSettings();
}


void conkyStatu::loadSettings()
{

    m_process->close();

    m_Setting->beginGroup("Conky");
    QString command            =m_Setting->command();
    int     border             =m_Setting->border();
    QString bgColor            =m_Setting->background();
    QString fgColor            =m_Setting->foreground(m_Parent->palette().windowText().color().name());
    QString borderColor        =m_Setting->borderColor();
    QString underline          =m_Setting->underline();
    QString overline           =m_Setting->overline();
    int     alpha              =m_Setting->alpha();
    QString fontName           =m_Setting->fontName(m_Parent->font().family());
    int     fontSize           =m_Setting->fontSize(m_Parent->font().pointSize());
    bool    fontBold           =m_Setting->fontBold(m_Parent->font().bold());
    int     radius             =m_Setting->radius();
    m_Setting->endGroup();

    //_________________________________________________ Fonts
    QFont font;
    font.setPointSize(fontSize);
    font.setFamily(fontName);
    font.setBold(fontBold);
    setFont(font);

    QFontMetrics fm(font);
    m_Height=fm.height()+(fm.leading()*2)+(border*2);
    qDebug()<<"conky height>>>>>>>>>>>>>>>>>>>>>>>"<<m_Height;
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
        qDebug()<<"Error command:"<<err;

    QStringList list=result.split("\n",QString::SkipEmptyParts);

    if(list.count() >0){
        QString out=list.last();
        if(out.contains("xrdb")){
            out=  StyleColors::xrdbget(out);
        }

        setText(out);
    }

}
