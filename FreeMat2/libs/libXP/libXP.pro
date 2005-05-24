TEMPLATE = lib

TARGET = XP

CONFIG += staticlib

INCLUDEPATH = ../libFreeMat ../..

HEADERS += DynLib.hpp \
System.hpp \
PathSearch.hpp \
RGBImage.hpp \
Rect2D.hpp \
PostScriptGC.hpp \
Point2D.hpp \
GraphicsContext.cpp \
Color.hpp \
KeyManager.hpp \
BitmapPrinterGC.hpp \
helv_table.h \
PrintableWidget.hpp 

SOURCES += DynLib.cpp \
System.cpp \
PathSearch.cpp \
Rect2D.cpp \
PostScriptGC.cpp \
Point2D.cpp \
GraphicsContext.cpp \
Color.cpp \
KeyManager.cpp \
BitmapPrinterGC.cpp \
PrintableWidget.cpp 

