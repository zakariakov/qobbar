#include "panel_adaptor.h"


panel_adaptor::panel_adaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

panel_adaptor::~panel_adaptor()
{
    // destructor

}


void panel_adaptor::setShortcut(const QString &shortcut)
{
    // handle method call org.tawhid.session.org.logout
    QMetaObject::invokeMethod(parent(), "setShortcut", Q_ARG(QString, shortcut));
}
void panel_adaptor::reconfigure()
{
  qDebug()<<"parent dbus"<< parent()->objectName();
     QMetaObject::invokeMethod(parent(), "reconfigure");
}

 void panel_adaptor::showMenu()
 {
      QMetaObject::invokeMethod(parent(), "showMenu");
 }

 void panel_adaptor::exit()
 {
      QMetaObject::invokeMethod(parent(), "exit");
 }
