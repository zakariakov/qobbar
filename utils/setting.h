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

#ifndef SETTING_H
#define SETTING_H

#include <QSettings>
class Setting : public QSettings
{
public:
    Setting();
    // Commun
    QString background(const QString &defaultValue=QString());
    QString foreground(const QString &defaultValue=QString());
    QString fontName(const QString &defaultValue);
    QString underline();
    QString overline();
    QString borderColor();
    int fontSize(int defaultValue);
    int radius(int defaultValue=0);
    int screen();
    int border();
    bool fontBold(bool defaultValue);
    int alpha();

    //Panel
    bool top();
    int barLeftSpacing();
    int barRightSpacing();
    int barCenterSpacing();
    QStringList barLeft();
    QStringList barCenter();
    QStringList barRight();
    int   paddingLeft();
    int   paddingTop();
    int   paddingRight();
    int   paddingBottom();
    int   meginLeft();
    int   meginTop();
    int   meginRight();
    int   meginBottom();
 //   int   panelHeight();
    bool  showSystry();
    //Pager
    int activeAlpha();
    QString activeBackground(const QString &defaultValue=QString());
    QString activeForeground(const QString &defaultValue=QString());
    QString activeText(const QString &defaultValue=QString());
    QString activeIcon(const QString &defaultValue=QString());

    QString activeUnderline();
    QString activeOverline();
    QString desktopDesplay();
    QStringList iconsList();

    //Statu
    QString command();
    int interval();
    int maxSize();
    QString label();
    QString suffix();
    QString prefix();
    QString clickLeft();
    QString clickRight();
    QString mouseWheelUp();
    QString mouseWheelDown();


private:
    QString mGroup;
};

#endif // SETTING_H
