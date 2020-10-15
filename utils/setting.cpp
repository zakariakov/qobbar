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
#include <QDir>
Q_GLOBAL_STATIC(Setting, SettingInstance)
Setting *Setting::instance()
{
    return SettingInstance();
}

Setting::Setting()
{
    setDefaultFormat(QSettings::NativeFormat);
    setIniCodec(QTextCodec::codecForName("UTF-8"));
    sync();
}


//___________________________________________________
QString Setting::background(const QString &defaultValue)
{
    return instance()->value("Background",defaultValue).toString();
}
//___________________________________________________
QString Setting::foreground(const QString &defaultValue)
{
    return instance()->value("Foreground",defaultValue).toString();
}
//___________________________________________________
QString Setting::underline()
{
    return instance()->value("Underline").toString();
}
//___________________________________________________
QString Setting::overline()
{
    return instance()->value("Overline").toString();
}
//___________________________________________________
QString Setting::borderColor()
{
    return instance()->value("BorderColor").toString();
}
//___________________________________________________
QString Setting::fontName(const QString &defaultValue)
{
    return instance()->value("FontName",defaultValue).toString();
}
//___________________________________________________
int Setting::fontSize(int defaultValue)
{
    return instance()->value("FontSize",defaultValue).toInt();
}
//___________________________________________________
int Setting::radius(int defaultValue)
{
    return instance()->value("BorderRadius",defaultValue).toInt();
}

//___________________________________________________
int Setting::leftTopRadius(int defaultValue)
{
    return instance()->value("LeftTopRadius",defaultValue).toInt();
}
//___________________________________________________
int Setting::rightTopRadius(int defaultValue)
{
    return instance()->value("RightTopRadius",defaultValue).toInt();
}
//___________________________________________________
int Setting::leftBottomRadius(int defaultValue)
{
    return instance()->value("LeftBottomRadius",defaultValue).toInt();
}
//___________________________________________________
int Setting::rightBottomRadius(int defaultValue)
{
    return instance()->value("RightBottomRadius",defaultValue).toInt();
}

//___________________________________________________
bool Setting::fontBold(bool defaultValue)
{
    return instance()->value("FontBold",defaultValue).toBool();
}
//___________________________________________________
int Setting::border()
{
    return instance()->value("Border",0).toInt();
}
//___________________________________________________
int Setting::alpha()
{
    return instance()->value("Alpha",255).toInt();
}

/*---------------------------------------------------*
*                       Panel                        *
*---------------------------------------------------*/
//___________________________________________________
int Setting::spacing()
{
    return instance()->value("Spacing",0).toInt();
}
//___________________________________________________
int Setting::barLeftSpacing()
{
    return instance()->value("BarLeftSpacing",0).toInt();
}
//___________________________________________________
int Setting::barRightSpacing()
{
    return instance()->value("BarRightSpacing",0).toInt();
}
//___________________________________________________
int Setting::barCenterSpacing()
{
    return instance()->value("BarCenterSpacing",0).toInt();
}
//___________________________________________________
bool Setting::top()
{
    return instance()->value("Top",true).toBool();
}
//___________________________________________________
QStringList Setting::barLeft()
{
    return instance()->value("BarLeft").toStringList();
}
//___________________________________________________
QStringList Setting::barRight()
{
    return instance()->value("BarRight").toStringList();
}
//___________________________________________________
QStringList Setting::barCenter()
{
    return instance()->value("BarCenter").toStringList();
}
//___________________________________________________
int  Setting::paddingLeft()
{
     return instance()->value("PaddingLeft",0).toInt();
}
//___________________________________________________
int  Setting::paddingTop()
{
     return instance()->value("PaddingTop",0).toInt();
}
//___________________________________________________
int  Setting::paddingRight()
{
     return instance()->value("PaddingRight",0).toInt();
}
//___________________________________________________
int  Setting::paddingBottom()
{
     return instance()->value("PaddingBottom",0).toInt();
}

//___________________________________________________
int  Setting::marginLeft()
{
     return instance()->value("MarginLeft",0).toInt();
}
//___________________________________________________
int  Setting::marginTop()
{
     return instance()->value("MarginTop",0).toInt();
}
//___________________________________________________
int  Setting::marginRight()
{
     return instance()->value("MarginRight",0).toInt();
}
//___________________________________________________
int  Setting::meginBottom()
{
     return instance()->value("MarginBottom",0).toInt();
}
//___________________________________________________
int  Setting::panelHeight()
{
     return instance()->value("Height",0).toInt();
}
bool Setting::showSystry()
{
    return instance()->value("Systray",false).toBool();
}
//___________________________________________________
int Setting::screen()
{
    return instance()->value("Monitor",0).toInt();
}
/*---------------------------------------------------*
*                       Pager                        *
*---------------------------------------------------*/

//___________________________________________________
int Setting::activeAlpha()
{
     return instance()->value("ActiveAlpha",255).toInt();
}
//___________________________________________________
QString Setting::activeBackground(const QString &defaultValue)
{
    return instance()->value("ActiveBackground",defaultValue).toString();
}
//___________________________________________________
QString Setting::activeForeground(const QString &defaultValue)
{
    return instance()->value("ActiveForeground",defaultValue).toString();
}
QString Setting::activeIcon(const QString &defaultValue)
{
    return instance()->value("ActiveIcon",defaultValue).toString();
}
//___________________________________________________
QString Setting::activeText(const QString &defaultValue)
{
    return instance()->value("ActiveText",defaultValue).toString();
}

//___________________________________________________
QString Setting::activeUnderline()
{
    return instance()->value("ActiveUnderline").toString();
}
//___________________________________________________
QString Setting::activeOverline()
{
    return instance()->value("ActiveOverline").toString();
}
//___________________________________________________
QString Setting::desktopDesplay()
{
    return instance()->value("DesktopDesplay","index").toString();
}
//___________________________________________________
QStringList Setting::iconsList()
{
    return instance()->value("IconsList").toStringList();
}

/*---------------------------------------------------*
*                       Statu                        *
*---------------------------------------------------*/

//___________________________________________________
QString Setting::command()
{
     return instance()->value("Command").toString();
}
//___________________________________________________
int Setting::interval()
{
     return instance()->value("Interval",1).toInt()*1000;
}
//___________________________________________________
int Setting::maxSize()
{
     return instance()->value("MaxSize",150).toInt();
}
//___________________________________________________
int Setting::minSize()
{
     return instance()->value("MinSize",0).toInt();
}

//___________________________________________________
QString Setting::label()
{
     return instance()->value("Label","$Command").toString();
}
////___________________________________________________
//QString Setting::suffix()
//{
//     return instance()->value("Suffix").toString();
//}
////___________________________________________________
//QString Setting::prefix()
//{
//     return instance()->value("Prefix").toString();
//}
//___________________________________________________
QStringList Setting::format()
{
     return instance()->value("Format",QStringList()<<"$Command").toStringList();
}
//___________________________________________________
QStringList Setting::ramps()
{
     return instance()->value("RampIcons").toStringList();
}

//___________________________________________________
QString Setting::clickLeft()
{
     return instance()->value("ClickLeft").toString();
}
//___________________________________________________
QString Setting::clickRight()
{
     return instance()->value("ClickRight").toString();
}
//___________________________________________________
QString Setting::mouseWheelUp()
{
     return instance()->value("MouseWheelUp").toString();
}
//___________________________________________________
QString Setting::mouseWheelDown()
{
     return instance()->value("MouseWheelDown").toString();
}
/*---------------------------------------------------*
*                 ACtiveWindow                       *
*---------------------------------------------------*/

//___________________________________________________
bool Setting::showButtons(bool defaultValue)
{
    return instance()->value("ShowButtons",defaultValue).toBool();
}
//___________________________________________________
QString Setting::closeColor(const QString &defaultValue)
{
    return instance()->value("CloseColor",defaultValue).toString();
}
//___________________________________________________
QString Setting::maxColor(const QString &defaultValue)
{
    return instance()->value("MaxColor",defaultValue).toString();
}
//___________________________________________________
QString Setting::minColor(const QString &defaultValue)
{
    return instance()->value("MinColor",defaultValue).toString();
}
//___________________________________________________
QString Setting::closeText(const QString &defaultValue)
{
    return instance()->value("CloseText",defaultValue).toString();
}
//___________________________________________________
QString Setting::maxText(const QString &defaultValue)
{
    return instance()->value("MaxText",defaultValue).toString();
}
//___________________________________________________
QString Setting::minText(const QString &defaultValue)
{
    return instance()->value("MinText",defaultValue).toString();
}

/*---------------------------------------------------*
*                 Variable Colors                   *
*---------------------------------------------------*/

QString Setting::vriableColor(QString key)
{
    return instance()->value(key).toString();
}
