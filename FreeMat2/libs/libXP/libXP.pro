TEMPLATE = lib

TARGET = XP

CONFIG += staticlib

INCLUDEPATH = ../libFreeMat ../..

HEADERS += DynLib.hpp \
System.hpp \
PathSearch.hpp \
Rect2D.hpp \
PostScriptGC.hpp \
Point2D.hpp \
GraphicsContext.cpp \
Color.hpp \
KeyManager.hpp \
helv_table.h \
XPWidget.hpp \
XPWindow.hpp \
QTGC.hpp

SOURCES += DynLib.cpp \
System.cpp \
PathSearch.cpp \
Rect2D.cpp \
PostScriptGC.cpp \
Point2D.cpp \
GraphicsContext.cpp \
Color.cpp \
KeyManager.cpp \
XPWidget.cpp \
XPWindow.cpp \
QTGC.cpp

