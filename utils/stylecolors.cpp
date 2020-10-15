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

#include "stylecolors.h"
#include "utils/defines.h"
#include "utils/setting.h"
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QDir>
#include <QProcess>
#include <QDebug>

Q_GLOBAL_STATIC(StyleColors, StyleColorsInstance)
StyleColors *StyleColors::instance()
{
    return StyleColorsInstance();
}



void StyleColors::xrdbquey()
{
    QProcess pr;
    pr.start("xrdb",QStringList()<<"-query");
    if (!pr.waitForStarted())
        return ;

    if (!pr.waitForFinished())
        return ;

    QString str= pr.readAllStandardOutput();
    //QString str2= pr.readAllStandardError();

    //qDebug()<<"str2="<<str2;

    QStringList list=str.split("\n");
    list.removeDuplicates();
    if(list.count()<15){

        return;
    }

    QStringList listColor;

    foreach (QString s, list) {

        if(!s.startsWith("*")){
            continue;
        }

        QString key=s.section(":",0,0).trimmed();
key.remove("*");
key.remove(".");

        // background
        if(key.trimmed()==("background")){
            QString value=s.section(":",1,1).trimmed();
            if(value.startsWith("#")){
                qDebug()<< "background"<<value;
                instance()->colorsMap["background"]=value;

            }
        }
        // foreground
        if(key.trimmed()==("foreground")){
            QString value=s.section(":",1,1).trimmed();
            if(value.startsWith("#")){
                qDebug()<< "foreground"<<value;
                instance()->colorsMap["foreground"]=value;

            }
        }
        // color[num]
        for (int i = 0; i < 16; ++i) {
            if(key.trimmed()==("color"+QString::number(i))){
                QString value=s.section(":",1,1).trimmed();
                if(value.startsWith("#")){
                    qDebug()<< key<<value;
                    instance()->colorsMap["color"+QString::number(i)]=value;

                }
            }

            //qDebug()<<s<<key;

        }
    }
}
QString StyleColors::loadXresourceColor(const QString &colorName)
{

//    if(!colX.isEmpty()){
//        QString colX=xrdbquey(colorName);
//        return colX;
//    }
    QString colx=instance()->colorsMap.value(colorName);
    if(!colx.isEmpty()){
        return colx;
    }

    QString xresourceFile=QDir::homePath()+"/.Xresources";
    if(!QFile::exists(xresourceFile))
        xresourceFile=QDir::homePath()+"/.Xdefaults";
    if(!QFile::exists(xresourceFile))
        return QString();

    QFile files(xresourceFile);
    if(!files.open( QFile::ReadOnly))
        return QString();

    QTextStream textStream(&files);
    textStream.setCodec(QTextCodec::codecForName("UTF-8"));
    QString line ;//premier line;

    while   (!textStream.atEnd()) {

        line = textStream.readLine().trimmed();
        if(line.startsWith("#"))continue;

        if(line.startsWith("!"))continue;

        if(!line.contains(":")) continue;


        if(line.startsWith("*"+colorName)||line.startsWith("*."+colorName)){

            QString key=line.section(":",0,0).trimmed();
            QString value=line.section(":",1,1).trimmed();

            if (key.isEmpty())  continue;

            if (value.isEmpty()) continue;

            // qDebug()<<key.section(".",1,1) <<value;
            key.remove("*");
            key.remove(".");
            QString color=key.trimmed();

            if(color==colorName){
                files.close();
                return value;
            }

        }

    }

    files.close();
    return QString();

}
QString StyleColors::xrdbget(QString txt)
{

    if(txt.contains("xrdb.background")){
        txt.replace("xrdb.background",loadXresourceColor("background"));

    }

    if(txt.contains("xrdb.foreground")){
        txt.replace("xrdb.foreground",loadXresourceColor("foreground"));

    }

    for (int i = 0; i < 16; ++i) {
        QRegExp rx("xrdb.color"+QString::number(i)+"(\\s*)(;)");

        if(txt.contains(rx)){
            QString newCol=loadXresourceColor("color"+QString::number(i));
            txt.replace(rx,newCol+";");
        }

    }

    // if(Defines::degug()) qDebug()<<"   [*]"<<__FILE__<<__LINE__<< "StyleColors::xrdbget return="<<txt;
    return txt;

}

QString StyleColors::loadVariableColor(QString key)
{
    Setting::instance()->beginGroup("Colors");
    QString col= Setting::vriableColor(key);
    Setting::instance()->endGroup();

    return  col;
}


QString StyleColors::getColors(QString col)
{

    if(col.startsWith("$"))
        col=loadVariableColor(col.remove("$"));

    if(col.startsWith("xrdb"))
        col=loadXresourceColor(col.section(".",1));

    return col;
}

QString StyleColors::style(QString bgColor, QString fgColor,
                           QString underline, QString overline,
                           int border, int alpha, QString borderColor,
                           int radius, int leftTopRadius, int rightTopRadius,
                           int leftBotRadius, int rightBotRadius)
{

    bgColor=    getColors(bgColor);
    fgColor=    getColors(fgColor);
    underline=  getColors(underline);
    overline=   getColors(overline);
    borderColor=getColors(borderColor);

    QColor bg(bgColor);
    bg.setAlpha(alpha);

    QString mStyleSheet;
    if(bg.isValid())mStyleSheet+=QString("background-color:rgba(%1,%2,%3,%4);\n")
            .arg(bg.red()).arg(bg.green()).arg(bg.blue()).arg(bg.alpha());

    if(QColor(fgColor).isValid())mStyleSheet+=QString("color:%1;\n")
            .arg(fgColor);

    if(QColor(underline).isValid())mStyleSheet+=QString("border-bottom: %1px solid %2;\n")
            .arg(qMax(1,border)).arg(underline);
    else mStyleSheet+=QString("border-bottom: 0px;\n") ;

    if(QColor(overline).isValid())mStyleSheet+=QString("border-top: %1px solid %2;\n")
            .arg(qMax(1,border)).arg(overline);
    else  mStyleSheet+=QString("border-top: 0px;\n") ;

    if(QColor(borderColor).isValid())mStyleSheet+=QString("border: %1px solid %2;\n")
            .arg(qMax(1,border)).arg(borderColor);
    // else  mStyleSheet+=QString("border-top: 0px;\n") ;
    if(radius>0)
        mStyleSheet+=QString("border-radius: %1px;\n") .arg(QString::number(radius));
    else{
        if(leftTopRadius>0)  mStyleSheet+=QString("border-top-left-radius: %1px;\n").arg(QString::number(leftTopRadius));
        if(rightTopRadius>0) mStyleSheet+=QString("border-top-right-radius: %1px;\n").arg(QString::number(rightTopRadius));
        if(leftBotRadius>0)  mStyleSheet+=QString("border-bottom-left-radius: %1px;\n").arg(QString::number(leftBotRadius));
        if(rightBotRadius>0) mStyleSheet+=QString("border-bottom-right-radius: %1px;\n").arg(QString::number(rightBotRadius));
    }
    //qDebug()<<"   [*]"<<__FILE__<<__LINE__<<radius<<leftRadius<<rightRadius;
    //if(Defines::degug()) qDebug()<<"   [*]"<<__FILE__<<__LINE__<<"StyleSheet"<<mStyleSheet;
    return mStyleSheet;
}
