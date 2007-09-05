
ADD_DEFINITIONS( ${QT_DEFINITIONS} ) 
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} )


QT4_WRAP_CPP( KeyManager.moc.cpp KeyManager.cpp )
QT4_WRAP_CPP( QTTerm.moc.cpp QTTerm.cpp )
QT4_WRAP_CPP( Editor.moc.cpp Editor.cpp ) 
QT4_WRAP_CPP( PathTool.moc.cpp PathTool.cpp )  
QT4_WRAP_CPP( highlighter.moc.cpp highlighter.cpp )
QT4_WRAP_CPP( VariablesTool.moc.cpp VariablesTool.cpp )
QT4_WRAP_CPP( FileTool.moc.cpp FileTool.cpp )
QT4_WRAP_CPP( HistoryWidget.moc.cpp HistoryWidget.cpp )  
QT4_WRAP_CPP( ToolDock.moc.cpp ToolDock.cpp )
QT4_WRAP_CPP( VariablesTool.moc.cpp VariablesTool.cpp )

QT4_WRAP_UI( findform.ui.h findform.ui )
QT4_WRAP_UI( replaceform.ui.h replaceform.ui )
QT4_WRAP_UI( synlightconf.ui.h synlightconf.ui )  
QT4_WRAP_UI( indentconf.ui.h indentconf.ui )


ADD_LIBRARY( XP DynLib.cpp 
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
indentconf.ui )

