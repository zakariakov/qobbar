#include "panelapplication.h"
#include <QDebug>
PanelApplication::PanelApplication(int& argc, char** argv)
    : QApplication(argc, argv),
      mPanel(nullptr)
{
    setOrganizationName("qobbar");
    setApplicationName("qobbar");
    setApplicationDisplayName("QobBar");
    setLayoutDirection(Qt::LeftToRight);
    setApplicationVersion("0.2.1");
qApp->installNativeEventFilter(this);
}

bool PanelApplication::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    if(mPanel){
        //qDebug()<<"PanelApplication::nativeEventFilter";
        mPanel->setNativeEventFilter(eventType,message,result);
        //return true;
    }
   return false;
}
