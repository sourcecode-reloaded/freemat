INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} )


ADD_LIBRARY( XP 
DynLib.cpp 
KeyManager.cpp 
PathSearch.cpp 
QTTerm.cpp 
System.cpp 
Editor.cpp 
PathTool.cpp 
highlighter.cpp 
FileTool.cpp 
Common.cpp 
ToolDock.cpp 
VariablesTool.cpp 
HistoryWidget.cpp 
findform.ui 
replaceform.ui 
synlightconf.ui 
indentconf.ui 
${CMAKE_CURRENT_SOURCE_DIR}/KeyManager.moc.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/QTTerm.moc.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/Editor.moc.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/PathTool.moc.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/highlighter.moc.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/VariablesTool.moc.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/FileTool.moc.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/HistoryWidget.moc.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/ToolDock.moc.cpp 
)

QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/KeyManager.hpp ${CMAKE_CURRENT_SOURCE_DIR}/KeyManager.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/QTTerm.hpp ${CMAKE_CURRENT_SOURCE_DIR}/QTTerm.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/Editor.hpp ${CMAKE_CURRENT_SOURCE_DIR}/Editor.moc.cpp ) 
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/PathTool.hpp ${CMAKE_CURRENT_SOURCE_DIR}/PathTool.moc.cpp )  
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/highlighter.hpp ${CMAKE_CURRENT_SOURCE_DIR}/highlighter.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/VariablesTool.hpp ${CMAKE_CURRENT_SOURCE_DIR}/VariablesTool.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/FileTool.hpp ${CMAKE_CURRENT_SOURCE_DIR}/FileTool.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/HistoryWidget.hpp ${CMAKE_CURRENT_SOURCE_DIR}/HistoryWidget.moc.cpp )  
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/ToolDock.hpp ${CMAKE_CURRENT_SOURCE_DIR}/ToolDock.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/VariablesTool.hpp ${CMAKE_CURRENT_SOURCE_DIR}/VariablesTool.moc.cpp )
QT4_WRAP_UI( findform.ui.h findform.ui )
QT4_WRAP_UI( replaceform.ui.h replaceform.ui )
QT4_WRAP_UI( synlightconf.ui.h synlightconf.ui )  
QT4_WRAP_UI( indentconf.ui.h indentconf.ui )
