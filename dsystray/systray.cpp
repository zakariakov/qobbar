
/********************************************************************
 *(c)GPL3+
  Inspired by freedesktops tint2
  original code  http://razor-qt.org
  modified by abou zakaria
*********************************************************************/
//

#include "systray.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrender.h>
#include <X11/extensions/Xdamage.h>
#include <xcb/xcb.h>
#include <xcb/damage.h>
#include "utils/x11utills.h"
#include <X11/X.h>

#define SYSTEM_TRAY_REQUEST_DOCK    0
#define SYSTEM_TRAY_BEGIN_MESSAGE   1
#define SYSTEM_TRAY_CANCEL_MESSAGE  2

#define XEMBED_EMBEDDED_NOTIFY  0
#define XEMBED_MAPPED          (1 << 0)

SysTray::SysTray( QWidget *parent):
    QWidget(parent),
    mValid(false),
    mTrayId(0),
    mDamageEvent(0),
    mDamageError(0),
    mIconSize(TRAY_ICON_SIZE_DEFAULT, TRAY_ICON_SIZE_DEFAULT),
    mDisplay(QX11Info::display())
{

     QFontMetrics fm(parent->font());
     int size=fm.height();
     mIconSize=QSize(size-3,size-3);
//   font.setPointSize(parent->font().pointSize());
//   setFont(font);
   setContentsMargins(0,0,0,0);


    mLayout = new QHBoxLayout(this);
    mLayout->setContentsMargins(3,0,3,0);
    mLayout->setSpacing(3);
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);

    _NET_SYSTEM_TRAY_OPCODE = X11UTILLS::atom("_NET_SYSTEM_TRAY_OPCODE");
   qApp->installNativeEventFilter(this);
    // Init the selection later just to ensure that no signals are sent until
    // after construction is done and the creating object has a chance to connect.
    QTimer::singleShot(0, this, SLOT(startTray()));

 // setStyleSheet("background-color: rgb(255, 170, 255);");
}


/************************************************

 ************************************************/
SysTray::~SysTray()
{
    stopTray();
}


/************************************************

 ************************************************/
bool SysTray::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{


    if (eventType != "xcb_generic_event_t")
        return false;

    xcb_generic_event_t* event = static_cast<xcb_generic_event_t *>(message);

    TrayIcon* icon;
    int event_type = event->response_type & ~0x80;

    switch (event_type)
    {
        case ClientMessage:
            clientMessageEvent(event);
            break;

        case DestroyNotify: {
            unsigned long event_window;
            event_window = reinterpret_cast<xcb_destroy_notify_event_t*>(event)->window;
            icon = findIcon(event_window);
            if (icon)
            {
                icon->windowDestroyed(event_window);
                mIcons.removeAll(icon);
                delete icon;
            }
            break;
        }
        default:
            if (event_type == mDamageEvent + XDamageNotify)
            {
                xcb_damage_notify_event_t* dmg = reinterpret_cast<xcb_damage_notify_event_t*>(event);
                icon = findIcon(dmg->drawable);
                if (icon)
                    icon->update();
            }
            break;
    }

    return false;
}




/************************************************

 ************************************************/
void SysTray::clientMessageEvent(xcb_generic_event_t *e)
{
    unsigned long opcode;
    unsigned long message_type;
    Window id;
    xcb_client_message_event_t* event = reinterpret_cast<xcb_client_message_event_t*>(e);
    uint32_t* data32 = event->data.data32;
    message_type = event->type;
    opcode = data32[1];
    if(message_type != _NET_SYSTEM_TRAY_OPCODE)
        return;

    switch (opcode)
    {
        case SYSTEM_TRAY_REQUEST_DOCK:
            id = data32[2];
            if (id)
                addIcon(id);
            break;


        case SYSTEM_TRAY_BEGIN_MESSAGE:
        case SYSTEM_TRAY_CANCEL_MESSAGE:
            qDebug()<<"   [-]"<<__FILE__<< __LINE__ << "we don't show balloon messages.";
            break;


        default:
//            if (opcode == xfitMan().atom("_NET_SYSTEM_TRAY_MESSAGE_DATA"))
//                qDebug() << "message from dockapp:" << e->data.b;
//            else
//                qDebug() << "SYSTEM_TRAY : unknown message type" << opcode;
            break;
    }
}

/************************************************

 ************************************************/
TrayIcon* SysTray::findIcon(Window id)
{
    foreach(TrayIcon* icon, mIcons)
    {
        if (icon->iconId() == id || icon->windowId() == id)
            return icon;
    }
    return 0;
}


/************************************************

************************************************/
void SysTray::setIconSize(QSize iconSize)
{

    mIconSize = iconSize;
    unsigned long size = qMin(mIconSize.width(), mIconSize.height());
    XChangeProperty(mDisplay,
                    mTrayId,
                    X11UTILLS::atom("_NET_SYSTEM_TRAY_ICON_SIZE"),
                    XA_CARDINAL,
                    32,
                    PropModeReplace,
                    (unsigned char*)&size,
                    1);
}


/************************************************

************************************************/
VisualID SysTray::getVisual()
{
    VisualID visualId = 0;
    Display* dsp = mDisplay;

    XVisualInfo templ;
    templ.screen=QX11Info::appScreen();
    templ.depth=32;
    templ.c_class=TrueColor;

    int nvi;
    XVisualInfo* xvi = XGetVisualInfo(dsp, VisualScreenMask|VisualDepthMask|VisualClassMask, &templ, &nvi);

    if (xvi)
    {
        int i;
        XRenderPictFormat* format;
        for (i = 0; i < nvi; i++)
        {
//            format = XRenderFindVisualFormat(dsp, xvi[i].visual);
//            if (format &&
//                format->type == PictTypeDirect &&
//                format->direct.alphaMask)
//            {
                visualId = xvi[i].visualid;
                break;
//            }
        }
        XFree(xvi);
    }

    return visualId;
}


/************************************************
   freedesktop systray specification
 ************************************************/
void SysTray::startTray()
{
    Display* dsp = mDisplay;
    Window root = QX11Info::appRootWindow();

    QString s = QString("_NET_SYSTEM_TRAY_S%1").arg(DefaultScreen(dsp));
    Atom _NET_SYSTEM_TRAY_S = X11UTILLS::atom(s.toLatin1());

    if (XGetSelectionOwner(dsp, _NET_SYSTEM_TRAY_S) != None)
    {
       qDebug()<<"   [-]"<<__FILE__<< __LINE__<< " Another systray is running";
        mValid = false;
        return;
    }

    // init systray protocol
    mTrayId = XCreateSimpleWindow(dsp, root, -1, -1, 1, 1, 0, 0, 0);

    XSetSelectionOwner(dsp, _NET_SYSTEM_TRAY_S, mTrayId, CurrentTime);
    if (XGetSelectionOwner(dsp, _NET_SYSTEM_TRAY_S) != mTrayId)
    {
        qDebug()<<"   [-]"<<__FILE__<< __LINE__ << " Can't get systray manager";
        stopTray();
        mValid = false;
        return;
    }

    int orientation = 0;
    XChangeProperty(dsp,
                    mTrayId,
                    X11UTILLS::atom("_NET_SYSTEM_TRAY_ORIENTATION"),
                    XA_CARDINAL,
                    32,
                    PropModeReplace,
                    (unsigned char *) &orientation,
                    1);

    // ** Visual ********************************
    VisualID visualId = getVisual();
    if (visualId)
    {
        XChangeProperty(mDisplay,
                        mTrayId,
                        X11UTILLS::atom("_NET_SYSTEM_TRAY_VISUAL"),
                        XA_VISUALID,
                        32,
                        PropModeReplace,
                        (unsigned char*)&visualId,
                        1);
    }
    // ******************************************

    setIconSize(mIconSize);

    XClientMessageEvent ev;
    ev.type = ClientMessage;
    ev.window = root;
    ev.message_type = X11UTILLS::atom("MANAGER");
    ev.format = 32;
    ev.data.l[0] = CurrentTime;
    ev.data.l[1] = _NET_SYSTEM_TRAY_S;
    ev.data.l[2] = mTrayId;
    ev.data.l[3] = 0;
    ev.data.l[4] = 0;
    XSendEvent(dsp, root, False, StructureNotifyMask, (XEvent*)&ev);

    XDamageQueryExtension(mDisplay, &mDamageEvent, &mDamageError);

    qDebug()<<"   [-]"<<__FILE__<< __LINE__ << "Systray started";
    mValid = true;



}


/************************************************

 ************************************************/
void SysTray::stopTray()
{
    for (auto & icon : mIcons)
        disconnect(icon, &QObject::destroyed, this, &SysTray::onIconDestroyed);
    qDeleteAll(mIcons);
    if (mTrayId)
    {
        XDestroyWindow(mDisplay, mTrayId);
        mTrayId = 0;
    }
    mValid = false;
}


/************************************************

 ************************************************/
void SysTray::onIconDestroyed(QObject * icon)
{
    //in the time QOjbect::destroyed is emitted, the child destructor
    //is already finished, so the qobject_cast to child will return nullptr in all cases
    mIcons.removeAll(static_cast<TrayIcon *>(icon));
}

/************************************************

 ************************************************/
void SysTray::addIcon(Window winId)
{
    TrayIcon* icon = new TrayIcon(winId, mIconSize, this);
    mIcons.append(icon);
    mLayout->addWidget(icon);
    connect(icon, &QObject::destroyed, this, &SysTray::onIconDestroyed);
}
