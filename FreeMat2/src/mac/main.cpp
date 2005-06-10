#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "GUITerminal.hpp"
#include "MainApp.hpp"
#include <qapplication.h>

using namespace FreeMat;

#include "BaseTerminal.hpp"
#include "Module.hpp"
#include "Class.hpp"
#include "LoadCore.hpp"
#include "LoadFN.hpp"
#include "GraphicsCore.hpp"
#include "File.hpp"


int RunMainApp(BaseTerminal* term, std::string helpPath) {
  Context *context = new Context;
  
  LoadModuleFunctions(context);
  LoadClassFunction(context);
  LoadCoreFunctions(context);
  LoadFNFunctions(context);
  LoadGraphicsCoreFunctions(context);  
  InitializeFigureSubsystem();

  const char *envPtr;
  envPtr = getenv("FREEMAT_PATH");
  term->setContext(context);
  if (envPtr)
    term->setPath(std::string(envPtr));
  else 
    term->setPath(std::string(""));
  WalkTree *twalk = new WalkTree(context,term);
  term->outputMessage(" Freemat v2.0");
  term->outputMessage("\n");
  term->outputMessage(" Copyright (c) 2002-2005 by Samit Basu\n");
  while (twalk->getState() != FM_STATE_QUIT) {
    twalk->resetState();
    twalk->evalCLI();
  }
  return 0;
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv, TRUE);
  
  GUITerminal *iterm = new GUITerminal;
  iterm->show();
  iterm->resizeTextSurface();
  
  return RunMainApp(iterm, "");
}
