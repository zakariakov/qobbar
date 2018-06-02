#ifndef SETTING_H
#define SETTING_H

#include <QSettings>
class Setting : public QSettings
{
public:
    Setting();
    // Commun
    QString background(const QString &defaultValue=QString());
    QString foreground(const QString &defaultValue=QString());
    QString fontName(const QString &defaultValue);
    QString underline();
    QString overline();
    QString borderColor();
    int fontSize(int defaultValue);
    int border();
    bool fontBold(bool defaultValue);
    int alpha();

    //Panel
    bool top();
    int barLeftSpacing();
    int barRightSpacing();
    int barCenterSpacing();
    QStringList barLeft();
    QStringList barCenter();
    QStringList barRight();
    int   paddingLeft();
    int   paddingTop();
    int   paddingRight();
    int   paddingBottom();
    //Pager
    int activeAlpha();
    QString activeBackground(const QString &defaultValue=QString());
    QString activeForeground(const QString &defaultValue=QString());
    QString activeUnderline();
    QString activeOverline();
    QString desktopDesplay();
    QStringList iconsList();

    //Statu
    QString command();
    int interval();
    QString label();
    QString suffix();
    QString prefix();
    QString clickLeft();
    QString clickRight();
    QString mouseWheelUp();
    QString mouseWheelDown();


private:
    QString mGroup;
};

#endif // SETTING_H
