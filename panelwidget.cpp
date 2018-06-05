#include "panel_adaptor.h"
#include "ui_panelwidget.h"
#include "panelwidget.h"
#include "utils/stylecolors.h"
#include <QSettings>
#include <QtPlatformHeaders/QXcbWindowFunctions>

PanelWidget::PanelWidget(bool debug, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PanelWidget),mdebug(debug),
    mSysTray(0),mPager(0),mTaskbar(0)
{

    QByteArray sS=qgetenv("DESKTOP_SESSION");

  if(mdebug)  qDebug()<<"[+]"<<__FILE__<< __LINE__<<"  Env="<<sS;

//    if(sS=="i3")
//        setWindowFlags( Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
//    else
        setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

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

    mSetting=new Setting();

  if(mdebug)   qDebug()<<"[+]"<<__FILE__<<__LINE__<<"setting.fileName:"<<mSetting->fileName();

    mFileSystemWatcher=new QFileSystemWatcher(this);
    mFileSystemWatcher->addPath(mSetting->fileName());
    connect(mFileSystemWatcher,SIGNAL(fileChanged(QString)),this,SLOT(reconfigure(QString)));

    loadIconThems();

    loadSettings(true);

    //-----------------------------------------------------------------
    connect(QApplication::desktop(),SIGNAL(workAreaResized(int)),this,SLOT(resizePanel()));

    moveToAllDesktop();

    resizePanel();

}

//-----------------------------------------------------------------------------------
PanelWidget::~PanelWidget()
{
    delete ui;
}

//-------------------------------------------------------------------------------
void PanelWidget::loadSettings(bool charge)
{

    if(mdebug)  qDebug()<<"[+]"<<__FILE__<< __LINE__<<"loadSettings():beginGroup(Panel)";

    mSetting->beginGroup("Panel");

    QString     bgColor         =mSetting->background(this->palette().window().color().name());
    QString     fgColor         =mSetting->foreground(this->palette().windowText().color().name());
    QString     underline       =mSetting->underline();
    QString     overline        =mSetting->overline();
    QString     borderColor     =mSetting->borderColor();
    QString     fontName        =mSetting->fontName(qApp->font().family());
    int         fontSize        =mSetting->fontSize(qApp->font().pointSize());
    bool        fontBold        =mSetting->fontBold(qApp->font().bold());
    int         barLeftSpacing  =mSetting->barLeftSpacing();
    int         barRightSpacing =mSetting->barRightSpacing();
    int         barCenterSpacing=mSetting->barCenterSpacing();
    int         alpha           =mSetting->alpha();
    QStringList listLeft        =mSetting->barLeft();
    QStringList listCenter      =mSetting->barCenter();
    QStringList listRight       =mSetting->barRight();
    int         _left           =mSetting->paddingLeft();
    int         _top            =mSetting->paddingTop();
    int         _rigt           =mSetting->paddingRight();
    int         _bot            =mSetting->paddingBottom();
    mBorder                     =mSetting->border();
    m_topPos                    =mSetting->top();

    mPaddingRect = QRect(QPoint(_left,_top),QPoint(_rigt,_bot));

    mSetting->endGroup();

    if(mdebug)  qDebug()<<"[+]"<<__FILE__<< __LINE__<<"loadSettings():endGroup(Panel)";

    // INIT ______________________________________________________
    QFont font;
    font.setFamily(fontName);
    font.setBold(fontBold);
    font.setPointSize(fontSize);
    //    font.insertSubstitution("Font Awesome","Font Awesome");
    //    font.insertSubstitution("Ubuntu","Font Awesome");
    //    font.insertSubstitution("Sans","Font Awesome");
    //    qDebug()<<"[==]"<<__FILE__<< __LINE__<<"font"<<font.substitutions();
    setFont(font);

    ui->horizontalLayout->setMargin(mBorder);
    ui->horizontalLayout_ceter->setSpacing(barCenterSpacing);
    ui->horizontalLayout_right->setSpacing(barRightSpacing);
    ui->horizontalLayout_left->setSpacing(barLeftSpacing);

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
                                       mBorder,
                                       alpha,
                                       borderColor);

    ui->widgetBg-> setStyleSheet("QWidget#widgetBg{"+mystyle+"}");
//qDebug()<<ui->widgetBg->styleSheet();
    // charge status ---------------------------------------------
    if(charge)
        chargeStatus( listLeft, listCenter, listRight);

    calculatSize();
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
            qDebug()<<"remove"<<i.key();
            i.value()->cancelRender();
            // i.value()->close();
            delete i.value();
            i.remove();
        }
    }

    // Delete non-existing widgets ---------------------------
    foreach (QString e, listWidget) {
        if(!list.contains(e)){
            if     (e==_PAGER)  {if(mPager)  {delete mPager; mPager=0; listWidget.removeOne(_PAGER);}}
            else if(e==_SYSTRAY){if(mSysTray){delete mSysTray;mSysTray=0;listWidget.removeOne(_SYSTRAY);}}
            else if(e==_TASKBAR){if(mTaskbar){delete mTaskbar;mTaskbar=0;listWidget.removeOne(_TASKBAR);}}
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
    case LEFT: posString="LEFT"; break;
    case CENTER: posString="CENTER"; break;
    case RIGHT: posString="RIGHT"; break;
    }

    foreach (QString group, list) {

       if(group.isEmpty())continue;

        if(mdebug)  qDebug()<<"[+]"<<__FILE__<< __LINE__<<" addWidget:"<<group<<posString;

        if(group==_PAGER){
            if(!mPager){
                mPager=new Pager(mSetting,this,mdebug);
                listWidget.append(group);
            }
            addWidget(mPager, pos);
        }//_PAGER

        else if(group==_SYSTRAY){
            if(!mSysTray){
                mSysTray=new SysTray(this);
                listWidget.append(group);
            }
            addWidget(mSysTray, pos);
        }//_SYSTRAY

        else if(group==_TASKBAR){
            if(!mTaskbar){
                mTaskbar=new  DtaskbarWidget(mSetting,this,mdebug);
                listWidget.append(group);
            }
            addWidget(mTaskbar, pos);
        }//_TASKBAR

        else {
            if(!mSetting->childGroups().contains(group))continue;

            if(listStatus.contains(group)){
                StatusLabel *statLab=listStatus.value(group);
                if(statLab){
                    addWidget(statLab, pos);
                    statLab->loadSettings();
                }
            }else{
                StatusLabel *statLab=new StatusLabel(group,mSetting,this,mdebug);
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
void PanelWidget::reconfigure(QString)
{
    /*if(mdebug)*/ qDebug()<<"[+]"<<__FILE__<< __LINE__<<"reconfigure()"<<listStatus.count();
     /*if(mdebug)*/qDebug()<<"[+]"<<__FILE__<< __LINE__<<"reconfigure()"<<listWidget.count();

    mSetting->sync();

    mFileSystemWatcher->blockSignals(true);

    loadSettings(true);

    if(mPager)
        mPager->loadSettings();

    if(mTaskbar)
        mTaskbar->loadSettings();

     foreach (StatusLabel *w, listStatus) {
       //  w->setPalette(this->palette());
        w->loadSettings();
     }

    calculatSize();

    resizePanel();

    mFileSystemWatcher->addPath(mSetting->fileName());

    mFileSystemWatcher->blockSignals(false);

   if(mdebug)  qDebug()<<"[+2]"<<__FILE__<< __LINE__<<" reconfigure"<<mSetting->fileName();
    /*if(mdebug)*/qDebug()<<"[+]"<<__FILE__<< __LINE__<<"reconfigure()"<<listStatus.count();
   /*if(mdebug)*/ qDebug()<<"[+]"<<__FILE__<< __LINE__<<"reconfigure()"<<listWidget.count();

}

//------------------------------------------------------------------------------------------
void PanelWidget::calculatSize()
{

    int border=0;
    QFontMetrics fm(font());
    int heightSize=fm.height();
    foreach (StatusLabel *w, listStatus) {
        QFontMetrics fm(w->font());
        qDebug()<<fm.height();
      heightSize=  qMax(heightSize,fm.height());
      border=  qMax(border,w->border());


//        int wSize=fm.height();
//        if (wSize>heightSize)
//            heightSize=wSize;

//        int wBorder=w->border();
//        if(wBorder>border)
//            border=wBorder;
    }
    setMaximumHeight(heightSize+(mBorder*2)+(border));
   /*if(mdebug)*/  qDebug()<<"[+]"<<__FILE__<< __LINE__<<"calculatSize:"<<"font height:"<< heightSize<<"border:"<<(mBorder)<<(border);

}

//------------------------------------------------------------------------------------------
void PanelWidget::resizePanel()
{
   if(mdebug)  qDebug()<<"[+]"<<__FILE__<< __LINE__<<"resizePanel()";

   if(mdebug)  qDebug()<<"[+]"<<__FILE__<< __LINE__<<"Padding:"<<mPaddingRect.left()<< mPaddingRect.top()<<mPaddingRect.right()<<mPaddingRect.bottom();

     int p_left=mPaddingRect.left();
     int p_top=mPaddingRect.top();
     int p_right=mPaddingRect.right();
     int p_bot=mPaddingRect.bottom();

    // QFontMetrics fm(font());
    //  int size=fm.height();
    int panelHeight=this->maximumHeight();
//    if(mSysTray){
//        mSysTray->setIconSize(QSize(panelHeight-4,panelHeight-4));
//    }

    QRect screen(QApplication::desktop()->screen(0)->geometry());
   if(mdebug)  qDebug()<<"[+]"<<__FILE__<< __LINE__<<"   screen  :"<<screen;
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

    int mScreenNum= QApplication::desktop()->primaryScreen();
    QRect desktop = QApplication::desktop()->screen(mScreenNum)->geometry();

    if (m_topPos==true){
        setStrut(  rect.bottom(), 0,
                   rect.left(), rect.right(),
                   0, 0
                   );
    }else{
        setStrut(   0, desktop.height() - rect.y(),
                    0, 0,
                    rect.left(), rect.right()
                    );

    }

//    this->setGeometry(rect);
//    this->resize(rect.width(),rect.height());

  if(mdebug)   qDebug()<<"[+]"<<__FILE__<< __LINE__<<"   widget panel resized: "<<rect;
    setMaximumWidth(rect.width()-(p_left+p_right));
    setMinimumHeight(panelHeight);
    //  m_rootWindow = QX11Info::appRootWindow();

}


/*----------------------------------------------------------------------------------------
                              Begin       X11 LIBr
-----------------------------------------------------------------------------------------*/
void PanelWidget::setDock ()
{
    if (winId() == 0)
        return ;

    if (!QX11Info::display())
        return ;
    //unsigned int wid=getWindowPID(winId());

    Atom a[2] = { None, None };
    a[0] = XInternAtom (QX11Info::display(), "_NET_WM_WINDOW_TYPE_DOCK", false);
    Atom a2 = XInternAtom (QX11Info::display(), "_NET_WM_WINDOW_TYPE", false);

    XChangeProperty (QX11Info::display(),
                     /*tlwWindow->*/winId(),
                     a2, XA_ATOM, 32, PropModeReplace, (unsigned char *) a, 1);

//TODO for XCB
  {
    //---------------------------------------------------------------------------TODO
    //return;
    //    xcb_connection_t *c = XGetXCBConnection( QX11Info::display());
    //    xcb_ewmh_connection_t EWMH;
    //    xcb_intern_atom_cookie_t *EWMHCookie = xcb_ewmh_init_atoms(c, &EWMH);
    //    if (! xcb_ewmh_init_atoms_replies(&EWMH, EWMHCookie, NULL)) {
    //        return ;
    //    }
    //    xcb_change_property(c,       /* Connection to the X server */
    //                        XCB_PROP_MODE_REPLACE,     /* Property mode */
    //                        wid,   /* Window */
    //                        EWMH._NET_WM_WINDOW_TYPE, /* Property to change */
    //                        XCB_ATOM_ATOM,     /* Type of the property */
    //                        32,   /* Format of the property (8, 16, 32) */
    //                        1, /* Length of the data parameter */
    //                        &(EWMH._NET_WM_WINDOW_TYPE_DOCK));    /* Data */
    //----------------------------------------------------------------------------
   }

}

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
    XSendEvent(QX11Info::display(), QX11Info::appRootWindow(0), 0, (SubstructureRedirectMask | SubstructureNotifyMask) , (XEvent *) &msg);

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

    QByteArray sS=qgetenv("DESKTOP_SESSION");

    if(sS=="elokab-session"){
        QSettings setting("elokab","elokabsettings");
        setting.beginGroup("Themes");
        icnThem=  setting.value("iconsTheme",QIcon::themeName()).toString();
        setting.endGroup();

    }

    if(icnThem=="hicolor"||icnThem.isEmpty()){

        QStringList failback;
        failback <<"Adwaita" <<"oxygen"<< "Mint-X"
                << "Humanity"<<"Numix"
                << "Prudence-icon"<< "elementary"
                << "Adwaita"<<"breeze"<< "gnome";

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

    if(mdebug)  qDebug()<<"[+]"<<__FILE__<< __LINE__<<"icon theme :"<< icnThem;
    QIcon::setThemeName(icnThem);


}
