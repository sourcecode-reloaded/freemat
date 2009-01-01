/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include "application.hpp"
#include <qapplication.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qfiledialog.h>
#include <qclipboard.h>
#include <iostream>
#include "FMFontDialog.hpp"
#include <qsettings.h>
#include "Interpreter.hpp"
#include "highlighter.hpp"
#include "Editor.hpp"
#include <QtGui>
#include <QDebug>
#include "PathTool.hpp"
#include "ToolDock.hpp"

const int about_linecount = 21;
const char *about_strings[] = {"Julie Maya & Neil - My Fan Club",
			       "Bruno De Man - New icon, general suggestions, support and FreeMat advocate",
			       "Thomas Beutlich - MAT file support",
			       "M. Vogel - Matlab compatibility scripts",
			       "Brian Yanoff - Compatibility scripts",
			       "Jeff Fessler - Support and test code, help with  class support",
			       "Al Danial - Help with sparse matrix support",
			       "MT19937 - Takuji Nishimura and Makoto Matsumoto's random number generator",
			       "RANLIB - Random number generator library",
			       "ATLAS - Optimized BLAS",
			       "LAPACK - Linear algebra",
			       "UMFPACK - Sparse linear equation solver",
			       "ARPACK - Sparse eigenvalue problems",
			       "FFTW - Fast Fourier Transforms",
			       "ffcall - Foreign Function interface",
			       "Qt4 - Cross platform GUI and API",
			       "libtecla - Inspiration for console interface code",
			       "wxbasic - Inspiration for interpreter layout",
			       "kde/konsole - Inspiration for QTTerm (GUI Console)",
			       "tubeplot - Written by Anders Sandberg",
			       "nsis - Installer on Win32"};

#define QUOTEME(x) #x
#define MAKEASCII(x) x.toAscii().constData()
#include <QtGui>

ApplicationWindow::~ApplicationWindow() {
}

void ApplicationWindow::createActions() {
  //  filetoolAct = new QAction("&File Browser",this);
  //  connect(filetoolAct,SIGNAL(triggered()),this,SLOT(filetool()));
  //  workAct = new QAction("&Workspace Tool",this);
  //  connect(workAct,SIGNAL(triggered()),this,SLOT(workspacetool()));
  //  historyAct = new QAction("Show &History Tool",this);
  //  connect(historyAct,SIGNAL(triggered()),this,SLOT(history()));
  cleanHistoryAct = new QAction("&Clear History Tool",this);
  connect(cleanHistoryAct,SIGNAL(triggered()),this,SLOT(cleanhistory()));
  editorAct = new QAction("&Editor",this);
  editorAct->setShortcut(Qt::Key_E | Qt::CTRL);
  connect(editorAct,SIGNAL(triggered()),this,SLOT(editor()));
  pathAct = new QAction("&Path Tool",this);
  connect(pathAct,SIGNAL(triggered()),this,SLOT(path()));
  saveAct = new QAction(QIcon(":/images/save.png"),"&Save Transcript",this);
  saveAct->setShortcut(Qt::Key_S | Qt::CTRL);
  connect(saveAct,SIGNAL(triggered()),this,SLOT(save()));
  clearAct = new QAction("&Clear Console",this);
  connect(clearAct,SIGNAL(triggered()),this,SLOT(clearconsole()));
  quitAct = new QAction(QIcon(":/images/quit.png"),"&Quit",this);
  quitAct->setShortcut(Qt::Key_Q | Qt::CTRL); 
  connect(quitAct,SIGNAL(triggered()),this,SLOT(close()));
  copyAct = new QAction(QIcon(":/images/copy.png"),"&Copy",this);
  copyAct->setShortcut(Qt::Key_C | Qt::CTRL);
  connect(copyAct,SIGNAL(triggered()),this,SLOT(copy()));
  pasteAct = new QAction(QIcon(":/images/paste.png"),"&Paste",this);
  pasteAct->setShortcut(Qt::Key_V | Qt::CTRL);
  connect(pasteAct,SIGNAL(triggered()),this,SLOT(paste()));
  fontAct = new QAction("&Font",this);
  connect(fontAct,SIGNAL(triggered()),this,SLOT(font()));
  scrollbackAct = new QAction("&Scrollback Size",this);
  connect(scrollbackAct,SIGNAL(triggered()),this,SLOT(scrollback()));
  aboutAct = new QAction("&About",this);
  connect(aboutAct,SIGNAL(triggered()),this,SLOT(about()));
  manualAct = new QAction("Online &Manual",this);
  manualAct->setShortcut(Qt::Key_F1);
  connect(manualAct,SIGNAL(triggered()),this,SLOT(manual()));
  aboutQt = new QAction("About &Qt",this);
  connect(aboutQt,SIGNAL(triggered()),qApp,SLOT(aboutQt()));
  pauseAct = new QAction(QIcon(":/images/player_pause.png"),"&Pause",this);
  continueAct = new QAction(QIcon(":/images/dbgrun.png"),"&Continue",this);
  continueAct->setShortcut(Qt::Key_Down | Qt::CTRL); 
  stopAct = new QAction(QIcon(":/images/player_stop.png"),"&Stop",this);
  stopAct->setShortcut(Qt::Key_Escape | Qt::CTRL); 
  dbStepAct = new QAction(QIcon(":/images/dbgnext.png"),"&Step Over",this);
  dbStepAct->setShortcut(Qt::Key_F10); 
  dbTraceAct = new QAction(QIcon(":/images/dbgstep.png"),"&Step Into",this);
  dbTraceAct->setShortcut(Qt::Key_F11); 
  checkUpdates = new QAction("Check for Updated Software",this);
}


void ApplicationWindow::createMenus() {
  fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction(saveAct);
  fileMenu->addAction(clearAct);
  fileMenu->addAction(quitAct);
  editMenu = menuBar()->addMenu("&Edit");
  editMenu->addAction(copyAct);
  editMenu->addAction(pasteAct);
  QMenu* configMenu = editMenu->addMenu("&Preferences");
  configMenu->addAction(fontAct);
  configMenu->addAction(scrollbackAct);
  debugMenu = menuBar()->addMenu("&Debug");
  debugMenu->addAction(pauseAct);
  debugMenu->addAction(continueAct);
  debugMenu->addAction(stopAct);
  debugMenu->addAction(dbStepAct);
  debugMenu->addAction(dbTraceAct);
  toolsMenu = menuBar()->addMenu("&Tools");
  toolsMenu->addAction(editorAct);
  toolsMenu->addAction(pathAct);
  //  toolsMenu->addAction(filetoolAct);
  //  toolsMenu->addAction(workAct);
  //  historyMenu = toolsMenu->addMenu("&History");
  //  historyMenu->addAction(historyAct);
  toolsMenu->addAction(cleanHistoryAct);
  
  helpMenu = menuBar()->addMenu("&Help");
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(manualAct);
  helpMenu->addAction(aboutQt);
  helpMenu->addAction(checkUpdates);
}

bool ApplicationWindow::event(QEvent* e) {
  if (e->type() == QEvent::WindowActivate) {
    if (m_term)
      m_term->setFocus(Qt::MouseFocusReason);
  }
  return QMainWindow::event(e);
}

void ApplicationWindow::createToolBars() {
  editToolBar = addToolBar("Edit");
  editToolBar->addAction(copyAct);
  editToolBar->addAction(pasteAct);
  editToolBar->setObjectName("edittoolbar");
  debugToolBar = addToolBar("Debug");
  debugToolBar->addAction(pauseAct);
  debugToolBar->addAction(continueAct);
  debugToolBar->addAction(stopAct);
  debugToolBar->addAction(dbStepAct);
  debugToolBar->addAction(dbTraceAct);
  m_stacktool = new StackTool;
  debugToolBar->addWidget(m_stacktool);
  debugToolBar->setObjectName("debugtoolbar");
  dirToolBar = addToolBar("Current directory");
  dirToolBar->setObjectName("dirtoolbar");
  cdCombo = new QComboBox;
  cdCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  dirToolBar->addWidget(cdCombo);
  cdCombo->setVisible(true);
  cdCombo->setObjectName("cdCombo");
  cdButton = new QPushButton("...");
  cdButton->setMaximumWidth(25);
  cdButton->setObjectName("cdButton");
  dirToolBar->addWidget(cdButton);
  cdButton->setVisible(true);
}

void ApplicationWindow::createStatusBar() {
  statusBar()->showMessage("Ready");
}

ApplicationWindow::ApplicationWindow() : QMainWindow() {
  setWindowIcon(QPixmap(":/images/freemat_small_mod_64.png"));
  setWindowTitle(Interpreter::getVersionString() + " Command Window");
  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  initializeTools();
  createToolBox();
  setObjectName("appwindow");
  isEditorExist = false;
}

void ApplicationWindow::createToolBox() {
  //  m_tool = new ToolDock(this);
  //  addDockWidget(Qt::LeftDockWidgetArea, m_tool);
  m_filetool = new FileTool(this);
  addDockWidget(Qt::LeftDockWidgetArea, m_filetool);
  toolsMenu->addAction(m_filetool->toggleViewAction());
  m_history = new HistoryWidget(this);
  addDockWidget(Qt::LeftDockWidgetArea, m_history);
  toolsMenu->addAction(m_history->toggleViewAction());
  m_variables = new VariablesTool(this);
  addDockWidget(Qt::LeftDockWidgetArea, m_variables);
  toolsMenu->addAction(m_variables->toggleViewAction());
}

void ApplicationWindow::initializeTools() {
}

void ApplicationWindow::CWDChanged() {
  QString curdir=QDir::currentPath();
  int ind = cdCombo->findText(curdir);
  if (ind>=0)
    cdCombo->removeItem(ind);
  cdCombo->insertItem(0,curdir);
  cdCombo->setCurrentIndex(0);
  while (cdCombo->count()>12)
    cdCombo->removeItem(cdCombo->count()-1);
}

void ApplicationWindow::closeEvent(QCloseEvent* ce) {
  emit shutdown();
  if (isEditorExist) {
    ce->ignore();
    return;
  }
  writeSettings();
  //  delete m_tool;
  delete m_filetool;
  delete m_history;
  delete m_variables;
  ce->accept();
  qApp->exit(0);
}

void ApplicationWindow::readSettings() {
  QSettings settings("FreeMat", Interpreter::getVersionString());
  QPoint pos = settings.value("mainwindow/pos", QPoint(200, 200)).toPoint();
  QSize size = settings.value("mainwindow/size", QSize(600, 400)).toSize();
  if ((pos.x() < 0) || (pos.y() < 0))
    pos = QPoint(200,200);
  resize(size);
  move(pos);
  QByteArray state = settings.value("mainwindow/state").toByteArray();
  restoreState(state);
  QStringList cdList = settings.value("mainwindow/cdlist").toStringList();
  if (cdList.count()>0)
    cdCombo->addItems(cdList);
}

void ApplicationWindow::writeSettings() {
  QSettings settings("FreeMat", Interpreter::getVersionString());
  settings.setValue("mainwindow/state",saveState());
  settings.setValue("mainwindow/pos", pos());
  settings.setValue("mainwindow/size", size());
  QStringList cdList;
  for (int j=0;j<cdCombo->count();j++)
    cdList.append(cdCombo->itemText(j));
  settings.setValue("mainwindow/cdlist",cdList);
  settings.sync();
}

void ApplicationWindow::checkEditorExist(bool isExist){
  isEditorExist = isExist;
}
void ApplicationWindow::tclose() {
  close();
}

void ApplicationWindow::SetGUITerminal(QTTerm* term) {
  m_term = term;
  setCentralWidget(term);
}

void ApplicationWindow::clearconsole() {
  m_keys->ClearDisplayCommand();
}

void ApplicationWindow::SetKeyManager(KeyManager *keys) {
  m_keys = keys;
  connect(keys,SIGNAL(SendCommand(QString)),
 	  m_history,SLOT(addCommand(QString)));
  connect(m_history,SIGNAL(clearHistory()),
	  keys,SLOT(ClearHistory()));
  connect(m_history,SIGNAL(writeHistory()),
	  keys,SLOT(WriteHistory()));
  connect(m_history,SIGNAL(sendCommand(QString)),
 	  keys,SLOT(QueueCommand(QString)));
  connect(m_filetool,SIGNAL(sendCommand(QString)),
 	  keys,SLOT(QueueMultiString(QString)));
  connect(keys,SIGNAL(UpdateCWD()),m_filetool,SLOT(updateCWD()));
  connect(keys,SIGNAL(UpdateCWD()),this,SLOT(CWDChanged()));
  connect(keys,SIGNAL(updateDirView(QVariant)),m_filetool,SLOT(updateDirView(QVariant)));
  connect(keys,SIGNAL(updateVarView(QVariant)),m_variables,SLOT(updateVariableView(QVariant)));
  connect(m_stacktool,SIGNAL(updateStackDepth(int)),keys,SLOT(updateStackDepth(int)));
  connect(keys,SIGNAL(updateStackView(QStringList)),
	  m_stacktool,SLOT(updateStackView(QStringList)));
  //  connect(keys,SIGNAL(UpdateInfoViews()),
  //	  m_variables,SLOT(refresh()));
  connect(pauseAct,SIGNAL(triggered()),m_keys,SIGNAL(RegisterInterrupt()));
  connect(continueAct,SIGNAL(triggered()),m_keys,SLOT(ContinueAction()));
  connect(stopAct,SIGNAL(triggered()),m_keys,SLOT(StopAction()));
  connect(dbStepAct,SIGNAL(triggered()),m_keys,SLOT(DbStepAction()));
  connect(dbTraceAct,SIGNAL(triggered()),m_keys,SLOT(DbTraceAction()));
  connect(checkUpdates,SIGNAL(triggered()),this,SLOT(checkForUpdates()));
  connect(cdCombo,SIGNAL(activated(const QString&)),
	  m_keys,SLOT(ChangeDir(const QString&)));
  connect(cdButton,SIGNAL(clicked()),this,SLOT(dirButtonClicked()));
}

void ApplicationWindow::save() {
  QString fn = QFileDialog::getSaveFileName();
  if (!fn.isEmpty()) {
    FILE *fp;
    fp = fopen(MAKEASCII(fn),"w");
    if (!fp) {
      char buffer[1000];
      sprintf(buffer,"Unable to save transcript to file %s!",
	      MAKEASCII(fn));
      QMessageBox::information(this,"Error",buffer,
			       QMessageBox::Ok);
      return;
    }
    fprintf(fp,"%s",m_term->getAllText().toStdString().c_str());
    fclose(fp);
  }
}

void ApplicationWindow::copy() {
  QString copytextbuf = m_term->getSelectionText();
  QClipboard *cb = QApplication::clipboard();
  cb->setText(copytextbuf, QClipboard::Clipboard);
}

void ApplicationWindow::paste() {
  QClipboard *cb = QApplication::clipboard();
  QString text;
  if (cb->supportsSelection())
    text = cb->text(QClipboard::Selection);
  if (text.isNull())
    text = cb->text(QClipboard::Clipboard);
  text.replace("\r\n","\n");
  text.replace("\r","\n");
  if (!text.isNull()) {
    m_keys->QueueMultiString(text);
    m_term->clearSelection();
  }
}

void ApplicationWindow::scrollback() {
  int scroll_limit = m_term->getScrollbackLimit();
  bool ok;
  int new_limit = QInputDialog::getInteger(this, 
					   "Change Scrollback of Command Window",
					   "Lines:",scroll_limit,100,10000,
					   1,&ok);
  if (ok) {
    QSettings settings("FreeMat", Interpreter::getVersionString());
    settings.setValue("console/scrollback",new_limit);
    m_term->setScrollbackLimit(new_limit);
  }
}

void ApplicationWindow::font() {
  QFont old_font = m_term->getFont();
  FMFontDialog g(old_font,this);
  if (g.exec() == QDialog::Accepted) {
    QFont new_font = g.font();
    QSettings settings("FreeMat", Interpreter::getVersionString());
    settings.setValue("terminal/font",new_font.toString());
    m_term->setFont(new_font);
  }
}

void ApplicationWindow::manual() {
  emit startHelp();
}

void ApplicationWindow::editor() {
  emit startEditor();
}

void ApplicationWindow::path() {
  emit startPathTool();
}

void ApplicationWindow::workspacetool() {
  //  m_tool->show();
  //  m_tool->raiseVariables();
}

void ApplicationWindow::filetool() {
  //  m_tool->show();
  //  m_tool->raiseFileTool();
}

void ApplicationWindow::history() {
  //  m_tool->show();
  //  m_tool->raiseHistoryTool();
}

void ApplicationWindow::cleanhistory() {
  m_history->clear();
}

void ApplicationWindow::about() {
  QString text;
  text += "FreeMat Version ";
  text += QUOTEME(VERSION);
  text += "\n\n";
  text += "Licensed under the GNU Public License Ver 2\n";
  text += "Web: http://freemat.sf.net\n";
  text += "Email: freemat@googlegroup.com\n";
  QMessageBox::about(this,"About FreeMat",text);
}

void ApplicationWindow::init() {
  //  m_variables->setContext(m_keys->GetCompletionContext());
  // Check for the latest version?
  QSettings settings("FreeMat", Interpreter::getVersionString());
  QDate lastCheck = settings.value("mainwindow/lastcheckdate").toDate();
  if (QDate::currentDate().daysTo(lastCheck) > 7) {
    checkForUpdates();
  }
  QString font = settings.value("terminal/font").toString();
  if (!font.isNull()) {
    QFont new_font;
    new_font.fromString(font);
    m_term->setFont(new_font);
  }
  else {
  /* Set to the most common monospace font on Windows, Mac and Unix shown at
    http://www.codestyle.org/css/font-family/sampler-Monospace.shtml */
  #ifdef Q_WS_WIN
    QFont new_font("Courier New", 10, QFont::Normal);
  #else 
      #ifdef Q_WS_MAC
        QFont new_font("Monaco", 10, QFont::Normal);
      #else
        QFont new_font("DejaVu Sans Mono", 10, QFont::Normal);
      #endif
  #endif
    m_term->setFont(new_font);
  }
}

void ApplicationWindow::dirButtonClicked() {
  QString dir = QFileDialog::getExistingDirectory(this,
						  "Choose a directory",
						  "",
						  QFileDialog::ShowDirsOnly);
  if (!dir.isEmpty()) 
    m_keys->ChangeDir(dir);
}

void ApplicationWindow::checkForUpdates() {
  m_http = new QHttp(this);
  connect(m_http, SIGNAL(requestFinished(int, bool)),
	  this, SLOT(httpRequestFinished(int, bool)));
  QUrl url("http://freemat.sourceforge.net/version.txt");
  m_buffer.open(QBuffer::ReadWrite);
  m_http->setHost(url.host(), url.port() != -1 ? url.port() : 80);
  httpGetId = m_http->get(url.path(), &m_buffer);
  QSettings settings("FreeMat", Interpreter::getVersionString());
  settings.setValue("mainwindow/lastcheckdate",QDate::currentDate());
}

void ApplicationWindow::httpRequestFinished(int requestId, bool error) {
  char buffer[1000];
  if (error)
    statusBar()->showMessage("Unable to check for updates...");
  if (requestId != httpGetId) return;
  const char *qp = m_buffer.data();

  if (atof(QUOTEME(VERSION)) < atof(m_buffer.data())) {
    sprintf(buffer,"A newer version of FreeMat appears to available. \n Please update by downloading version %s at http://freemat.sourceforge.net.",qp);
    QMessageBox::information(this,"Update Available",
			     buffer,QMessageBox::Ok);
  } else 
    statusBar()->showMessage("You are running the latest version of FreeMat");
}
