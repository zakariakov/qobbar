#ifndef PANELAPPLICATION_H
#define PANELAPPLICATION_H

#include "panelwidget.h"
#include <QObject>
#include <QApplication>

class PanelApplication : public QApplication,public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
     explicit PanelApplication(int& argc, char** argv);
    //explicit PanelApplication(QObject *parent = nullptr);
    void setPanel(PanelWidget* panel){ mPanel = panel;}
 bool nativeEventFilter(const QByteArray &eventType, void *message, long *result);
signals:

public slots:

private:

  PanelWidget  *mPanel;
};

#endif // PANELAPPLICATION_H
