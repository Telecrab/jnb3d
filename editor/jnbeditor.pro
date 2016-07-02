#-------------------------------------------------
#
# Project created by QtCreator 2012-09-12T15:26:03
#
#-------------------------------------------------

QT       += widgets core gui multimedia

TARGET = jnbeditor
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    imageitem.cpp \
    soundwidget.cpp \
    musicwidget.cpp \
    micromod/micromod.c \
    graphicresourceview.cpp \
    ../lib/resources/datloader.cpp \
    ../lib/resources/resourcecontainer.cpp

HEADERS  += \
    mainwindow.h \
    imageitem.h \
    soundwidget.h \
    musicwidget.h \
    micromod/micromod.h \
    graphicresourceview.h \
    ../lib/resources/datloader.h \
    ../lib/resources/resourcecontainer.h

FORMS    += \
    mainwindow.ui \
    soundwidget.ui \
    musicwidget.ui

RESOURCES += \
    resources.qrc

QMAKE_CXXFLAGS += -std=c++11 -Wall
