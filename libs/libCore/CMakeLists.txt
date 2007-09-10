

INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}  ${QT_QTNETWORK_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} )

QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/helpwidget.hpp ${CMAKE_CURRENT_SOURCE_DIR}/helpwidget.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/XML.hpp ${CMAKE_CURRENT_SOURCE_DIR}/XML.moc.cpp )

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
${CMAKE_CURRENT_SOURCE_DIR}/helpwidget.moc.cpp
${CMAKE_CURRENT_SOURCE_DIR}/XML.moc.cpp )