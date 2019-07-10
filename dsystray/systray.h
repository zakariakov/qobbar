/********************************************************************
 *(c)GPL3+
  Inspired by freedesktops tint2
original code  http://razor-qt.org
 modified by abou zakaria
*********************************************************************/

#ifndef SYSTRAY_H
#define SYSTRAY_H

#include <QtGui>

#include <QObject>

#include <QList>

#include <QLayout>
#include <QApplication>
#include <QtDebug>
#include <QX11Info>
#include <QTimer>
#include <QIcon>
#include <QResizeEvent>
#include <QPainter>
#include <QBitmap>
#include <QAbstractNativeEventFilter>

#include "trayicon.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>

class SysTray: public  QWidget, QAbstractNativeEventFilter
{
          Q_OBJECT



  public:
      SysTray(/*Setting *s, */QWidget* parent = nullptr);
      ~SysTray();


      QSize iconSize() const { return mIconSize; }
      void setIconSize(QSize iconSize);

      bool nativeEventFilter(const QByteArray &eventType, void *message, long *);

      void loadSettings();

  signals:
      void iconSizeChanged(int iconSize);

  private slots:
      void startTray();
      void stopTray();
      void onIconDestroyed(QObject * icon);

  private:
      VisualID getVisual();
QWidget *mWidgetContent;
      void clientMessageEvent(xcb_generic_event_t *e);

      int clientMessage(WId _wid, Atom _msg,
                        long unsigned int data0,
                        long unsigned int data1 = 0,
                        long unsigned int data2 = 0,
                        long unsigned int data3 = 0,
                        long unsigned int data4 = 0) const;


      void addIcon(Window id);
      TrayIcon* findIcon(Window trayId);
//Setting *mSetting;
      bool mValid;
      Window mTrayId;
      QList<TrayIcon*> mIcons;
      int mDamageEvent;
      int mDamageError;
      QSize mIconSize;
     QHBoxLayout *mLayout;

      Atom _NET_SYSTEM_TRAY_OPCODE;
      Display* mDisplay;
};





#endif //SYSTRAY_H
