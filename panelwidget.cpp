#include "panel_adaptor.h"
#include "ui_panelwidget.h"
#include "panelwidget.h"
#include "utils/setting.h"
#include "utils/stylecolors.h"
#include "utils/defines.h"
#include <QtPlatformHeaders/QXcbWindowFunctions>
#include <QDesktopWidget>
PanelWidget::PanelWidget(bool bypassWm, /*bool debug,*/ QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PanelWidget)/*,mdebug(debug)*/,
    mSysTray(nullptr),mPager(nullptr),mConky(nullptr),mTaskbar(nullptr),mWindow(nullptr)
{


    QByteArray sS=qgetenv("DESKTOP_SESSION");

    qDebug()<<"#####################################################";
    qDebug()<<"                Qobbar V: "<<QApplication::applicationVersion();
    qDebug()<<"#####################################################";

    qDebug()<<"\033[36m[+] PanelWidget"<< __LINE__<<"Screens\033[0m"<<QApplication::screens().count();

    qDebug()<<"\033[36m[+] PanelWidget"<< __LINE__<<"  Env=\033[0m"<<sS;

    //    if(sS=="i3")
    setWindowFlags( Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus );
if(bypassWm)
     setWindowFlags( Qt::BypassWindowManagerHint);

    //TODO FIX This
    setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    //  this->winId(); //have to create the QWindow
    // tlwWindow = this->windowHandle();
    //    QXcbWindowFunctions::setWmWindowType(tlwWindow, QXcbWindowFunctions::Dock);
    //    setDock ();

    //setAttribute(Qt::WA_AlwaysShowToolTips);
    setAttribute(Qt::WA_X11DoNotAcceptFocus);

    setAttribute(Qt::WA_TranslucentBackground,true);

    ui->setupUi(this);

    m_timer=new QTimer;
    // mSetting=new Setting();
    connect(m_timer,SIGNAL(	timeout()),this,SLOT(compositorChanged()));
    m_timer->start(5000);
    if(Defines::degug())   qDebug()<<"\033[36m  [+] PanelWidget"<<__LINE__<<"setting.fileName:\033[0m"<<Setting::instance()->fileName();

    mFileSystemWatcher=new QFileSystemWatcher(this);
    mFileSystemWatcher->addPath(Setting::instance()->fileName());
    connect(mFileSystemWatcher,SIGNAL(fileChanged(QString)),this,SLOT(reconfigure()));

    //-----------------------------------------------------------------
    connect(QApplication::desktop(),SIGNAL(workAreaResized(int)),this,SLOT(resizePanel()));
    connect(QApplication::desktop(),SIGNAL(resized(int)),this,SLOT(resizePanel()));

    moveToAllDesktop();

    loadIconThems();
    //resizePanel();
    //reconfigure();
    QTimer::singleShot(20,this,SLOT(reconfigure()));


}

//-----------------------------------------------------------------------------------
void PanelWidget::setNativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    if(mPager)   mPager->setNativeEventFilter(eventType,message,result);
    if(mTaskbar) mTaskbar->setNativeEventFilter(eventType,message,result);
    if(mSysTray) mSysTray->setNativeEventFilter(eventType,message,result);
    if(mWindow)  mWindow->setNativeEventFilter(eventType,message,result);
}


//-----------------------------------------------------------------------------------
QRect PanelWidget::desktopRect()
{
    QRect desktop;

    if(m_Screen<0 || m_Screen>QApplication::screens().count()-1){
        m_Screen=0;
        desktop=QApplication::primaryScreen()->geometry();
    }else{
        desktop=QApplication::screens().at(m_Screen)->geometry();
    }

    return desktop;
}
//-----------------------------------------------------------------------------------
PanelWidget::~PanelWidget()
{
    delete ui;
}

void PanelWidget::showHide()
{

    if(isHidden()){
        show();
        moveToAllDesktop();
        resizePanel();
    }

    else{
        hide();
        setStrut(0, 0,
                 0, 0,
                 0, 0
                 );
    }

}
/*****************************************************************************************************
 *                    TODO FIX ANIMATION SHOW HID
 ****************************************************************************************************
//void PanelWidget::moveToShow()
//{
//    if(m_topPos){
//        this->move(this->pos().x(),this->pos().y()+5);
//        if(this->pos().y()>=0) {
//            this->move(this->pos().x(),0);
//            m_timer->stop();

//            moveToAllDesktop();
//            resizePanel();
//        }
//    }else{
//        this->move(this->pos().x(),this->pos().y()-5);
//        //Todo fix me
//        if(this->pos().y()>=0) {
//            this->move(this->pos().x(),0);
//        }
//    }

//}

//void PanelWidget::moveToHide()
//{
//    if(m_topPos){
//        this->move(this->pos().x(),this->pos().y()-5);
//        if(this->pos().y()<=-m_height) {
//            this->move(this->pos().x(),-m_height);
//            m_timer->stop();
//            hide();
//        }
//    }else{
//        this->move(this->pos().x(),this->pos().y()+5);
//        //Todo fix me
//        if(this->pos().y()>=0) {
//            this->move(this->pos().x(),0);
//        }
//    }
//}
*/
//-------------------------------------------------------------------------------
void PanelWidget:: loadSettings(bool charge)
{

    if(Defines::degug())  qDebug()<<"\033[36m [+]"<<"PanelWidget"<< __LINE__<<"loadSettings():beginGroup(Panel)\033[0m";

    Setting::instance()->beginGroup("Panel");

    QString     bgColor         =Setting::background(this->palette().window().color().name());
    QString     fgColor         =Setting::foreground(this->palette().windowText().color().name());
    QString     underline       =Setting::underline();
    QString     overline        =Setting::overline();
    QString     borderColor     =Setting::borderColor();
    QString     fontName        =Setting::fontName(qApp->font().family());
    int         fontSize        =Setting::fontSize(qApp->font().pointSize());
    bool        fontBold        =Setting::fontBold(qApp->font().bold());
    int         spacing         =Setting::spacing();
    int         barLeftSpacing  =Setting::barLeftSpacing();
    int         barRightSpacing =Setting::barRightSpacing();
    int         barCenterSpacing=Setting::barCenterSpacing();
    int         alpha           =Setting::alpha();
    QStringList listLeft        =Setting::barLeft();
    QStringList listCenter      =Setting::barCenter();
    QStringList listRight       =Setting::barRight();
    int         _left           =Setting::paddingLeft();
    int         _top            =Setting::paddingTop();
    int         _rigt           =Setting::paddingRight();
    int         _bot            =Setting::paddingBottom();
    int         m_left          =Setting::marginLeft();
    int         m_top           =Setting::marginTop();
    int         m_rigt          =Setting::marginRight();
    int         m_bot           =Setting::meginBottom();
    m_Border                    =Setting::border();
    m_topPos                    =Setting::top();
    bool m_showSystry           =Setting::showSystry();
    int  m_radius               =Setting::radius();
    m_Screen                    =Setting::screen();
    m_height                    =Setting::panelHeight();
    int     leftTopRadius      =Setting::leftTopRadius();
    int     rightTopRadius     =Setting::rightTopRadius();
    int     leftBotRadius      =Setting::leftBottomRadius();
    int     rightBotRadius     =Setting::rightBottomRadius();

    m_PaddingRect = QRect(QPoint(_left,_top),QPoint(_rigt,_bot));

    m_MarginRect = QRect(QPoint(m_left,m_top),QPoint(m_rigt,m_bot));

    Setting::instance()->endGroup();

StyleColors::instance()->xrdbquey();

    if(Defines::degug())  qDebug()<<"\033[36m [+]"<<"PanelWidget"<< __LINE__<<"loadSettings():endGroup(Panel)\033[0m";
    //if(Defines::degug())  qDebug()<<"\033[36m [+]"<<"PanelWidget"<< __LINE__<<"loadSettings():endGroup(Panel):m_showSystry\033[0m"<<m_showSystry;

    // INIT ______________________________________________________
    QFont font;
    font.setFamily(fontName);
    font.setBold(fontBold);
    font.setPointSize(fontSize);

    setFont(font);

    // ui->horizontalLayout->setMargin(m_Border);
    ui->horizontalLayout_ceter->setSpacing(barCenterSpacing);
    ui->horizontalLayout_right->setSpacing(barRightSpacing);
    ui->horizontalLayout_left->setSpacing(barLeftSpacing);
   // ui->horizontalLayout->setContentsMargins(m_left+(m_radius/2),m_top+m_Border,m_rigt+(m_radius/2),m_bot+m_Border);
    ui->horizontalLayout->setContentsMargins(m_left,m_top+m_Border,m_rigt,m_bot+m_Border);

    ui->horizontalLayout->setSpacing(spacing);
    // StyleSheet ________________________________________________
    QPalette pal=this->palette();

    if(bgColor.startsWith("xrdb") || bgColor.startsWith("$"))
        bgColor=StyleColors::getColors(bgColor);

    if(fgColor.startsWith("xrdb") || fgColor.startsWith("$"))
        fgColor=StyleColors::getColors(fgColor);


    QColor clbg(bgColor);
    m_isCoposite=QX11Info::isCompositingManagerRunning();
    if(!m_isCoposite){
        alpha=255;
        m_radius=0;
        leftTopRadius=0;
        rightTopRadius=0;
        leftBotRadius=0;
        rightBotRadius=0;
_left=0;
_top=0;
_rigt=0;
_bot=0;
    }

    clbg.setAlpha(alpha);

    if(clbg.isValid())
        pal.setColor(QPalette::Window,clbg);

    if(QColor(fgColor).isValid())
        pal.setColor(QPalette::WindowText,QColor(fgColor));

    setPalette(pal);

    int RadiusSize=m_radius;
    RadiusSize=qMax(RadiusSize,leftTopRadius);
    RadiusSize=qMax(RadiusSize,rightTopRadius);
    RadiusSize=qMax(RadiusSize,leftBotRadius);
    RadiusSize=qMax(RadiusSize,rightBotRadius);

    if(RadiusSize>0)
     ui->widgetBg->setContentsMargins((RadiusSize/2)+1,0,(RadiusSize/2)+1,0);

    setContentsMargins(_left,_top,_rigt,_bot);
    QString mystyle=StyleColors::style(bgColor,
                                       fgColor,
                                       underline,
                                       overline,
                                       m_Border,
                                       alpha,
                                       borderColor,
                                       m_radius,
                                       leftTopRadius,
                                       rightTopRadius,
                                       leftBotRadius,
                                       rightBotRadius);

    ui->widgetBg-> setStyleSheet("QWidget#widgetBg{"+mystyle+"}");

    qDebug()<<"\033[36m[-] PanelWidget :------------------------------ "<< __LINE__;
    qDebug()<<"[+] PanelWidget : LeftList: "<<listLeft;
    qDebug()<<"[+] PanelWidget : CenterList: "<<listCenter;
    qDebug()<<"[+] PanelWidget : RightList: "<<listRight;
    qDebug()<<"[+] PanelWidget : Systry :"<<m_showSystry;
    qDebug()<<"[+] PanelWidget : fontName: "<<fontName;
    qDebug()<<"[+] PanelWidget : fontSize: "<<fontSize<<"\033[0m";

    // charge status ---------------------------------------------
    if(charge){

        if(m_listLeft==listLeft && m_listRight==listRight && m_listCenter ==listCenter){
            if(mPager)   mPager->loadSettings();
            if(mTaskbar) mTaskbar->loadSettings();
            if(mConky)   mConky->loadSettings();
            if(mSysTray) mSysTray->loadSettings();
            if(mWindow) mWindow->loadSettings();
            foreach (StatusLabel *w, listStatus) {
                w->loadSettings();
            }
            qDebug()<<"[+] PanelWidget : loadsettings: ==============returned";
            return;
        }

        m_listLeft=listLeft;
        m_listRight=listRight;
        m_listCenter=listCenter;

        chargeStatus( listLeft, listCenter, listRight);

        if(m_showSystry){
            if(!mSysTray){
                mSysTray=new SysTray(/*mSetting,*/this);
                // listWidget.append(group);
                ui->horizontalLayout_tray->addWidget(mSysTray);
            }

            // addWidget(mSysTray, pos);
        }else{
            if(mSysTray){delete mSysTray;mSysTray=nullptr;}

        }

        moveToAllDesktop();
        resizePanel();
    }

}

 void PanelWidget::emitSignal(const QString &group)
 {
     if(listStatus.contains(group)){
         StatusLabel *statLab=listStatus.value(group);
         if(statLab){
             statLab->startRender();
         }
     }
 }

void PanelWidget::compositorChanged()
{
    if(m_isCoposite!=QX11Info::isCompositingManagerRunning())
        loadSettings(false);
}

//-----------------------------------------------------------------------------------
void PanelWidget::chargeStatus(QStringList listLeft,QStringList listCenter,QStringList listRight)
{

    QStringList list;
    list<<listLeft<<listCenter<<listRight;

    // Delete non-existing Status ----------------------------
    QMutableHashIterator<QString , StatusLabel*> i(listStatus);
    while (i.hasNext())
    {
        i.next();
        if(!list.contains(i.key())){
            qDebug()<<"\033[34m remove \033[0m"<<i.key();
            i.value()->cancelRender();
            // i.value()->close();
            delete i.value();
            i.remove();
        }
    }

    // Delete non-existing widgets ---------------------------
    foreach (QString e, listWidget) {
        if(!list.contains(e)){
            if     (e==MPAGER)  {if(mPager)  {listWidget.removeOne(MPAGER);  delete mPager;  mPager=nullptr;  }}
            else if(e==MCONKY)  {if(mConky)  {listWidget.removeOne(MCONKY);  delete mConky;  mConky=nullptr;  }}
            else if(e==MTASKBAR){if(mTaskbar){listWidget.removeOne(MTASKBAR);delete mTaskbar;mTaskbar=nullptr;}}
            else if(e==MTASKBAR){if(mWindow) {listWidget.removeOne(MWINDOW); delete mWindow; mWindow=nullptr; }}
        }
    }

    //CHARGE STATUS and WIDGETS -----------------------------
    if(listLeft.count()>0)   addStatus(listLeft,  LEFT);
    if(listCenter.count()>0) addStatus(listCenter,CENTER);
    if(listRight.count()>0)  addStatus(listRight, RIGHT);

}

//-----------------------------------------------------------------------------------
void PanelWidget::addStatus(QStringList list,int pos)
{
    QString posString;
    switch (pos) {
    case LEFT:   posString="LEFT";   break;
    case CENTER: posString="CENTER"; break;
    case RIGHT:  posString="RIGHT";  break;
    }

    foreach (QString group, list) {

        if(group.isEmpty())continue;

        if(Defines::degug())  qDebug()<<"\033[36m [+]"<<"PanelWidget"<< __LINE__<<" addWidget:\033[0m"<<group<<posString;

        //_PAGER -------------------
        if(group==MPAGER){
            if(!mPager){
                mPager=new Pager(/*mSetting,*/this/*,mdebug*/);
                listWidget.append(group);
            }
            addWidget(mPager, pos);
        }//_PAGER

        //_SYSTRAY ----------------
        else if(group==MCONKY){
            if(!mConky){
                mConky=new conkyStatu(/*mSetting,*/this);
                listWidget.append(group);
            }

            addWidget(mConky, pos);
        }//_SYSTRAY

        //_TASKBAR -----------------
        else if(group==MTASKBAR){
            if(!mTaskbar){
                mTaskbar=new  DtaskbarWidget(this);
                listWidget.append(group);
            }
            addWidget(mTaskbar, pos);
        }//_TASKBAR

        // Active Window --------------
        else if(group==MWINDOW){
            if(!mWindow){
                mWindow=new  ActiveWindow(this);
                listWidget.append(group);
            }
            addWidget(mWindow, pos);
        }//_Window

        // Status --------------
        else {

            QString groupName=group;
            if(group.contains(":")){groupName=group.section(":",0,0);}

            if(!Setting::instance()->childGroups().contains(groupName))continue;

            if(listStatus.contains(group)){
                StatusLabel *statLab=listStatus.value(group);
                if(statLab){
                    addWidget(statLab, pos);
                    statLab->loadSettings();
                }

            }else{

                StatusLabel *statLab=new StatusLabel(group,this);
                listStatus.insert(group,statLab);
                addWidget(statLab, pos);
            }

        }//_Status

    }

}

//-----------------------------------------------------------------------------------
void PanelWidget::addWidget(QWidget *w,int pos)
{

    switch (pos) {
    case LEFT:      ui->horizontalLayout_left->addWidget(w);  break;
    case CENTER:    ui->horizontalLayout_ceter->addWidget(w); break;
    case RIGHT:     ui->horizontalLayout_right->addWidget(w); break;
    default:        break;
    }

}

//-----------------------------------------------------------------------------------
void PanelWidget::reconfigure()
{
    if(Defines::degug()) {
        qDebug()<<"\033[36m [+]"<<"PanelWidget"<< __LINE__<<"reconfigure()\033[0m"<<listStatus.count();
        qDebug()<<"\033[36m [+]"<<"PanelWidget"<< __LINE__<<"reconfigure()\033[0m"<<listWidget.count();
    }

    Setting::instance()->sync();

    mFileSystemWatcher->blockSignals(true);

    loadSettings(true);

    moveToAllDesktop();

    resizePanel();

    mFileSystemWatcher->addPath(Setting::instance()->fileName());

    mFileSystemWatcher->blockSignals(false);

    if(Defines::degug()) {
        qDebug()<<"\033[36m [+]"<<"PanelWidget"<< __LINE__<<" reconfigure Setting FileName\033[0m"<<Setting::instance()->fileName();
        qDebug()<<"\033[36m [+]"<<"PanelWidget"<< __LINE__<<"reconfigure()\033[0m"<<listStatus.count();
        qDebug()<<"\033[36m [+]"<<"PanelWidget"<< __LINE__<<"reconfigure()\033[0m"<<listWidget.count();
    }

}

//------------------------------------------------------------------------------------------
int PanelWidget::calculatSize()
{


    QFontMetrics fm(font());
    int heightSize=fm.height()+(fm.leading()*2)+(m_Border*4);
    foreach (StatusLabel *w, listStatus) {
        heightSize=  qMax(heightSize,w->heightSize());
    }

    if(mConky){
        heightSize=  qMax(heightSize,mConky->heightSize());
    }

    heightSize=  qMax(heightSize,m_height);
    heightSize=  heightSize+m_MarginRect.top()+m_MarginRect.bottom();
    setMaximumHeight(heightSize);


  qDebug()<<"\033[36m[+] PanelWidget >> calculatSize : height :\033[0m"<<heightSize;
    return  heightSize ;

}

//------------------------------------------------------------------------------------------
void PanelWidget::resizePanel()
{
    if(Defines::degug())  qDebug()<<"\033[36m [+]"<<"PanelWidget"<< __LINE__<<"resizePanel()\033[0m";

    if(Defines::degug())  qDebug()<<"\033[36m [+]"<<"PanelWidget"<< __LINE__<<"Padding:\033[0m"<<m_PaddingRect.left()<< m_PaddingRect.top()<<m_PaddingRect.right()<<m_PaddingRect.bottom()<<m_MarginRect.top()<<m_MarginRect.bottom();

    int p_left=m_PaddingRect.left();
    int p_top=m_PaddingRect.top();
    int p_right=m_PaddingRect.right();
    int p_bot=m_PaddingRect.bottom();

    int panelHeight=calculatSize();

    QRect screen=desktopRect();
    //QRect screen=QApplication::primaryScreen()->geometry();
    if(Defines::degug())  qDebug()<<"\033[36m [+]"<<"PanelWidget"<< __LINE__<<"   screen  :\033[0m"<<screen;
    QRect rect;

    rect.moveLeft(screen.left());
    //  rect.moveTop(screen.top());
    // qDebug()<<"m_topPos"<<m_topPos;
    if (m_topPos==true){
        // this->setGeometry(screen.left()+leftRef,screen.top()+topRef,screen.width()-rightRef,m_panelHeight);
        rect = QRect(screen.left(),
                     screen.top(),
                     screen.width(),
                     panelHeight+(p_top+p_bot));

        rect.moveTop(screen.top());

        // qDebug()<<"m_topPos=top"<<rect;
    } else {

        //this->setGeometry(screen.left(),screen.bottom(),screen.width(),m_panelHeight);
        rect = QRect(screen.left(),
                     screen.bottom(),
                     screen.width(),
                     panelHeight+(p_top+p_bot));

        rect.moveBottom(screen.bottom());
        //  qDebug()<<"m_topPos=botum"<<rect;
    }

    this->setGeometry(rect.left()+p_left,
                      rect.top()+p_top,
                      rect.width()-(p_right+p_left),
                      panelHeight);

    this->move(rect.left()+p_left,rect.top()+p_top);

    if (m_topPos){
        setStrut(  rect.bottom(), 0,
                   rect.left(), rect.right(),
                   0, 0
                   );
    }else{
        setStrut(   0,rect.height() ,
                    0, 0,
                    rect.left(), rect.right()
                    );

    }

    if(Defines::degug())   qDebug()<<"\033[36m [+]"<<"PanelWidget"<< __LINE__<<"   widget panel resized: \033[0m"<<rect;
    setMaximumWidth(rect.width()-(p_left+p_right));
    setMinimumHeight(panelHeight);
    //  m_rootWindow = QX11Info::appRootWindow();

}


/*----------------------------------------------------------------------------------------
                              Begin       X11 LIBr
-----------------------------------------------------------------------------------------*/


//void PanelWidget::setDock ()
//{
//    if (winId() == 0)
//        return ;

//    if (!QX11Info::display())
//        return ;
//    //unsigned int wid=getWindowPID(winId());

//    Atom a[2] = { None, None };
//    a[0] = XInternAtom (QX11Info::display(), "_NET_WM_WINDOW_TYPE_DOCK", false);
//    Atom a2 = XInternAtom (QX11Info::display(), "_NET_WM_WINDOW_TYPE", false);

//    XChangeProperty (QX11Info::display(),
//                     /*tlwWindow->*/winId(),
//                     a2, XA_ATOM, 32, PropModeReplace, (unsigned char *) a, 1);

//    //TODO for XCB
//    {
//        //---------------------------------------------------------------------------TODO
//        //return;
//        //    xcb_connection_t *c = XGetXCBConnection( QX11Info::display());
//        //    xcb_ewmh_connection_t EWMH;
//        //    xcb_intern_atom_cookie_t *EWMHCookie = xcb_ewmh_init_atoms(c, &EWMH);
//        //    if (! xcb_ewmh_init_atoms_replies(&EWMH, EWMHCookie, NULL)) {
//        //        return ;
//        //    }
//        //    xcb_change_property(c,       /* Connection to the X server */
//        //                        XCB_PROP_MODE_REPLACE,     /* Property mode */
//        //                        wid,   /* Window */
//        //                        EWMH._NET_WM_WINDOW_TYPE, /* Property to change */
//        //                        XCB_ATOM_ATOM,     /* Type of the property */
//        //                        32,   /* Format of the property (8, 16, 32) */
//        //                        1, /* Length of the data parameter */
//        //                        &(EWMH._NET_WM_WINDOW_TYPE_DOCK));    /* Data */
//        //----------------------------------------------------------------------------
//    }

//}

//-----------------------------------------------------------------------------------
Atom XFatom(const char* atomName)
{
    static QHash<QString, Atom> hash;

    if (hash.contains(atomName))
        return hash.value(atomName);

    Atom atom = XInternAtom(QX11Info::display(), atomName, false);
    hash[atomName] = atom;
    return atom;

}

//-----------------------------------------------------------------------------------
void PanelWidget::setStrut(int top,  int bottom,
                           int topStartX,    int topEndX,
                           int bottomStartX, int bottomEndX )

{

    unsigned long desstrut[12];
    memset(desstrut,0,sizeof(desstrut));

    desstrut[0] = 0;
    desstrut[1] = 0;
    desstrut[2] = top;
    desstrut[3] = bottom;

    desstrut[4] = 0;
    desstrut[5] = 0;
    desstrut[6] = 0;
    desstrut[7] = 0;
    desstrut[8] = topStartX;
    desstrut[9] = topEndX;
    desstrut[10] = bottomStartX;
    desstrut[11] = bottomEndX;

    XChangeProperty(QX11Info::display(), this->winId() , XFatom("_NET_WM_STRUT_PARTIAL"),
                    XA_CARDINAL, 32, PropModeReplace,  (unsigned char *) desstrut, 12  );

    XChangeProperty(QX11Info::display(), this->winId(), XFatom("_NET_WM_STRUT"),
                    XA_CARDINAL, 32, PropModeReplace, (unsigned char*) desstrut, 4);


}

//-----------------------------------------------------------------------------------
void PanelWidget::moveToAllDesktop()
{
    unsigned long desktop = true ? 0xFFFFFFFF : 0;

    XChangeProperty(QX11Info::display(), /*tlwWindow->*/winId(), XFatom("_NET_WM_DESKTOP"), XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<const unsigned char*>(&desktop), 1);

    //TODO FIX THIS
    XClientMessageEvent msg;
    msg.window = /*tlwWindow->*/winId();
    msg.type = ClientMessage;
    msg.message_type = XFatom(/*"_NET_WM_WINDOW_TYPE_DOCK" */"_NET_WM_DESKTOP");
    msg.send_event = true;
    msg.display = QX11Info::display();
    msg.format = 32;
    msg.data.l[0] = -1;
    //TODO FIX This
    XSendEvent(QX11Info::display(), QX11Info::appRootWindow(m_Screen), 0, (SubstructureRedirectMask | SubstructureNotifyMask) , (XEvent *) &msg);

}

/*----------------------------------------------------------------
                      End      X11 LIB
----------------------------------------------------------------*/

/*****************************************************************************************************
 *
 *****************************************************************************************************/
void PanelWidget::loadIconThems()
{

    QString icnThem=QIcon::themeName();

    if(icnThem=="hicolor"||icnThem.isEmpty()){

        QStringList failback;
        failback << "Papirus"<<"Adwaita"
                 <<"oxygen"<< "Mint-X"
                << "Humanity"<<"Numix"
                << "elementary"
                <<"breeze"<< "gnome";

        QDir dir("/usr/share/icons/");
        foreach (QString s, failback)
        {
            if (dir.exists(s))
            {
                icnThem=s;

                break;
            }
        }

    }

    if(icnThem.isEmpty())
        icnThem="hicolor";

    if(Defines::degug())  qDebug()<<"\033[36m [+]"<<"PanelWidget"<< __LINE__<<"icon theme :\033[0m"<< icnThem;
    QIcon::setThemeName(icnThem);

}

