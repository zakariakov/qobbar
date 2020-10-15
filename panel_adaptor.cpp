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

void panel_adaptor::emitSignal(const QString &key)
{
    // handle method call org.tawhid.session.org.logout
    QMetaObject::invokeMethod(parent(), "emitSignal", Q_ARG(QString, key));
}

void panel_adaptor::reconfigure()
{
     qDebug()<<"parent dbus"<< parent()->objectName();
     QMetaObject::invokeMethod(parent(), "reconfigure");
}

 void panel_adaptor::showHide()
 {
      QMetaObject::invokeMethod(parent(), "showHide");
 }

 void panel_adaptor::exit()
 {
      QMetaObject::invokeMethod(parent(), "exit");
 }
