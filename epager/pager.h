/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)GPL3+
 *
 * elokab - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/project/elokab/
 *
 * Copyright: 2013-2014 elokab team
 * Authors:
 *   Abou Zakaria <yahiaui@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#ifndef DESKTOPSWITCH_H
#define DESKTOPSWITCH_H
#include "utils/setting.h"
#include <QWidget>

#include <QAbstractNativeEventFilter>
#include <QToolButton>
#include <QLayout>
#include <xcb/xcb_event.h>
class QSignalMapper;
class QButtonGroup;
class ToolButton : public QToolButton
{
    Q_OBJECT

public:
    ToolButton( QWidget *parent ) :
        QToolButton(parent)
    {
        setContentsMargins(0,0,0,0);

        this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
        this->setToolButtonStyle(Qt::ToolButtonIconOnly);
        QFont font=parent->font();
        font.setPointSize(parent->font().pointSize());
        setFont(font);

        //           QFontMetrics fm(parent->font());
        //           int size=fm.height();
    }

signals:

    void activated();
};
/**
 * @brief The Pager class التبديل بين اسطح المكتب
 */
class Pager : public QWidget,public QAbstractNativeEventFilter

{
    Q_OBJECT

public:
    explicit    Pager(Setting *s,QWidget* parent = 0,bool debug=false);
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *);
    ~Pager();

    void setSize(QSize size);

public slots:
    void loadSettings();
private:

    QSignalMapper *m_pSignalMapper;
    int m_DeskCount;
    QWidget *mParent;
    Setting *mSetting;
    /*!< قائمة بمجموعة ازرار */
        QButtonGroup * m_GroupBtns;
    /*!< قائمة ياسماء اسطح المكتب */
    QStringList m_desktopNames;
    /*!< حجم الزر */
    QSize m_size;
bool mdebug;
    /**
     * @brief wheelEvent تحريك مؤشر العجلة
     * @param e
     */
    void wheelEvent(QWheelEvent* e);
    /**
     * @brief setupBtns تكوين الازرار
     */
    void setupBtns();
    QHBoxLayout * mHBoxLayout ;

enum DesktopType{DESKINDEX,DESKNAME,DESKICON};
int mDesktopType;
QStringList listIcons;
private slots:
    /**
     * @brief setDesktop الانتقال الى سطح مكتب
     * @param desktop رقم سطح المكتب
     */
    void setDesktop(int desktop);
    void goDesktop(int arg);
    void actvateBtnDesktop();
    void rechargeDesktop();

protected slots:

};




#endif
