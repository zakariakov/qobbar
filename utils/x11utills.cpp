#include "x11utills.h"

#include <QApplication>
#include <QX11Info>
#include <QImage>
#include <QHash>

// Keep all the X11 stuff with scary defines below normal headers.
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

//#include <X11/extensions/Xcomposite.h>
//#include <X11/extensions/Xdamage.h>


/*
  _NET_WM_STATE actions
 */
Q_GLOBAL_STATIC(X11UTILLS, X11UTILLSInstance)

X11UTILLS *X11UTILLS::instance()
{
   return X11UTILLSInstance();
}
static XErrorHandler oldX11ErrorHandler = NULL;

static int x11errorHandler(Display* display, XErrorEvent* error)
{
	if(error->error_code == BadWindow)
		return 0; // This usually happens when querying property on a window that's already gone. That's OK.

	return (*oldX11ErrorHandler)(display, error);
}



X11UTILLS::X11UTILLS()
{

	oldX11ErrorHandler = XSetErrorHandler(x11errorHandler);
//	int damageErrorBase;
//	XDamageQueryExtension(QX11Info::display(), &m_damageEventBase, &damageErrorBase);
}

X11UTILLS::~X11UTILLS()
{
    //m_instance = NULL;
}


unsigned long X11UTILLS::atom(const QString& name)
{
    if(instance()->m_cachedAtoms.contains(name)){
     //   qDebug()<<"instence"<<name;
             return instance()->m_cachedAtoms.value(name);

    }
    Atom atom = XInternAtom(QX11Info::display(),  name.toLatin1().data(), false);
    instance()->m_cachedAtoms[name] = atom;
    return atom;

//  static QHash<QString, Atom> hash;

//    if (hash.contains(name))
//        return hash.value(name);

//    Atom atom = XInternAtom(QX11Info::display(),  name.toLatin1().data(), false);
//    hash[name] = atom;
//    return atom;
}

void X11UTILLS::removeWindowProperty(Window window, const QString& name)
{
	XDeleteProperty(QX11Info::display(), window, atom(name));
}

void X11UTILLS::setWindowPropertyCardinalArray(Window window, const QString& name, const QVector<unsigned long>& values)
{

    XChangeProperty(QX11Info::display(), window, atom(name),
                    XA_CARDINAL, 32,
                    PropModeReplace, reinterpret_cast<const unsigned char*>(values.data()), values.size());
}

WindowList X11UTILLS::getClientList()
{
    //initialize the parameters for the XGetWindowProperty call
    unsigned long length, *data;
    length=0;

    /**
     * @todo maybe support multiple desktops here!
     */
    QList<unsigned long> output;
  //  if (getWindowProperty(QX11Info::appRootWindow(), atom("_NET_CLIENT_LIST"), (Atom)AnyPropertyType, &length,  (unsigned char**) &data))

    if (getWindowProperty(QX11Info::appRootWindow(), atom("_NET_CLIENT_LIST"), (Atom)AnyPropertyType, &length,  (unsigned char**) &data))
   {
        for (unsigned int i = 0; i < length; i ++){
            if(isWindowForTaskbar(data[i]))
            output.append(data[i]);

        }
        XFree(data);
    }

    return output;
}

bool X11UTILLS::getWindowProperty(unsigned long window,
                       Atom atom,               // property
                       Atom reqType,            // req_type
                       unsigned long* resultLen, // nitems_return
                       unsigned char** result   // prop_return
                      )
{
    int  format;
    unsigned long type, rest;
    return XGetWindowProperty(QX11Info::display(), window, atom, 0, 4096, false,
                              reqType, &type, &format, resultLen, &rest,
                              result)  == Success;
}
bool X11UTILLS::getRootWindowProperty(Atom atom,    // property
                           Atom reqType,            // req_type
                           unsigned long* resultLen,// nitems_return
                           unsigned char** result   // prop_return
                          )
{
    return getWindowProperty(QX11Info::appRootWindow(), atom, reqType, resultLen, result);
}

bool X11UTILLS::isWindowForTaskbar(Window window)
{
    {
        AtomList types = getWindowType(window);
        AtomList ignoreList;
        ignoreList  << atom("_NET_WM_WINDOW_TYPE_DESKTOP")
                    << atom("_NET_WM_WINDOW_TYPE_DOCK")
                    << atom("_NET_WM_WINDOW_TYPE_SPLASH")
                    << atom("_NET_WM_WINDOW_TYPE_TOOLBAR")
                    << atom("_NET_WM_WINDOW_TYPE_MENU")
                    // for qlipper - using qpopup as a main window
                    << atom("_NET_WM_WINDOW_TYPE_POPUP_MENU");
        // issue #284: qmmp its not registered in window list panel
        // qmmp has _KDE_NET_WM_WINDOW_TYPE_OVERRIDE in its
        // _NET_WM_WINDOW_TYPE(ATOM) = _KDE_NET_WM_WINDOW_TYPE_OVERRIDE, _NET_WM_WINDOW_TYPE_NORMAL
        // Let's expect that _KDE_NET_WM_WINDOW_TYPE_OVERRIDE can be set for
        // regular windows too. If it should be hidden we should expect
        // one of atoms listed above.
//                    << atom("_KDE_NET_WM_WINDOW_TYPE_OVERRIDE");

        foreach (Atom i, ignoreList)
        {
            if (types.contains(i))
                return false;
        }
        unsigned long len;
        unsigned long *data;
        if (getWindowProperty(window, atom("_NET_WM_STATE"), XA_ATOM, &len, (unsigned char**) &data))
        {
            for (unsigned long i=0; i<len; ++i)
            {
                if (data[i] == atom("_NET_WM_STATE_SKIP_PAGER"))
                    return false;
            }

        }
//        WindowState state = getWindowState(window);
//        if (state.SkipTaskBar)  return false;
    }

    Window transFor = None;
    // WM_TRANSIENT_FOR hint not set - normal window
    if (!XGetTransientForHint(QX11Info::display(), window, &transFor))
        return true;

    if (transFor == 0)      return true;
    if (transFor == window) return true;
    if (transFor == QX11Info::appRootWindow())   return true;

     AtomList transForTypes = getWindowType(transFor);
     return !transForTypes.contains(atom("_NET_WM_WINDOW_TYPE_NORMAL"));
}

AtomList X11UTILLS::getWindowType(Window window)
{
    AtomList result;

    unsigned long length, *data;
    length=0;
    if (!getWindowProperty(window, atom("_NET_WM_WINDOW_TYPE"), (Atom)AnyPropertyType, &length, (unsigned char**) &data))
        return result;

    for (unsigned int i = 0; i < length; i++)
        result.append(data[i]);

    XFree(data);
    return result;
}

Window X11UTILLS::getActiveAppWindow()
{
    Window window = getActiveWindow();
    if (window == 0)
        return 0;

    if (isWindowForTaskbar(window))
        return window;

    Window transFor = None;
    if (XGetTransientForHint(QX11Info::display(), window, &transFor))
        return transFor;

    return 0;
}

Window X11UTILLS::getActiveWindow()
{
    unsigned long len;
    unsigned long *data;
    if (!getWindowProperty(QX11Info::appRootWindow(), atom("_NET_ACTIVE_WINDOW"), XA_WINDOW,
                          &len, (unsigned char**) &data)
       )
        return 0;

    Window result = 0;
    if (len)
        result = data[0];

    XFree(data);
    return result;
}
bool X11UTILLS::getClientIcon(Window _wid, QPixmap& _pixreturn)
{
    int format;
    ulong type, nitems, extra;
    ulong* data = 0;

    XGetWindowProperty(QX11Info::display(), _wid, atom("_NET_WM_ICON"),
                       0, LONG_MAX, False, AnyPropertyType,
                       &type, &format, &nitems, &extra,
                       (uchar**)&data);
    if (!data)
    {

        return false;
    }

    QImage img (data[0], data[1], QImage::Format_ARGB32);
    for (int i=0; i<img.byteCount()/4; ++i)
        ((uint*)img.bits())[i] = data[i+2];

    _pixreturn = QPixmap::fromImage(img);
    XFree(data);

    return true;
}


int X11UTILLS::sendWindowMessage(Window _wid, Atom _msg,
                            unsigned long data0,
                            unsigned long data1,
                            unsigned long data2,
                            unsigned long data3,
                            unsigned long data4)
{
    XClientMessageEvent msg;
    msg.window = _wid;
    msg.type = ClientMessage;
    msg.message_type = _msg;
    msg.send_event = true;
    msg.display = QX11Info::display();
    msg.format = 32;
    msg.data.l[0] = data0;
    msg.data.l[1] = data1;
    msg.data.l[2] = data2;
    msg.data.l[3] = data3;
    msg.data.l[4] = data4;
    if (XSendEvent(QX11Info::display(), QX11Info::appRootWindow(), 0, (SubstructureRedirectMask | SubstructureNotifyMask) , (XEvent *) &msg) == Success)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}
void X11UTILLS::moveWindowToDesktop(Window _wid, int _display)
{
    sendWindowMessage(_wid, atom("_NET_WM_DESKTOP"), (unsigned long) _display,0,0,0,0);
}
template<class T>
static bool getWindowPropertyHelper(unsigned long window, unsigned long atom,
                                    unsigned long type, int& numItems, T*& data)
{
    Atom retType;
    int retFormat;
    unsigned long numItemsTemp;
    unsigned long bytesLeft;
    if(XGetWindowProperty(QX11Info::display(), window, atom, 0, 0x7FFFFFFF, False, type, &retType, &retFormat, &numItemsTemp, &bytesLeft, reinterpret_cast<unsigned char**>(&data)) != Success)
        return false;
    numItems = numItemsTemp;
    if(numItems == 0)
        return false;
    return true;
}
int X11UTILLS::getNumDesktop()
{
    unsigned long length, *data;
    getRootWindowProperty(atom("_NET_NUMBER_OF_DESKTOPS"), XA_CARDINAL, &length, (unsigned char**) &data);
    if (data)
    {
        int res = data[0];
        XFree(data);
        return res;
    }
    return 0;
}
///------------------------------------------------------------------

QString X11UTILLS::getWindowTitleUTF8String(unsigned long window, const QString& name)
{
    int numItems;
    char* data;
    QString value;
    if(!getWindowPropertyHelper(window, atom(name), atom("UTF8_STRING"), numItems, data))
        return value;
    value = QString::fromUtf8(data);
    XFree(data);
    return value;
}

QString X11UTILLS::getWindowTitleLatin1String(unsigned long window, const QString& name)
{
    int numItems;
    char* data;
    QString value;
    if(!getWindowPropertyHelper(window, atom(name), XA_STRING, numItems, data))
        return value;
    value = QString::fromLatin1(data);
    XFree(data);
    return value;
}

QString X11UTILLS::getWindowTitle(unsigned long window)
{
    QString result = getWindowTitleUTF8String(window, "_NET_WM_VISIBLE_NAME");
    if(result.isEmpty())
        result = getWindowTitleUTF8String(window, "_NET_WM_NAME");
    if(result.isEmpty())
        result = getWindowTitleLatin1String(window, "WM_NAME");
    if(result.isEmpty())
        result = "<Unknown>";
    return result;
}

QIcon X11UTILLS::getWindowIcon(unsigned long window)
{
    int numItems;
    unsigned long* rawData;
    QIcon icon;
    if(!getWindowPropertyHelper(window, atom("_NET_WM_ICON"), XA_CARDINAL, numItems, rawData))
        return icon;
    unsigned long* data = rawData;
    while(numItems > 0)
    {
        int width = static_cast<int>(data[0]);
        int height = static_cast<int>(data[1]);
        data += 2;
        numItems -= 2;
        QImage image(width, height, QImage::Format_ARGB32);
        for(int i = 0; i < height; i++)
        {
            for(int k = 0; k < width; k++)
            {
                image.setPixel(k, i, static_cast<unsigned int>(data[i*width + k]));
            }
        }
        data += width*height;
        numItems -= width*height;
        icon.addPixmap(QPixmap::fromImage(image));
    }
    XFree(rawData);
    return icon;
}


QString X11UTILLS::getApplicationName(unsigned long _wid)
{
    XClassHint hint;
    QString ret;

    if (XGetClassHint(QX11Info::display(), _wid, &hint))
    {
        if (hint.res_name)
        {
        //    qDebug()<<"--------------------------------"<<hint.res_class;
            ret = hint.res_name;
            XFree(hint.res_name);
        }

    }

    return ret;
}
QString X11UTILLS::getApplicationClasseName(unsigned long _wid)
{
    XClassHint hint;
    QString ret;

    if (XGetClassHint(QX11Info::display(), _wid, &hint))
    {

        if (hint.res_class)
        {
          //   qDebug()<<"--------------------------------"<<hint.res_class;
            ret=(hint.res_class);
        }
    }

    return ret;
}
void X11UTILLS::setActiveDesktop(int _desktop)
{
    sendWindowMessage(QX11Info::appRootWindow(), atom("_NET_CURRENT_DESKTOP"), (unsigned long) _desktop,0,0,0,0);
}
int X11UTILLS::getActiveDesktop()
{
    int res = -2;
    unsigned long length, *data;

    if (getRootWindowProperty(atom("_NET_CURRENT_DESKTOP"), XA_CARDINAL, &length, (unsigned char**) &data))
    {
        if (data)
        {
            res = data[0];
            XFree(data);
        }
    }

    return res;
}
int X11UTILLS::getWindowDesktop(unsigned long _wid)
{
    int  res = -1;
    unsigned long length, *data;
    // so we try to use net_wm_desktop first, but if the
    // system does not use net_wm standard we use win_workspace!
    if (getWindowProperty(_wid, atom("_NET_WM_DESKTOP"), XA_CARDINAL, &length, (unsigned char**) &data))
    {
        if (!data)
            return res;
        res = data[0];
        XFree(data);
    }
    else
    {
        if (getWindowProperty(_wid, atom("_WIN_WORKSPACE"), XA_CARDINAL, &length, (unsigned char**) &data))
        {
            if (!data)
                return res;
            res = data[0];
            XFree(data);
        }
    }

    return res;
}
void X11UTILLS::resizeWindow(unsigned long _wid, int _width, int _height)
{
    XResizeWindow(QX11Info::display(), _wid, _width, _height);
}

///---------------------------------------------------
void X11UTILLS::raiseWindow(unsigned long _wid)
{
    sendWindowMessage(_wid, atom("_NET_ACTIVE_WINDOW"),
                  2 );
}


/************************************************

 ************************************************/
void X11UTILLS::minimizeWindow(unsigned long _wid)
{
    sendWindowMessage(_wid, atom("WM_CHANGE_STATE"),
                  IconicState);
}

/************************************************

 ************************************************/
void X11UTILLS::maximizeWindow(unsigned long _wid, int direction)
{

    Atom atom1 = 0, atom2= 0;
    switch (direction)
    {
        case 0:
            atom1 = atom("_NET_WM_STATE_MAXIMIZED_HORZ");
            break;

        case 1:
            atom1 = atom("_NET_WM_STATE_MAXIMIZED_VERT");
            break;

        case 2:
            atom1 = atom("_NET_WM_STATE_MAXIMIZED_VERT");
            atom2 = atom("_NET_WM_STATE_MAXIMIZED_HORZ");
            break;

    }

    sendWindowMessage(_wid, atom("_NET_WM_STATE"),
                  1,
                  atom1, atom2,
                  2);
    raiseWindow(_wid);
}


/************************************************

 ************************************************/
void X11UTILLS::deMaximizeWindow(unsigned long _wid)
{
    sendWindowMessage(_wid, atom("_NET_WM_STATE"),
                  0,
                  atom("_NET_WM_STATE_MAXIMIZED_VERT"),
                  atom("_NET_WM_STATE_MAXIMIZED_HORZ"),
                  2);
     raiseWindow(_wid);
}

/************************************************

 ************************************************/
void X11UTILLS::shadeWindow(unsigned long _wid, bool shade)
{
    sendWindowMessage(_wid, atom("_NET_WM_STATE"),
                  shade ? 1 : 0,
                  atom("_NET_WM_STATE_SHADED"),
                  0,
                  2);
}


/************************************************

 ************************************************/
void X11UTILLS::closeWindow(unsigned long _wid)
{
    sendWindowMessage(_wid, atom("_NET_CLOSE_WINDOW"),
                  0, // Timestamp
                  2);
}
void X11UTILLS::setWindowLayer(unsigned long _wid, int layer)
{

    ulong aboveAction = (layer == 0) ?
                1 : 0;

    ulong belowAction = (layer == 2) ?
                1 : 0;
//    qWarning()<<" aboveAction"<<aboveAction;
//    qWarning()<<" belowAction"<<belowAction;

    sendWindowMessage(_wid, atom("_NET_WM_STATE"),
                      aboveAction,
                      atom("_NET_WM_STATE_ABOVE"),
                      0,
                      2);

    sendWindowMessage(_wid, atom("_NET_WM_STATE"),
                      belowAction,
                      atom("_NET_WM_STATE_BELOW"),
                      0,
                      2);

}
//alowed


QHash<QString ,bool> X11UTILLS::states(unsigned long window)
{
QHash<QString ,bool>hash;
    unsigned long len;
    unsigned long *data;
    if (getWindowProperty(window, atom("_NET_WM_STATE"), XA_ATOM, &len, (unsigned char**) &data))
    {
        for (unsigned long i=0; i<len; ++i)
        {


            if (data[i] == atom("_NET_WM_STATE_MODAL"))             hash["Modal"] = true;             else
            if (data[i] == atom("_NET_WM_STATE_STICKY"))            hash["Sticky"] = true;            else
            if (data[i] == atom("_NET_WM_STATE_MAXIMIZED_VERT"))    hash["MaximizedVert"] = true;     else
            if (data[i] == atom("_NET_WM_STATE_MAXIMIZED_HORZ"))    hash["MaximizedHoriz"] = true;    else
            if (data[i] == atom("_NET_WM_STATE_SHADED"))            hash["Shaded"] = true;            else
            if (data[i] == atom("_NET_WM_STATE_SKIP_TASKBAR"))      hash["SkipTaskBar"] = true;       else
            if (data[i] == atom("_NET_WM_STATE_SKIP_PAGER"))        hash["SkipPager"] = true;         else
            if (data[i] == atom("_NET_WM_STATE_HIDDEN"))            hash["Hidden"] = true;            else
            if (data[i] == atom("_NET_WM_STATE_FULLSCREEN"))        hash["FullScreen"] = true;        else
            if (data[i] == atom("_NET_WM_STATE_ABOVE"))             hash["AboveLayer"] = true;        else
            if (data[i] == atom("_NET_WM_STATE_BELOW"))             hash["BelowLayer"] = true;        else
            if (data[i] == atom("_NET_WM_STATE_DEMANDS_ATTENTION")) hash["Attention"] = true;



        }
 XFree(data);
    }

    return hash;

}


QHash<QString ,bool>  X11UTILLS::allowed(unsigned long window)
{
  QHash<QString ,bool>hash;

   unsigned long len;
   unsigned long *data;
   if (getWindowProperty(window, atom("_NET_WM_ALLOWED_ACTIONS"), XA_ATOM, &len, (unsigned char**) &data))
   {
       for (unsigned long i=0; i<len; ++i)
       {
           if (data[i] == atom("_NET_WM_ACTION_MOVE"))             hash["Move"] = true;            else
           if (data[i] == atom("_NET_WM_ACTION_RESIZE"))           hash["Resize"] = true;          else
           if (data[i] == atom("_NET_WM_ACTION_MINIMIZE"))         hash["Minimize"] = true;        else
           if (data[i] == atom("_NET_WM_ACTION_SHADE"))            hash["Shade"] = true;           else
           if (data[i] == atom("_NET_WM_ACTION_STICK"))            hash["Stick"] = true;           else
           if (data[i] == atom("_NET_WM_ACTION_MAXIMIZE_HORZ"))    hash["MaximizeHoriz"] = true;   else
           if (data[i] == atom("_NET_WM_ACTION_MAXIMIZE_VERT"))    hash["MaximizeVert"] = true;    else
           if (data[i] == atom("_NET_WM_ACTION_FULLSCREEN"))       hash["FullScreen"] = true;      else
           if (data[i] == atom("_NET_WM_ACTION_CHANGE_DESKTOP"))   hash["ChangeDesktop"] = true;   else
           if (data[i] == atom("_NET_WM_ACTION_CLOSE"))            hash["Close"] = true;           else
           if (data[i] == atom("_NET_WM_ACTION_ABOVE"))            hash["AboveLayer"] = true;      else
           if (data[i] == atom("_NET_WM_ACTION_BELOW"))            hash["BelowLayer"] = true;
       }
       XFree(data);
   }

  return hash;
}
/************************************************
 The Window Manager MUST set this property on the root window to be the ID of a child
 window created by himself, to indicate that a compliant window manager is active.

 http://standards.freedesktop.org/wm-spec/wm-spec-latest.html#id2550836
 ************************************************/

bool X11UTILLS::isWindowManagerActive()
{
    //Window *wins;

    //getRootWindowProperty(atom("_NET_SUPPORTING_WM_CHECK"), XA_WINDOW, &length, (unsigned char**) &wins);

    Atom type;
    unsigned long length;
    Window *wins;
    int format;
    unsigned long after;

    XGetWindowProperty(QX11Info::display(), QX11Info::appRootWindow(), atom("_NET_SUPPORTING_WM_CHECK"),
                       0, LONG_MAX,
                       false, XA_WINDOW, &type, &format, &length,
                       &after, (unsigned char **)&wins);

    if ( type == XA_WINDOW && length > 0 && wins[0] != None )
    {
        XFree(wins);
        return true;
    }
    return false;
}

QRect X11UTILLS::getwindowGeometry(unsigned long window)

{
QRect rect;
    XSizeHints hints;
    int rx, ry;

 //   QHash<QString ,int>hash;
    Window junkwin;
    XWindowAttributes win_attributes;
    if (!XGetWindowAttributes(QX11Info::display(), window, &win_attributes)){
        //printf("Can't get window attributes.");
        //  vistemplate.visualid = XVisualIDFromVisual(win_attributes.visual);
        return rect;
    }

    (void) XTranslateCoordinates (QX11Info::display(), window, win_attributes.root,
                                  -win_attributes.border_width,
                                  -win_attributes.border_width,
                                  &rx, &ry, &junkwin);
//    hash["win_left"] =rx;;
//    hash["win_top"]=ry;
//    hash["win_height"]=(win_attributes.height);
//    hash["win_width"]=  (win_attributes.width)    ;
    rect=QRect(rx,ry,win_attributes.width,win_attributes.height);
    Window wmframe = window;
    while (True) {
        Window root, parent;
        Window *childlist;
        unsigned int ujunk;
        Status status;
        status = XQueryTree(QX11Info::display(), wmframe, &root, &parent, &childlist, &ujunk);
        if (parent == root || !parent || !status)
            break;
        wmframe = parent;
        if (status && childlist)
            XFree((char *)childlist);
    }

    if (wmframe != window) {
        /* WM reparented, so find edges of the frame */
        /* Only works for ICCCM-compliant WMs, and then only if the
         window has corner gravity.  We would need to know the original width
     of the window to correctly handle the other gravities. */

        XWindowAttributes frame_attr;

        if (!XGetWindowAttributes(QX11Info::display(), wmframe, &frame_attr))
            return rect;

   int  rw,rh;

        switch (hints.win_gravity) {
        case NorthWestGravity: case SouthWestGravity:
        case NorthEastGravity: case SouthEastGravity:
        case WestGravity:
            rx = frame_attr.x;
        }
        switch (hints.win_gravity) {
        case NorthWestGravity: case SouthWestGravity:
        case NorthEastGravity: case SouthEastGravity:
        case EastGravity:
            rw =frame_attr.width;
        }
        switch (hints.win_gravity) {
        case NorthWestGravity: case SouthWestGravity:
        case NorthEastGravity: case SouthEastGravity:
        case NorthGravity:
            ry = frame_attr.y;
        }
        switch (hints.win_gravity) {
        case NorthWestGravity:
        case SouthWestGravity:
        case NorthEastGravity:
        case SouthEastGravity:
        case SouthGravity:
            rh =frame_attr.height ;

        }
         rect=QRect(frame_attr.x,frame_attr.y,frame_attr.width,frame_attr.height);
//        hash["wm_left"] =frame_attr.x;;
//        hash["wm_top"]=frame_attr.y;
//        hash["wm_height"]=(frame_attr.height);
//        hash["wm_width"]=  (frame_attr.width)    ;
    }

    return rect;
}

void X11UTILLS::moveWindow(Window _win, int _x, int _y) const
{
    XMoveWindow(QX11Info::display(), _win, _x, _y);
}

Atom X11UTILLS::xatom(const char* atomName)
{
    static QHash<QString, Atom> hash;
    if (hash.contains(atomName))
        return hash.value(atomName);
    Atom atom = XInternAtom(QX11Info::display(), atomName, false);
    hash[atomName] = atom;
    return atom;
}
