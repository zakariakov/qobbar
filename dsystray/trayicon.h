/********************************************************************
 *(c)GPL3+
  Inspired by freedesktops tint2
  original code  http://razor-qt.org
  modified by abou zakaria
*********************************************************************/

#ifndef TRAYICON_H
#define TRAYICON_H

#include <QWidget>
#include <QPainter>
#include <QBitmap>
#include <QEvent>

#include <QX11Info>
#include <QDebug>
#include <X11/X.h>
#include <X11/extensions/Xdamage.h>

#define TRAY_ICON_SIZE_DEFAULT 16
class TrayIcon: public QWidget
{
  Q_OBJECT



public:
    TrayIcon(Window iconId, QSize const & iconSize, QWidget* parent);
    virtual ~TrayIcon();

    Window iconId() { return mIconId; }
    Window windowId() { return mWindowId; }
    void windowDestroyed(Window w);

    QSize iconSize() const { return mIconSize; }
    void setIconSize(QSize iconSize);

    QSize sizeHint() const;

protected:
    bool event(QEvent *event);
    void draw(QPaintEvent* event);

private:
    void init();
    QRect iconGeometry();
    Window mIconId;
    Window mWindowId;
    QSize mIconSize;
    Damage mDamage;
    Display* mDisplay;

    static bool isXCompositeAvailable();


};



#endif // TRAYICON_H
