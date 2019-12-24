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

#ifndef STYLECOLORS_H
#define STYLECOLORS_H


#include <QObject>
#include <QColor>
class StyleColors
{

public:

    explicit StyleColors(){}

    static   QString loadXresourceColor(const QString &colorName);

    static   QString style(QString bgColor, QString fgColor,
                         QString underLine, QString overLine,
                         int border=1, int alpha=255,
                         QString borderColor=QString(), int radius=0,
                         int leftRadius=0,int rightRadius=0);

    static   QString xrdbget(QString txt);

    static   QString loadVariableColor(QString key);
    static   QString getColors(QString col);
signals:

public slots:
};

#endif // STYLECOLORS_H
