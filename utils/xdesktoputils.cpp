#include "xdesktoputils.h"
#include <X11/Xutil.h>
#include <assert.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <QX11Info>
#include <QHash>

//#define EXIT_SUCCESS 1
//#define EXIT_FAILURE 0;
XDesktop::XDesktop()
{

}
//قائمة باسماء اسطح المكتب
QStringList XDesktop::names()
{
    QStringList ret;
    unsigned long length;
    unsigned char *data = 0;

    if (rootWindowProperty(atom("_NET_DESKTOP_NAMES"), atom("UTF8_STRING"), &length, &data))
    {
        if (data)
        {
            char* c = (char*)data;
            char* end = (char*)data + length;
            while (c < end)
            {
                ret << QString::fromUtf8(c);
                c += strlen(c) + 1; // for trailing \0
            }

            XFree(data);
        }
    }


    return ret;
}
//اسم سطح المكتب الحالي
QString XDesktop::name(int num, const QString &dName)
{
    QStringList lnames = names();
    if (num<0 || num>lnames.count()-1)
        return dName;

    return lnames.at(num);
}
// عدد اسطح المكتب
int XDesktop::count()
{
    unsigned long length, *data;
    rootWindowProperty(atom("_NET_NUMBER_OF_DESKTOPS"), XA_CARDINAL, &length, (unsigned char**) &data);
    if (data)
    {
        int res = data[0];
        XFree(data);
        return res;
    }
    return 0;
}
//السطح النشط
int XDesktop::active()
{
    int res = -2;
    unsigned long length, *data;
    if (rootWindowProperty(atom("_NET_CURRENT_DESKTOP"), XA_CARDINAL, &length, (unsigned char**) &data))
    {
        if (data)
        {
            res = data[0];
            XFree(data);
        }
    }

    return res;
}
//تفعيل سطح مكتب محدد
int XDesktop::setCurrent(int numDesktop)
{
   // xSendMessage(QX11Info::appRootWindow(), atom("_NET_CURRENT_DESKTOP"), (unsigned long) numDesktop);
    XClientMessageEvent msg;
    msg.window = QX11Info::appRootWindow();
    msg.type = 33;//ClientMessage
    msg.message_type = atom("_NET_CURRENT_DESKTOP");
    msg.send_event = true;
    msg.display = QX11Info::display();
    msg.format = 32;
    msg.data.l[0] =  (unsigned long) numDesktop;

    if (XSendEvent(QX11Info::display(), QX11Info::appRootWindow(), 0, (SubstructureRedirectMask | SubstructureNotifyMask) , (XEvent *) &msg) == Success)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}
//خصائص النافذة الام
bool XDesktop::rootWindowProperty(Atom atom,    // property
                           Atom reqType,            // req_type
                           unsigned long* resultLen,// nitems_return
                           unsigned char** result   // prop_return
                          )
{
    return windowProperty( QX11Info::appRootWindow(), atom, reqType, resultLen, result);
}
//خصائص النافذة
bool XDesktop::windowProperty(Window window,
                       Atom atom,               // property
                       Atom reqType,            // req_type
                       unsigned long* resultLen,// nitems_return
                       unsigned char** result   // prop_return
                      )
{
    int  format;
    unsigned long type, rest;

//    int XGetWindowProperty(
//        Display*		/* display */,
//        Window		/* w */,
//        Atom		/* property */,
//        long		/* long_offset */,
//        long		/* long_length */,
//        Bool		/* delete */,
//        Atom		/* req_type */,
//        Atom*		/* actual_type_return */,
//        int*		/* actual_format_return */,
//        unsigned long*	/* nitems_return */,
//        unsigned long*	/* bytes_after_return */,
//        unsigned char**	/* prop_return */
//    )

    return XGetWindowProperty(QX11Info::display(), window, atom, 0, 4096, false,
                              reqType, &type, &format, resultLen, &rest,
                              result)  == Success;

}


Atom XDesktop::atom(const char* atomName)
{
    static QHash<QString, Atom> hash;

    if (hash.contains(atomName))
        return hash.value(atomName);

    Atom atom = XInternAtom(QX11Info::display(), atomName, false);
    hash[atomName] = atom;
    return atom;
}
//رسالة الى خادزم اكس
