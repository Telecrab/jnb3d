TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    glad/src/glad.c \
    window.cpp \
    sprite.cpp \
    animation.cpp \
    ../lib/resources/datloader.cpp \
    ../lib/resources/resourcecontainer.cpp \
    sdlfileio.cpp \
    ../lib/resources/abstractfileio.cpp \
    calllogger.cpp

INCLUDEPATH += $$PWD/glad/include \
               $$PWD/glm

include(deployment.pri)
qtcAddDeployment()


#win32:LIBS += -L$$PWD/glfw-3.1.1.bin.WIN32/lib-mingw/ -lglfw3 -lgdi32 -lopengl32
#win32:INCLUDEPATH += $$PWD/glfw-3.1.1.bin.WIN32/include
#win32:DEPENDPATH += $$PWD/glfw-3.1.1.bin.WIN32/include
#win32:PRE_TARGETDEPS += $$PWD/glfw-3.1.1.bin.WIN32/lib-mingw/libglfw3.a

unix:LIBS += -lSDL2 -ldl
#unix:PRE_TARGETDEPS += $$PWD/build-SDL2/libSDL2.a


QMAKE_CXXFLAGS += -std=c++11 -Wall

HEADERS += \
    window.h \
    sprite.h \
    animation.h \
    common.h \
    stb_image.h \
    ../lib/resources/datloader.h \
    ../lib/resources/resourcecontainer.h \
    sdlfileio.h \
    ../lib/resources/abstractfileio.h \
    calllogger.h
