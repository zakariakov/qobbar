#ifndef DTASKBARWIDGET_H
#define DTASKBARWIDGET_H

#include "dactiontaskbar.h"
#include "utils/setting.h"
#include <QWidget>

#include <QLayout>
#include <QSpacerItem>
//#include <KF5/KWindowSystem/KWindowSystem>
//#include <KF5/KWindowSystem/KWindowInfo>
//#include <KF5/KWindowSystem/NETWM>

#include "X11/Xlib.h"
#include <X11/Xatom.h>
#include <xcb/xcb_event.h>

class DtaskbarWidget : public QWidget,public QAbstractNativeEventFilter
{

          Q_OBJECT

public:
          // DtaskbarWidget();
      explicit DtaskbarWidget(Setting *s,QWidget *parent = nullptr,bool debug=false);

      bool nativeEventFilter(const QByteArray &eventType, void *message, long *);
      void windowPropertyChanged(unsigned long window, unsigned long atom);

protected:

signals:

public slots:

void loadSettings();


private slots:


          void refreshTaskList();
          void activeWindowChanged();
          void wheelEvent(QWheelEvent* event);


private:
          QWidget *m_parent;
          Setting *mSetting;
          QWidget *widgetContent;
          bool mDebug;
         // QMap<unsigned long, Client*> m_clients;
          QHash<unsigned long, DActionTaskbar*> mButtonsHash;
          unsigned long m_rootWindow;
          unsigned long m_activeWindow;
          QHBoxLayout * m_horizontalLayout ;


};

#endif // DTASKBARWIDGET_H
