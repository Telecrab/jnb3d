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
    soundwidget.cpp

HEADERS  += \
        resourceview.h \
    mainwindow.h \
    imageitem.h \
    soundwidget.h

FORMS    += \
    mainwindow.ui \
    soundwidget.ui

RESOURCES +=

QMAKE_CXXFLAGS += -std=c++11
