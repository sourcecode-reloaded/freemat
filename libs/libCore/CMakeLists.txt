FIND_PACKAGE(Qt4)

QT4_WRAP_CPP( helpwidget.moc.cpp helpwidget.cpp )
QT4_WRAP_CPP( XML.moc.cpp XML.cpp )

ADD_DEFINITIONS( ${QT_DEFINITIONS} ) 

INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}  ${QT_QTNETWORK_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} )

ADD_LIBRARY( Core Analyze.cpp 
Cast.cpp 
Constants.cpp 
Constructors.cpp 
Directory.cpp 
FFT.cpp 
helpwidget.cpp 
Inspect.cpp 
IO.cpp 
LoadCore.cpp 
Misc.cpp 
MPIWrap.cpp 
Random.cpp 
RanLib.cpp 
StringOps.cpp 
Transcendental.cpp 
Trigonometric.cpp 
Utils.cpp 
MatIO.cpp 
XML.cpp 
RPC.cpp 
helpwidget.moc.cpp
XML.moc.cpp )