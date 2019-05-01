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

#include "setting.h"
#include <QTextCodec>
#include <QDebug>
#include <QDir>
Setting::Setting()
{
    setDefaultFormat(QSettings::NativeFormat);
    setIniCodec(QTextCodec::codecForName("UTF-8"));
    sync();
}

//___________________________________________________
QString Setting::background(const QString &defaultValue)
{
    return value("Background",defaultValue).toString();
}
//___________________________________________________
QString Setting::foreground(const QString &defaultValue)
{
    return value("Foreground",defaultValue).toString();
}
//___________________________________________________
QString Setting::underline()
{
    return value("Underline").toString();
}
//___________________________________________________
QString Setting::overline()
{
    return value("Overline").toString();
}
//___________________________________________________
QString Setting::borderColor()
{
    return value("BorderColor").toString();
}
//___________________________________________________
QString Setting::fontName(const QString &defaultValue)
{
    return value("FontName",defaultValue).toString();
}
//___________________________________________________
int Setting::fontSize(int defaultValue)
{
    return value("FontSize",defaultValue).toInt();
}
//___________________________________________________
int Setting::radius(int defaultValue)
{
    return value("BorderRadius",defaultValue).toInt();
}

//___________________________________________________
bool Setting::fontBold(bool defaultValue)
{
    return value("FontBold",defaultValue).toBool();
}
//___________________________________________________
int Setting::border()
{
    return value("Border",0).toInt();
}
//___________________________________________________
int Setting::alpha()
{
    return value("Alpha",255).toInt();
}

/*---------------------------------------------------*
*                       Panel                        *
*---------------------------------------------------*/

//___________________________________________________
int Setting::barLeftSpacing()
{
    return value("BarLeftSpacing",0).toInt();
}
//___________________________________________________
int Setting::barRightSpacing()
{
    return value("BarRightSpacing",0).toInt();
}
//___________________________________________________
int Setting::barCenterSpacing()
{
    return value("BarCenterSpacing",0).toInt();
}
//___________________________________________________
bool Setting::top()
{
    return value("Top",true).toBool();
}
//___________________________________________________
QStringList Setting::barLeft()
{
    return value("BarLeft").toStringList();
}
//___________________________________________________
QStringList Setting::barRight()
{
    return value("BarRight").toStringList();
}
//___________________________________________________
QStringList Setting::barCenter()
{
    return value("BarCenter").toStringList();
}
//___________________________________________________
int  Setting::paddingLeft()
{
     return value("PaddingLeft",0).toInt();
}
//___________________________________________________
int  Setting::paddingTop()
{
     return value("PaddingTop",0).toInt();
}
//___________________________________________________
int  Setting::paddingRight()
{
     return value("PaddingRight",0).toInt();
}
//___________________________________________________
int  Setting::paddingBottom()
{
     return value("PaddingBottom",0).toInt();
}

//___________________________________________________
int  Setting::meginLeft()
{
     return value("MarginLeft",0).toInt();
}
//___________________________________________________
int  Setting::meginTop()
{
     return value("MarginTop",0).toInt();
}
//___________________________________________________
int  Setting::meginRight()
{
     return value("MarginRight",0).toInt();
}
//___________________________________________________
int  Setting::meginBottom()
{
     return value("MarginBottom",0).toInt();
}
//___________________________________________________
//int  Setting::panelHeight()
//{
//     return value("Height",0).toInt();
//}
bool Setting::showSystry()
{
    return value("Systray",false).toBool();
}
/*---------------------------------------------------*
*                       Pager                        *
*---------------------------------------------------*/

//___________________________________________________
int Setting::activeAlpha()
{
     return value("ActiveAlpha",255).toInt();
}
//___________________________________________________
QString Setting::activeBackground(const QString &defaultValue)
{
    return value("ActiveBackground",defaultValue).toString();
}
//___________________________________________________
QString Setting::activeForeground(const QString &defaultValue)
{
    return value("ActiveForeground",defaultValue).toString();
}
QString Setting::activeIcon(const QString &defaultValue)
{
    return value("ActiveIcon",defaultValue).toString();
}
//___________________________________________________
QString Setting::activeText(const QString &defaultValue)
{
    return value("ActiveText",defaultValue).toString();
}

//___________________________________________________
QString Setting::activeUnderline()
{
    return value("ActiveUnderline").toString();
}
//___________________________________________________
QString Setting::activeOverline()
{
    return value("ActiveOverline").toString();
}
//___________________________________________________
QString Setting::desktopDesplay()
{
    return value("DesktopDesplay","index").toString();
}
//___________________________________________________
QStringList Setting::iconsList()
{
    return value("IconsList").toStringList();
}

/*---------------------------------------------------*
*                       Statu                        *
*---------------------------------------------------*/

//___________________________________________________
QString Setting::command()
{
     return value("Command").toString();
}
//___________________________________________________
int Setting::interval()
{
     return value("Interval",1).toInt()*1000;
}
//___________________________________________________
int Setting::maxSize()
{
     return value("MaxSize",100).toInt();
}

//___________________________________________________
QString Setting::label()
{
     return value("Label").toString();
}
//___________________________________________________
QString Setting::suffix()
{
     return value("Suffix").toString();
}
//___________________________________________________
QString Setting::prefix()
{
     return value("Prefix").toString();
}
//___________________________________________________
QString Setting::clickLeft()
{
     return value("ClickLeft").toString();
}
//___________________________________________________
QString Setting::clickRight()
{
     return value("ClickRight").toString();
}
//___________________________________________________
QString Setting::mouseWheelUp()
{
     return value("MouseWheelUp").toString();
}
//___________________________________________________
QString Setting::mouseWheelDown()
{
     return value("MouseWheelDown").toString();
}
//___________________________________________________
