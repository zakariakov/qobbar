#ifndef CONKYSTATU_H
#define CONKYSTATU_H
#include "utils/setting.h"
#include <QWidget>

#include <QProcess>
#include <QLabel>
#include <QTimer>
class conkyStatu : public QLabel
{
        Q_OBJECT
public:
    conkyStatu(Setting *s, QWidget *parent=nullptr);


   void loadSettings();
   int heightSize(){return m_Height;}
private slots:
   void startConky();

private:

     QWidget  *m_Parent;
     Setting  *m_Setting;
     QProcess *m_process;
     int       m_Height;
};

#endif // CONKYSTATU_H
