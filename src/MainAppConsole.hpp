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
#ifndef __MainAppConsole_hpp__
#define __MainAppConsole_hpp__

#include <string>
#include <QObject>
#include "Interpreter.hpp"

class MainAppConsole : public QObject
{
  Q_OBJECT
  Interpreter *m_eval;
  ScopePtr m_global;
  QStringList basePath, userPath;
  QTimer *profilerTimer, *refreshTimer;
public:
  MainAppConsole();
  ~MainAppConsole();
  Context *NewContext();
  int  StartNewInterpreterThread();
  void UpdatePaths();
  void UpdateBasePath(QString rootpath);
  void SetRootPath(QString path, Interpreter* eval);
  bool debugwin;
  void EnableProfiler( bool bEnable );
public slots:
  int Run();
  void DoStuff();
  void ExecuteLine(QString txt);
  void DoGraphicsCall(Interpreter*, FuncPtr f, ArrayVector m, int narg);
  void Quit();
  void Crashed();
  void RegisterInterrupt();
  void EnableRepaint();
  void DisableRepaint();
  void CollectProfileSample();
  //  void showFileAtLine(QString fileName, int lineNumber);
  void ControlProfiler(bool enableflag);

  void outputRawText(QString);
signals:
  void Shutdown();
  void Initialize();
  void SetEnable( bool );
};

#endif
