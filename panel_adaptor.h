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
                                       "    <method name=\"setShortcut\">\n"
                                       "        <arg name=\"shortcut\" type=\"s\" direction=\"in\"/>"
                                       "    </method>"
                                       "    <method name=\"reconfigure\">\n"
                                       "    </method>"
                                       "    <method name=\"showMenu\">\n"
                                       "    </method>"
                                       "    <method name=\"exit\">\n"
                                       "    </method>"
                                       "  </interface>\n"
                                       "")
public:
    panel_adaptor(QObject *parent);
    virtual ~panel_adaptor();


public Q_SLOTS: // METHODS
    void setShortcut(const QString &shortcut);
    void reconfigure();
    void showMenu();
    void exit();
};


#endif
