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
    static Setting *instance();
    // Commun
    static  QString background(const QString &defaultValue=QString());
    static QString foreground(const QString &defaultValue=QString());

     static QString fontName(const QString &defaultValue);
    static  QString underline();
     static QString overline();
     static QString borderColor();
     static int fontSize(int defaultValue);
     static int radius(int defaultValue=0);
     static int screen();
     static int border();
     static bool fontBold(bool defaultValue);
     static int alpha();

    //Panel
     static bool top();
     static int   spacing();
     static int barLeftSpacing();
     static int barRightSpacing();
     static int barCenterSpacing();
     static QStringList barLeft();
    static  QStringList barCenter();
    static  QStringList barRight();
     static int   paddingLeft();
     static int   paddingTop();
     static int   paddingRight();
     static int   paddingBottom();
     static int   marginLeft();
     static int   marginTop();
     static int   marginRight();
     static int   meginBottom();
     static int   panelHeight();
     static bool  showSystry();
    //Pager
     static int activeAlpha();
    static  QString activeBackground(const QString &defaultValue=QString());
    static  QString activeForeground(const QString &defaultValue=QString());
    static  QString activeText(const QString &defaultValue=QString());
    static  QString activeIcon(const QString &defaultValue=QString());

     static QString activeUnderline();
     static QString activeOverline();
     static QString desktopDesplay();
     static QStringList iconsList();

    //Statu
    static  QString command();
    static  int interval();
    static  int maxSize();
    static  QString label();
//    static  QString suffix();
//    static  QString prefix();
 static  QStringList format();
 static  QStringList ramps();
    static  QString clickLeft();
    static  QString clickRight();
    static  QString mouseWheelUp();
    static  QString mouseWheelDown();

      static QString closeColor(const QString &defaultValue=QString());
      static QString maxColor(const QString &defaultValue=QString());
      static QString minColor(const QString &defaultValue=QString());
      static QString closeText(const QString &defaultValue=QString());
      static QString maxText(const QString &defaultValue=QString());
      static QString minText(const QString &defaultValue=QString());



private:
    QString mGroup;
};

#endif // SETTING_H
