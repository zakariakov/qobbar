#ifndef X11SUPPORT_H
#define X11SUPPORT_H
#include <QDebug>
#include <QApplication>
#include <QX11Info>
#include <QImage>
#include <QVector>
#include <QMap>
#include <QHash>
#include <QObject>
#include <QIcon>
#include <QPixmap>
#include "X11/Xlib.h"

typedef QList<unsigned long> WindowList;
typedef QList<Atom> AtomList;
/**
 * @brief The X11UTILLS class فئة التعامل مع مدراء النوافذ
 */
class X11UTILLS: public QObject
{
    Q_OBJECT
public:
    X11UTILLS();
    ~X11UTILLS();
   static X11UTILLS *instance();

//   static X11UTILLS *instance();
    /**
     * @brief atom
     * @param atomName
     * @return
     */
    static unsigned long atom(const QString& atomName);
    /**
     * @brief removeWindowProperty
     * @param window
     * @param name
     */
    static void removeWindowProperty(Window window, const QString& name);
    /**
     * @brief setWindowPropertyCardinalArray
     * @param window
     * @param name
     * @param values
     */
    static void setWindowPropertyCardinalArray(Window window, const QString& name, const QVector<unsigned long>& values);
    /**
     * @brief getClientList قائمة بالنوافذ الصالحة لمدير المهام
     * @return QList<unsigned long> WindowList;
     */
    static WindowList getClientList() ;

    /**
     * @brief getWindowProperty
     * @param window
     * @param atom
     * @param reqType
     * @param resultLen
     * @param result
     * @return
     */
    static bool getWindowProperty(unsigned long window,
                                  Atom atom,               // property
                                  Atom reqType,            // req_type
                                  unsigned long* resultLen,// nitems_return
                                  unsigned char** result   // prop_return
                                  ) ;
    /**
     * @brief getRootWindowProperty
     * @param atom
     * @param reqType
     * @param resultLen
     * @param result
     * @return
     */
    static bool getRootWindowProperty(Atom atom,    // property
                                      Atom reqType,            // req_type
                                      unsigned long* resultLen,// nitems_return
                                      unsigned char** result   // prop_return
                                      ) ;
    /**
     * @brief isWindowForTaskbar هل النافذة صالحة لمبدل المهام
     * @param window معرف النافذة
     * @return نعم او لا
     */
    static bool isWindowForTaskbar(Window window) ;
    /**
     * @brief getWindowType جلب نوع النافذة
     * @param window معرف النافذة
     * @return
     */
    static AtomList getWindowType(Window window) ;
    /**
   * @brief getActiveAppWindow جلب التطبيق النشط
   * @return عنوان النافذة
   */
    static Window getActiveAppWindow() ;
    /**
     * @brief getActiveWindow جلب النافذة النشطة
     * @return  عنوان النافذة
     */
    static Window getActiveWindow() ;
    /**
     * @brief getClientIcon جلب ايقونة النافذة
     * @param _wid معرف النافذة
     * @param _pixreturn الضورة التي ستجلب
     * @return الصورة
     */
    static bool getClientIcon(Window _wid, QPixmap& _pixreturn) ;
    /**
     * @brief sendWindowMessage اشارة الى مدير النوافذ
     * @param _wid معرف النافذة
     * @param _msg
     * @param data0
     * @param data1
     * @param data2
     * @param data3
     * @param data4
     * @return
     */
    static int sendWindowMessage(Window _wid, Atom _msg,
                                 unsigned long data0,
                                 long unsigned int data1 = 0,
                                 long unsigned int data2 = 0,
                                 long unsigned int data3 = 0,
                                 long unsigned int data4 = 0) ;
    /**
     * @brief moveWindowToDesktop ارسال النافذة الى سطح مكتب معين
     * @param _wid معرف النافذة
     * @param _display رقم سطح المكتب
     */
    static void moveWindowToDesktop(Window _wid, int _display);
    /**
 * @brief getNumDesktop جلب عدد اسطح المكتب
 * @return
 */
    static int getNumDesktop() ;
    /**
     * @brief getWindowTitleUTF8String  جلب عنوان النافذة من نوع اونيكود
     * @param window معرف النافذة
     * @param name
     * @return
     */
    static QString getWindowTitleUTF8String(unsigned long window, const QString& name);
    /**
     * @brief getWindowPropertyLatin1String
     * @param window
     * @param name
     * @return
     */
    static QString getWindowTitleLatin1String(unsigned long window, const QString& name);
    /**
     * @brief getWindowTitle جلب عنوان النافذة
     * @param window معرف النافذة
     * @return عنوان النافذة
     */
    static QString getWindowTitle(unsigned long window);
    /**
     * @brief getWindowIcon
     * @param window
     * @return
     */
    static QIcon getWindowIcon(unsigned long window);
    /**
     * @brief getApplicationName
     * @param _wid
     * @return
     */
    static QString getApplicationName(unsigned long _wid) ;
    /**
     * @brief getApplicationClasseName
     * @param _wid
     * @return
     */
    static QString getApplicationClasseName(unsigned long _wid) ;
    /**
     * @brief setActiveDesktop
     * @param _desktop
     */
    static void setActiveDesktop(int _desktop);
    /**
     * @brief getActiveDesktop
     * @return
     */
    static int getActiveDesktop() ;
    /**
     * @brief getWindowDesktop
     * @param _wid
     * @return
     */
    static int getWindowDesktop(unsigned long _wid);
    /**
     * @brief raiseWindow
     * @param _wid
     */
    static void raiseWindow(unsigned long _wid) ;
    /**
     * @brief minimizeWindow
     * @param _wid
     */
    static void minimizeWindow(unsigned long _wid) ;
    /**
     * @brief maximizeWindow
     * @param _wid
     * @param direction
     */
    static void maximizeWindow(unsigned long _wid, int direction = 2) ;
    /**
     * @brief deMaximizeWindow
     * @param _wid
     */
    static void deMaximizeWindow(unsigned long _wid) ;
    /**
     * @brief shadeWindow
     * @param _wid
     * @param shade
     */
    static void shadeWindow(unsigned long _wid, bool shade) ;
    /**
     * @brief resizeWindow
     * @param _wid
     * @param _width
     * @param _height
     */
    static void resizeWindow(unsigned long _wid, int _width, int _height) ;
    /**
     * @brief closeWindow
     * @param _wid
     */
    static void closeWindow(unsigned long _wid) ;
    /**
    * @brief setWindowLayer
    * @param _wid
    * @param layer
    */
    static  void setWindowLayer(unsigned long _wid, int layer) ;

    /**
     * @brief states حالات النافذة مثل فوق ت
    * @param window
     * @return
    */
    static QHash<QString ,bool> states(unsigned long window);
    /**
     * @brief allowed
     * @param window
     * @return
     */
    static QHash<QString ,bool>  allowed(unsigned long window) ;
    /**
    * @brief getwindowGeometry جلب حجم النافذة وموضعها
    * @param window معرف النافذة
    * @return مربع النافذة
    */
    static QRect getwindowGeometry (unsigned long window);
    /**
    * @brief isWindowManagerActive معرفة هل يوجد مدير نوافذ
    * @return نعم لا
    */
    static bool isWindowManagerActive() ;
    void moveWindow(Window _win, int _x, int _y) const;
        static Atom xatom(const char* atomName);
    QHash<QString ,Atom>m_cachedAtoms;
};

#endif
