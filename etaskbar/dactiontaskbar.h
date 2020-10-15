#ifndef DACTIONTASKBAR_H
#define DACTIONTASKBAR_H

#include <QtGui>
#include <QToolButton>

#include <X11/X.h>
//#include <X11/Xlib.h>
class DActionTaskbar : public QToolButton
{
          Q_OBJECT
public:

          explicit DActionTaskbar(const Window window, QWidget *parent =nullptr);
void setActiveWin(bool arg);
protected:
         virtual void dragEnterEvent(QDragEnterEvent *event);
        virtual void dragLeaveEvent(QDragLeaveEvent *);
          void mousePressEvent(QMouseEvent *event);


protected:
          void contextMenuEvent( QContextMenuEvent* event);
private:
          Window  m_Window;
          QAction *m_actParent;
           const QMimeData *mimeData;
           bool mActive;
private slots:

          QString appName();

          QString classeName();

          /**********عمليات النوافذ***************/
          void minimizeWindow();                  //تصغير النافذة
          void restoreWindow();                   //استعادة النافذة من التصغير
          void moveWindowToDesktop();             //نقل النافذة الى سطح المكت//
          void unMaximizeWindow();  //استعادة النافذة من التكبير
          void shadeWindow();          //تحجيم النافذة اي نظليلها
          void unShadeWindow();         //استعادة النافذة من التحجيم
          void closeApplication();           //غلق النافذة
          void maximizeWindow();        //تكبير التافذة
          void setApplicationLayer();             //وضع النافذة فوق تحت عادي

          void activateWithDraggable();
public slots:

          void getIcon();                //ايقونة البرنامج
          void getText();                //نص لابرنامج

void windowPropertyChanged(unsigned long /*atom*/);
};

#endif // DACTIONTASKBAR_H
