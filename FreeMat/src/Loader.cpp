// WARNING - This file is automatically generated by
// the siggen.py script in tools.  Do not
// edit it by hand, or changes will be lost!  If you
// need to add a built in function to FreeMat, add a
// signature block to its documentation and rerun siggen.


// First each function gets prototyped.
#include "Array.hpp"
#include "Context.hpp"


ArrayVector EditorFunction(int, const ArrayVector&, Interpreter*);
ArrayVector EditFunction(int, const ArrayVector&, Interpreter*);
ArrayVector ThreadIDFunction(int, const ArrayVector&, Interpreter*);
ArrayVector PauseFunction(int, const ArrayVector&, Interpreter*);
ArrayVector SleepFunction(int, const ArrayVector&, Interpreter*);
ArrayVector ThreadNewFunction(int, const ArrayVector&, Interpreter*);
ArrayVector ThreadStartFunction(int, const ArrayVector&, Interpreter*);
ArrayVector ThreadValueFunction(int, const ArrayVector&);
ArrayVector ThreadWaitFunction(int, const ArrayVector&);
ArrayVector ThreadKillFunction(int, const ArrayVector&);
ArrayVector ThreadFreeFunction(int, const ArrayVector&);
ArrayVector ClcFunction(int, const ArrayVector&);
ArrayVector ProfilerFunction(int, const ArrayVector&, Interpreter*);
ArrayVector RootPathFunction(int, const ArrayVector&, Interpreter*);


void LoadBuiltinFunctionsMain(Context *context, bool guiflag) {
  if (guiflag)  context->addGfxSpecialFunction("editor",EditorFunction,0,0,NULL);
  if (guiflag)  context->addGfxSpecialFunction("edit",EditFunction,-1,0,NULL);
  context->addSpecialFunction("threadid",ThreadIDFunction,0,1,NULL);
  if (guiflag)  context->addGfxSpecialFunction("pause",PauseFunction,1,0,"flag",NULL);
  if (guiflag)  context->addGfxSpecialFunction("sleep",SleepFunction,1,0,"n",NULL);
  context->addSpecialFunction("threadnew",ThreadNewFunction,0,1,NULL);
  context->addSpecialFunction("threadstart",ThreadStartFunction,4,0,"threadid","function","nargout","varargin",NULL);
  context->addFunction("threadvalue",ThreadValueFunction,2,-1,"handle","timeout",NULL);
  context->addFunction("threadwait",ThreadWaitFunction,2,1,"handle","timeout",NULL);
  context->addFunction("threadkill",ThreadKillFunction,1,0,"handle",NULL);
  context->addFunction("threadfree",ThreadFreeFunction,2,0,"handle","timeout",NULL);
  if (guiflag)  context->addGfxFunction("clc",ClcFunction,0,0,NULL);
  context->addSpecialFunction("profiler",ProfilerFunction,-1,-1,NULL);
  if (guiflag)  context->addGfxSpecialFunction("rootpath",RootPathFunction,1,0,"path",NULL);
}

