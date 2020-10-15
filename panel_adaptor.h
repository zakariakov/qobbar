#ifndef RUN_ADAPTOR_H
#define RUN_ADAPTOR_H

#include <QObject>
#include <QtDBus/QtDBus>
#include <QWidget>
QT_BEGIN_NAMESPACE

class QString;

QT_END_NAMESPACE

/*
 * Adaptor class for interface org.tawhid.session.org
 */

class panel_adaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.elokab.panel.Interface")
    Q_CLASSINFO("D-Bus Introspection", ""
                                       "  <interface name=\"org.elokab.panel.Interface\">\n"
                                       "    <method name=\"emitSignal\">\n"
                                       "        <arg name=\"key\" type=\"s\" direction=\"in\"/>"
                                       "    </method>"
                                       "    <method name=\"reconfigure\">\n"
                                       "    </method>"
                                       "    <method name=\"showHide\">\n"
                                       "    </method>"
                                       "    <method name=\"exit\">\n"
                                       "    </method>"
                                       "  </interface>\n"
                                       "")
public:
    panel_adaptor(QObject *parent);
    virtual ~panel_adaptor();


public Q_SLOTS: // METHODS
    void emitSignal(const QString &key);
    void reconfigure();
    void showHide();
    void exit();
};


#endif
