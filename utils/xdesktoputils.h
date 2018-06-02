#ifndef XWINUTILS_H
#define XWINUTILS_H
#include <QStringList>
#include <X11/Xlib.h>

class XDesktop
{
public:
    XDesktop();
    /**
  * @brief desktopNames جلب قائمة باسماء اسطح المكتب
  * rootWindowProperty() سيجلب اولا خصائص النافذة الام
  * @return قائمة باسماء اسطح المكتب
  */
    static  QStringList names() ;
    /**
    * @brief desktopName اسم سطح المكتب الحالي
    * @param num معرف سطح المكتب الحالي
    * @param name الاسم الحالي
    * @return اسم سطح المكتب
    */
    static QString name(int num, const QString &dName) ;

    /**
   * @brief desktopCount جلب عدد اسطح المكتب
   * @return العدد
   */
    static int count() ;

    /**
    * @brief desktopCurrent سطح المكتب الحالي
    * @return معرف سطح المكتب الحالي
    */
    static  int active() ;

    /**
    * @brief setdesktopCurrent تفعيل سطح مكتب محدد
    * @param numDesktop معرف سكح المكتب
    */
    static  int setCurrent(int numDesktop) ;

    /**
    * @brief rootWindowProperty جلب خصائص النافذة الام
    * @return  windowProperty()
    */
    static  bool rootWindowProperty(Atom atom,               // property
                                    Atom reqType,            // req_type
                                    unsigned long* resultLen,// nitems_return
                                    unsigned char** result   // prop_return
                                    ) ;

    /**
     * @brief windowProperty جلب خصائص النافذة
     * @param window معرف النافذة
     * @return  XGetWindowProperty()
   */
    static bool windowProperty(Window window,
                               Atom atom,               // property
                               Atom reqType,            // req_type
                               unsigned long* resultLen,// nitems_return
                               unsigned char** result   // prop_return
                               );


    /**
   * @brief atom typedef unsigned long Atom		* Also in Xdefs.h *
   * @param atomName الاسم
   * @return atom
   */
    static Atom atom(const char* atomName);

};

#endif // XWINUTILS_H
