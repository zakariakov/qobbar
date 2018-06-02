#ifndef STYLECOLORS_H
#define STYLECOLORS_H

#include <QObject>
#include <QColor>
class StyleColors
{

public:

    explicit StyleColors(){}

    static   QString loadXresourceColor(const QString &colorName);

    static   QString style(QString bgColor, QString fgColor,
                         QString underLine, QString overLine,
                         int border=1, int alpha=255, QString borderColor=QString());

    static   QString xrdbget(QString txt);

signals:

public slots:
};

#endif // STYLECOLORS_H
