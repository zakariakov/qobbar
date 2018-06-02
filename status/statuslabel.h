#ifndef STATUSLABEL_H
#define STATUSLABEL_H
#include "utils/setting.h"
#include <QLabel>
#include <QTimer>
#include <QProcess>
#include <QFutureWatcher>

class StatusLabel : public QLabel
{
    Q_OBJECT
signals:
    //-----------------------
    void readFiniched();
    void textReady(QString str);

protected slots:
    void on_textReady(QString str);
    void on_render_finished();

public slots:

 void    loadSettings();
protected:
    void mouseReleaseEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent* e);
    //__________________________________________
public:
    StatusLabel(QString group,Setting *s, QWidget *parent=0,bool debug=false);
      ~StatusLabel();
int border(){return mBoreder;}
    QString name(){return mName;}

void    cancelRender();
private:
     enum CmdType { MouseLeft, MouseRight, MouseWheelUp, MouseWheelDown };

    //_________________________________
    QString mCommand;
    QString mLabel;
    QString mSuffix;
    QString mPrefix;
    QString mName;
    int     mInterval;
    QTimer *mTimer;
QWidget *mParent;
Setting *mySetting;
    QString mMouseLeftCmd;
    QString mMouseRightCmd;
    QString mMouseWheelUpCmd;
    QString mMouseWheelDownCmd;
    int mBoreder;
    //________________________________
    QFutureWatcher< void >* m_render;
    QString m_string;
    void render();
    void startCommand(int interval);
    void execCmd(int type);
bool mdebug;
private slots:
    QString updateCommand();

    void    startRender();
    //void    cancelRender();
};

#endif // STATUSLABEL_H
