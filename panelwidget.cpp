#include "panel_adaptor.h"
#include "ui_panelwidget.h"
#include "panelwidget.h"
#include "utils/setting.h"
#include "utils/stylecolors.h"
#include "utils/defines.h"
#include <QtPlatformHeaders/QXcbWindowFunctions>
#include <QDesktopWidget>
PanelWidget::PanelWidget(/*bool debug,*/ QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PanelWidget)/*,mdebug(debug)*/,
    mSysTray(nullptr),mPager(nullptr),mConky(nullptr),mTaskbar(nullptr),m_timer(nullptr)
{


    QByteArray sS=qgetenv("DESKTOP_SESSION");

 qDebug()<<"\033[36m #####################################################";
 qDebug()<<"	  ___        _     _					";
 qDebug()<<"	 / _ \  ___ | |__ | |__   __ _ _ __		";
 qDebug()<<"	| | | |/ _ \| '_ \| '_ \ / _` | '__|	";
 qDebug()<<"	| |_| | (_) | |_) | |_) | (_| | |		";
 qDebug()<<"	 \__\_\\___/|_.__/|_.__/ \__,_|_|		";
 qDebug()<<"\033[36m #####################################################";

     qDebug()<<"\033[36m[+] PanelWidget"<< __LINE__<<"Screens\033[0m"<<QApplication::screens().count();

     qDebug()<<"\033[36m[+] PanelWidget"<< __LINE__<<"  Env=\033[0m"<<sS;

    //    if(sS=="i3")
         setWindowFlags( Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    //    else
  //  setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint /*| Qt::WindowStaysOnTopHint*/);

    //TODO FIX This
    setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    //  this->winId(); //have to create the QWindow
    // tlwWindow = this->windowHandle();
    //    QXcbWindowFunctions::setWmWindowType(tlwWindow, QXcbWindowFunctions::Dock);
    //    setDock ();

    setAttribute(Qt::WA_AlwaysShowToolTips);
    setAttribute(Qt::WA_X11DoNotAcceptFocus);
    setWindowTitle(tr("ObBar"));
    // setObjectName("PanelWidget");
    //setPalette(Qt::transparent);
    setAttribute(Qt::WA_TranslucentBackground,true);

    ui->setupUi(this);

   // mSetting=new Setting();

    if(Defines::degug())   qDebug()<<"\033[36m  [+] PanelWidget"<<__LINE__<<"setting.fileName:\033[0m"<<Setting::instance()->fileName();

    mFileSystemWatcher=new QFileSystemWatcher(this);
    mFileSystemWatcher->addPath(Setting::instance()->fileName());
    connect(mFileSystemWatcher,SIGNAL(fileChanged(QString)),this,SLOT(reconfigure()));


 //loadSettings(true);

    //-----------------------------------------------------------------
    connect(QApplication::desktop(),SIGNAL(workAreaResized(int)),this,SLOT(resizePanel()));
    connect(QApplication::desktop(),SIGNAL(resized(int)),this,SLOT(resizePanel()));

    moveToAllDesktop();

loadIconThems();
//resizePanel();
//reconfigure();
QTimer::singleShot(20,this,SLOT(reconfigure()));


}

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
    if(!m_timer)  m_timer=new QTimer;


    if(isHidden()){
//        connect(m_timer,SIGNAL(	timeout()),this,SLOT(moveToShow()));
//        m_timer->start(1000);
        show();
        moveToAllDesktop();
        resizePanel();

    }else{
//        connect(m_timer,SIGNAL(	timeout()),this,SLOT(moveToHide()));
//        m_timer->start(1000);

        hide();

            setStrut(0, 0,
                       0, 0,
                       0, 0
                       );

    }
}
/*****************************************************************************************************
 *                    TODO FIX ANIMATION SHOW HID
 *****************************************************************************************************/
void PanelWidget::moveToShow()
{
    if(m_topPos){
        this->move(this->pos().x(),this->pos().y()+5);
        if(this->pos().y()>=0) {
           this->move(this->pos().x(),0);
            m_timer->stop();

            moveToAllDesktop();
            resizePanel();
        }
     }else{
         this->move(this->pos().x(),this->pos().y()-5);
        //Todo fix me
        if(this->pos().y()>=0) {
           this->move(this->pos().x(),0);
        }
    }

}

void PanelWidget::moveToHide()
{
    if(m_topPos){
        this->move(this->pos().x(),this->pos().y()-5);
        if(this->pos().y()<=-m_height) {
           this->move(this->pos().x(),-m_height);
             m_timer->stop();
             hide();
        }
     }else{
         this->move(this->pos().x(),this->pos().y()+5);
        //Todo fix me
        if(this->pos().y()>=0) {
           this->move(this->pos().x(),0);
        }
    }
}

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
    m_Border                     =Setting::border();
    m_topPos                    =Setting::top();
    bool m_showSystry           =Setting::showSystry();
    int  m_radius               =Setting::radius();
    m_Screen                    =Setting::screen();
    m_height                    =Setting::panelHeight();

    m_PaddingRect = QRect(QPoint(_left,_top),QPoint(_rigt,_bot));

    m_MarginRect = QRect(QPoint(m_left,m_top),QPoint(m_rigt,m_bot));

    Setting::instance()->endGroup();



    if(Defines::degug())  qDebug()<<"\033[36m [+]"<<"PanelWidget"<< __LINE__<<"loadSettings():endGroup(Panel)\033[0m";
    //if(Defines::degug())  qDebug()<<"\033[36m [+]"<<"PanelWidget"<< __LINE__<<"loadSettings():endGroup(Panel):m_showSystry\033[0m"<<m_showSystry;

    // INIT ______________________________________________________
    QFont font;
    font.setFamily(fontName);
    font.setBold(fontBold);
    font.setPointSize(fontSize);
    //    font.insertSubstitution("Font Awesome","Font Awesome");
    //    font.insertSubstitution("Ubuntu","Font Awesome");
    //    font.insertSubstitution("Sans","Font Awesome");
    //    qDebug()<<"[==]"<<"PanelWidget"<< __LINE__<<"font"<<font.substitutions();
    setFont(font);

    // ui->horizontalLayout->setMargin(m_Border);

    ui->horizontalLayout_ceter->setSpacing(barCenterSpacing);
    ui->horizontalLayout_right->setSpacing(barRightSpacing);
    ui->horizontalLayout_left->setSpacing(barLeftSpacing);
    ui->horizontalLayout->setContentsMargins(m_left+(m_radius/2),m_top+m_Border,m_rigt+(m_radius/2),m_bot+m_Border);
    ui->horizontalLayout->setSpacing(spacing);
    // StyleSheet ________________________________________________
    QPalette pal=this->palette();

    if(bgColor.startsWith("xrdb"))
        bgColor=StyleColors::loadXresourceColor(bgColor.section(".",1));

    if(fgColor.startsWith("xrdb"))
        fgColor=StyleColors::loadXresourceColor(fgColor.section(".",1));


    QColor clbg(bgColor);
    clbg.setAlpha(alpha);

    if(clbg.isValid())
        pal.setColor(QPalette::Window,clbg);



    if(QColor(fgColor).isValid())
        pal.setColor(QPalette::WindowText,QColor(fgColor));

    setPalette(pal);

    QString mystyle=StyleColors::style(bgColor,
                                       fgColor,
                                       underline,
                                       overline,
                                       m_Border,
                                       alpha,
                                       borderColor,
                                       m_radius);

    ui->widgetBg-> setStyleSheet("QWidget#widgetBg{"+mystyle+"}");

    qDebug()<<"\033[36m[-] PanelWidget :------------------------------ "<< __LINE__;
    qDebug()<<"[+] PanelWidget : LeftList: "<<listLeft;
    qDebug()<<"[+] PanelWidget : CenterList: "<<listCenter;
    qDebug()<<"[+] PanelWidget : RightList: "<<listRight;
    qDebug()<<"[+] PanelWidget : Systry :"<<m_showSystry;
    qDebug()<<"[+] PanelWidget : fontName: "<<fontName;
    qDebug()<<"[+] PanelWidget : fontSize: "<<fontSize<<"\033[0m";


    // charge status ---------------------------------------------
    if(charge)
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
    //    ui->widgetBg-> setAutoFillBackground(false);
    //    ui->widgetBg-> setWindowOpacity(alpha);

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
            if     (e==MPAGER)  {if(mPager)  {delete mPager; mPager=nullptr; listWidget.removeOne(MPAGER);}}
            else if(e==MCONKY)  {if(mConky){delete mConky;mConky=nullptr;listWidget.removeOne(MCONKY);}}
            else if(e==MTASKBAR){if(mTaskbar){delete mTaskbar;mTaskbar=nullptr;listWidget.removeOne(MTASKBAR);}}
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

        if(group==MPAGER){
            if(!mPager){
                mPager=new Pager(/*mSetting,*/this/*,mdebug*/);
                listWidget.append(group);
            }
            addWidget(mPager, pos);
        }//_PAGER

                else if(group==MCONKY){
                    if(!mConky){
                        mConky=new conkyStatu(/*mSetting,*/this);
                        listWidget.append(group);
                    }

                   addWidget(mConky, pos);
                }//_SYSTRAY

        else if(group==MTASKBAR){
            if(!mTaskbar){
                mTaskbar=new  DtaskbarWidget(/*mSetting,*/this/*,mdebug*/);
                listWidget.append(group);
            }
            addWidget(mTaskbar, pos);
        }//_TASKBAR

        else {

            QString groupName=group;
            if(group.contains(":")){
                groupName=group.section(":",0,0);
            }
            if(!Setting::instance()->childGroups().contains(groupName))continue;

            if(listStatus.contains(group)){
                StatusLabel *statLab=listStatus.value(group);
                if(statLab){
                    addWidget(statLab, pos);
                    statLab->loadSettings();
                }
            }else{

                StatusLabel *statLab=new StatusLabel(group,this/*,mSetting,this,mdebug*/);
                listStatus.insert(group,statLab);
                addWidget(statLab, pos);
            }

        }//Status

        // qApp->processEvents();
    }

}

//-----------------------------------------------------------------------------------
void PanelWidget::addWidget(QWidget *w,int pos)
{

    switch (pos) {
    case LEFT:
        ui->horizontalLayout_left->addWidget(w);
        break;
    case CENTER:
        ui->horizontalLayout_ceter->addWidget(w);
        break;
    case RIGHT:
        ui->horizontalLayout_right->addWidget(w);
        break;
    default:
        break;
    }

}

//-----------------------------------------------------------------------------------
void PanelWidget::reconfigure()
{
    if(Defines::degug()) qDebug()<<"\033[36m [+]"<<"PanelWidget"<< __LINE__<<"reconfigure()\033[0m"<<listStatus.count();
    if(Defines::degug()) qDebug()<<"\033[36m [+]"<<"PanelWidget"<< __LINE__<<"reconfigure()\033[0m"<<listWidget.count();

    Setting::instance()->sync();

    mFileSystemWatcher->blockSignals(true);

    loadSettings(true);

    if(mPager)   mPager->loadSettings();
    if(mTaskbar) mTaskbar->loadSettings();
    if(mConky)   mConky->loadSettings();
    if(mSysTray) mSysTray->loadSettings();

    foreach (StatusLabel *w, listStatus) {
        w->loadSettings();
    }


   moveToAllDesktop();
   resizePanel();


    mFileSystemWatcher->addPath(Setting::instance()->fileName());

    mFileSystemWatcher->blockSignals(false);

    if(Defines::degug())  qDebug()<<"\033[36m [+]"<<"PanelWidget"<< __LINE__<<" reconfigure Setting FileName\033[0m"<<Setting::instance()->fileName();
    if(Defines::degug())  qDebug()<<"\033[36m [+]"<<"PanelWidget"<< __LINE__<<"reconfigure()\033[0m"<<listStatus.count();
    if(Defines::degug())  qDebug()<<"\033[36m [+]"<<"PanelWidget"<< __LINE__<<"reconfigure()\033[0m"<<listWidget.count();

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
    heightSize=heightSize+m_MarginRect.top()+m_MarginRect.bottom();
    setMaximumHeight(heightSize);
    if(Defines::degug())  qDebug()<<"\033[36m [+]"<<"PanelWidget"<< __LINE__<<"calculatSize()"<<"MaximumHeight:\033[0m"<< heightSize+m_MarginRect.top()+m_MarginRect.bottom();
    qDebug()<<"\033[36m[+] PanelWidget : height :\033[0m"<<heightSize;
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

    // QFontMetrics fm(font());
    //  int size=fm.height();
    int panelHeight=calculatSize();
    //    if(mSysTray){
    //        mSysTray->setIconSize(QSize(panelHeight-4,panelHeight-4));
    //    }

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

    // QRect desktop=desktopRect();
    //QRect desktop =QApplication::primaryScreen()->geometry();
    if (m_topPos){
        setStrut(  rect.bottom(), 0,
                   rect.left(), rect.right(),
                   0, 0
                   );
    }else{
        setStrut(   0, rect.height() - rect.y(),
                    0, 0,
                    rect.left(), rect.right()
                    );

    }

    //    this->setGeometry(rect);
    //    this->resize(rect.width(),rect.height());

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

//    QByteArray sS=qgetenv("DESKTOP_SESSION");

//    if(sS=="elokab-session"){
//        QSettings setting("elokab","elokabsettings");
//        setting.beginGroup("Themes");
//        icnThem=  setting.value("iconsTheme",QIcon::themeName()).toString();
//        setting.endGroup();

//    }

    if(icnThem=="hicolor"||icnThem.isEmpty()){

        QStringList failback;
        failback << "Papirus"<<"Adwaita" <<"oxygen"<< "Mint-X"
                 << "Humanity"<<"Numix"
                 << "Prudence-icon"<< "elementary"
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

