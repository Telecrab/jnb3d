#-------------------------------------------------
#
# Project created by QtCreator 2012-09-12T15:26:03
#
#-------------------------------------------------

QT       += widgets core gui multimedia

TARGET = jnbeditor
TEMPLATE = app


SOURCES += main.cpp\
        resourceview.cpp \
    mainwindow.cpp \
    imageitem.cpp \
    soundwidget.cpp \
    musicwidget.cpp \
    micromod/micromod.c

HEADERS  += \
        resourceview.h \
    mainwindow.h \
    imageitem.h \
    soundwidget.h \
    musicwidget.h \
    micromod/micromod.h

FORMS    += \
    mainwindow.ui \
    soundwidget.ui \
    musicwidget.ui

RESOURCES +=

QMAKE_CXXFLAGS += -std=c++11
