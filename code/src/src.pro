# This file is a part of
# QVGE - Qt Visual Graph Editor
#
# (c) 2016-2018 Ars L. Masiuk (ars.masiuk@gmail.com)
#
# It can be used freely, maintaining the information above.


TEMPLATE = app
TARGET = qvgeapp
VERSION = 0.4.1.0

CONFIG(debug, debug|release){
        DESTDIR = $$OUT_PWD/../bin.debug
        LIBS += -L$$OUT_PWD/../lib.debug
}
else{
        DESTDIR = $$OUT_PWD/../bin
        LIBS += -L$$OUT_PWD/../lib
}

QT += core gui widgets xml opengl network
CONFIG += c++11

# app sources
SOURCES += $$files($$PWD/*.cpp)
HEADERS += $$files($$PWD/*.h)
FORMS += $$files($$PWD/*.ui)
RESOURCES += $$files($$PWD/*.qrc)

# base sources
include($$PWD/../base/base.pri)
include($$PWD/../qvge/qvge.pri)

# includes & libs
INCLUDEPATH += $$PWD $$PWD/..
LIBS += -lqvge

win32{
    LIBS += -lopengl32 -luser32

    RC_FILE = $$PWD/../win32/icon.rc
}

# compiler stuff
win32-msvc*{
  QMAKE_CXXFLAGS += /MP
}
