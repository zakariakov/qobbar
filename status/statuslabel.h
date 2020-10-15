#ifndef STATUSLABEL_H
#define STATUSLABEL_H
#include "utils/setting.h"
#include <QLabel>
#include <QTimer>
#include <QProcess>
#include <QFutureWatcher>
#include <QThread>

//********************* THREAD **************************
class Thread : public QThread
{
    Q_OBJECT

public:
    Thread(){}
    void setCommand(const QString &cmd){mCmd=cmd;}
    QString command(){return mCmd;}

signals:
    void terminated(const QString &result);

protected:
    void run();

private:
    QString     mCmd;
};

//********************* STATU **************************
class StatusLabel : public QLabel
{
    Q_OBJECT
signals:
    //-----------------------
    void readFiniched();
    void textReady(QString str);

public slots:
    void  loadSettings();
    // بداية القراءة
    void startRender();
protected:
    void mouseReleaseEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent* e);

public:
    //  البناء
    StatusLabel(QString group, QWidget *parent=nullptr);
    // انهاء البناء
    ~StatusLabel();
    //  اعادة حجم النص مع الاطار
    int heightSize(){return mHeight;}
    //  اسم العملية
    QString name(){return mName;}
    // انهاء الرندر
    void cancelRender();

private:
    enum CmdType { MouseLeft, MouseRight, MouseWheelUp, MouseWheelDown };

    Thread *mThread;                // العملية في الخلفية
    //_________________________________
    QStringList mRampList;
    QString     mLabel;
    //    QString mSuffix;
    //    QString mPrefix;
    QString     mName;              // الاسم
    int         mInterval;          // مدة اعادة تنفيذ الامر
    int         maxSize=100;        // الحجم الاقصى للحامل
    QTimer      *mTimer;            // ساعة ضبط التنفيذ
    QWidget     *mParent;           // النافذة الاب لتغيير لمعرفة اعداداتها
    QString     mMouseLeftCmd;      // امر نقر الموس الايسر
    QString     mMouseRightCmd;     // امر عند نقر الموس الايمن
    QString     mMouseWheelUpCmd;   // امر تدوير العجلة للاعلى
    QString     mMouseWheelDownCmd; //امر تدوير العجللة للاسفل
    int         mBoreder;           // عرض الاطار
    int         mHeight;            // حجم النض مع الاطار

    //________________________________
    //  بداية الرندر
    void startCommand(int interval);
    //  تنفيذ الامر عند النقر او سحب الماوس
    void execCmd(int type);


private slots:

    // تحديث القراءة
    void updateCmd(QString result);

    QString getRampIcon(QString str);
};

#endif // STATUSLABEL_H
