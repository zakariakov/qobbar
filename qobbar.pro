#-------------------------------------------------
#
# Project created by QtCreator 2017-03-04T09:15:20
#
#-------------------------------------------------

QT       += core gui  x11extras concurrent  dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

LIBS += $(SUBLIBS)  -L/usr/lib -lX11  -lXcomposite -lXdamage

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET = qobbar
DESTDIR = usr/bin
CONFIG += qt \
          release
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
INCLUDEPATH +=build

SOURCES += main.cpp\
        panelwidget.cpp \
        panel_adaptor.cpp \
    status/conkystatu.cpp \
        status/statuslabel.cpp \
        etaskbar/dtaskbarwidget.cpp \
        etaskbar/dactiontaskbar.cpp \
    utils/defines.cpp \
        utils/x11utills.cpp \
#        utils/edir.cpp \
#       utils/desktopfile.cpp \
        dsystray/systray.cpp \
        dsystray/trayicon.cpp \
        utils/xdesktoputils.cpp \
        epager/pager.cpp \
#        windowdock.cpp \
        utils/stylecolors.cpp \
        utils/setting.cpp

HEADERS  += panelwidget.h \
            panel_adaptor.h \
    status/conkystatu.h \
            status/statuslabel.h \
#           utils/desktopfile.h \
    utils/defines.h \
            utils/x11utills.h \
#            utils/edir.h \
#            utils/mystyle.h \
            utils/xdesktoputils.h \
            etaskbar/dactiontaskbar.h \
            etaskbar/dtaskbarwidget.h \
            dsystray/systray.h \
            dsystray/trayicon.h \
            epager/pager.h \
#            windowdock.h \
            utils/stylecolors.h \
            utils/setting.h


FORMS    += panelwidget.ui \


DISTFILES += \
    etc/xdg/qobbar/qobbar.conf \
    etc/xdg/qobbar/blocks.sh \
    README.md
#---------------------------------------------
#                 TRANSLATIONS
#---------------------------------------------
#---------------------------------------------
#                   INSTALL
#---------------------------------------------
 MKDIR = mkdir -p /etc/xdg

 target.path = /usr/bin

          xdgConf.files=etc/xdg/qobbar/*
          xdgConf.path=/etc/xdg/qobbar/

 INSTALLS +=target \
            xdgConf
