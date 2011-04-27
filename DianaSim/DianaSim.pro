#-------------------------------------------------
#
# Project created by QtCreator 2011-04-20T06:16:31
#
#-------------------------------------------------

QT       += core gui network

TARGET = DianaSim
TEMPLATE = app

INCLUDEPATH += /data/proj/diana/NYVIS_DEV/Stefan.Fagerstrom/profet2/local/include

LIBS += -L/data/proj/diana/NYVIS_DEV/Stefan.Fagerstrom/profet2/local/lib -lqUtilities -lpuTools -lpuCtools -lpuSQL -lpq \
-L/usr/lib64/mysql -lmysqlclient

SOURCES += main.cpp\
        mainwindow.cpp \
    dianaclient.cpp \
    senderthread.cpp \
    connectthread.cpp

HEADERS  += mainwindow.h \
    dianaclient.h \
    senderthread.h \
    connectthread.h

FORMS    += mainwindow.ui \
    Client.ui
