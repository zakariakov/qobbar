#ifndef ACTIVEWINDOW_H
#define ACTIVEWINDOW_H

#include <QObject>
#include <QWidget>
#include <QToolButton>
#include <QLabel>
#include <QTimer>
#include <QAbstractNativeEventFilter>

class MToolButton : public QToolButton
{
    Q_OBJECT

public:
    MToolButton( QWidget *parent ) :
        QToolButton(parent)
    {
        setContentsMargins(0,0,0,0);

        this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
        this->setToolButtonStyle(Qt::ToolButtonIconOnly);
        QFont font=parent->font();
        font.setPointSize(parent->font().pointSize());
        setFont(font);

        //           QFontMetrics fm(parent->font());
        //           int size=fm.height();
    }

signals:

//    void activated();
public slots:
//  void  setData(QString txt){m_data=txt;}
//  QString data(){return  m_data;}
private:
    QString m_data;
};

//----------------------------------------------------------------
class ActiveWindow : public QWidget/*,public QAbstractNativeEventFilter*/
{
    Q_OBJECT
public:
    explicit ActiveWindow(QWidget *parent = nullptr);
 void setNativeEventFilter(const QByteArray &eventType, void *message, long *);
signals:

public slots:
  void loadSettings();
private slots:
void activeWindowChanged();
void closeActiveWindow();
void maxRestoreActiveWindow();
void minRestoreActiveWindow();

private:
     QWidget *mParent;
     QWidget *mWidBgr;
    MToolButton *btnClose;
    MToolButton *btnMax;
    MToolButton *btnMin;
    QLabel *labelTitle;
    QTimer *mTimer;
     unsigned long m_window ;

     QHash<QString ,bool>wState;
     QHash<QString ,bool>wAllow;
     int maxSize;
};

#endif // ACTIVEWINDOW_H
