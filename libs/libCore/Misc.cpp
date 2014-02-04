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

#include "Array.hpp"
#include "Interpreter.hpp"
#include "Scanner.hpp"
#include "Parser.hpp"
#include "System.hpp"
#include "Algorithms.hpp"
#include "PathSearch.hpp"

//@@Signature
//sfunction simkeys SimKeysFunction
//inputs itext
//outputs otext
//DOCBLOCK freemat_simkeys
ArrayVector SimKeysFunction(int nargout, const ArrayVector& arg,
			    Interpreter* eval) {
  if (arg.size() == 0)
    throw Exception("simkeys requires at least one argument (the cell array of strings to simulate)");
  ParentScopeLocker lock(eval->getContext());
  eval->clearCaptureString();
  eval->setCaptureState(true);
  if (arg[0].dataClass() != CellArray)
    throw Exception("simkeys requires a cell array of strings");
  const BasicArray<Array> &dp(arg[0].constReal<Array>());
  for (index_t i=1;i<=dp.length();i++) {
    FMString txt(dp[i].asString());
    if ((txt.size() > 0) && (!txt.endsWith('\n')))
      txt.push_back('\n');
    eval->ExecuteLine(txt);
  }
  eval->ExecuteLine("quit\n");
  try {
    while(1) 
      eval->evalCLI();
  } catch (InterpreterContinueException& e) {
  } catch (InterpreterBreakException& e) {
  } catch (InterpreterReturnException& e) {
  } catch (InterpreterRetallException& e) {
  } catch (InterpreterQuitException& e) {
  }
  eval->setCaptureState(false);
  return ArrayVector(Array(eval->getCaptureString()));
}

//@@Signature
//sfunction diary DiaryFunction
//inputs x
//outputs state
//DOCBLOCK freemat_diary
ArrayVector DiaryFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (nargout == 1) {
    if (arg.size() > 0)
      throw Exception("diary function with an assigned return value (i.e, 'x=diary') does not support any arguments");
    return ArrayVector(Array(bool(eval->getDiaryState())));
  }
  if (arg.size() == 0) {
    eval->setDiaryState(!eval->getDiaryState());
    return ArrayVector();
  }
  FMString diaryString(arg[0].asString());
  if (diaryString.toLower() == "on")
    eval->setDiaryState(true);
  else if (diaryString.toLower() == "off")
    eval->setDiaryState(false);
  else {
    eval->setDiaryFilename(diaryString);
    eval->setDiaryState(true);
  }
  return ArrayVector();
}


//@@Signature
//sfunction quiet QuietFunction
//inputs mode
//outputs mode
//DOCBLOCK freemat_quiet
ArrayVector QuietFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() > 0) {
    FMString qtype(arg[0].asString().toUpper());
    if (qtype == "NORMAL")
      eval->setQuietLevel(0);
    else if (qtype == "QUIET")
      eval->setQuietLevel(1);
    else if (qtype == "SILENT")
      eval->setQuietLevel(2);
    else
      throw Exception("quiet function takes one argument - the quiet level (normal, quiet, or silent) as a string");
  }
  FMString rtype;
  if (eval->getQuietLevel() == 0)
    rtype = "normal";
  else if (eval->getQuietLevel() == 1)
    rtype = "quiet";
  else if (eval->getQuietLevel() == 2)
    rtype = "silent";
  return ArrayVector(Array(rtype));
}

//@@Signature
//sfunction source SourceFunction
//inputs filename
//outputs none
//DOCBLOCK freemat_source
ArrayVector SourceFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() != 1)
    throw Exception("source function takes exactly one argument - the filename of the script to execute");
  FMString filename = arg[0].asString();
  bool failed;
  FMString scriptText(ReadFileIntoString(filename,failed));
  if (failed)
    throw Exception("Unable to open file " + filename + " for reading");
  Scanner S(scriptText,filename);
  Parser P(S);
  Tree pcode(P.process());
  if (pcode.is(TOK_FUNCTION_DEFS))
    throw Exception("only scripts can be source-ed, not functions");
  Tree code = pcode.first();
  ParentScopeLocker lock(eval->getContext());
  eval->block(code);
  return ArrayVector();
}

//@@Signature
//sfunction builtin BuiltinFunction
//inputs fname varargin
//outputs varargout
//DOCBLOCK freemat_builtin
ArrayVector BuiltinFunction(int nargout, const ArrayVector& arg,Interpreter* eval){
  if (arg.size() == 0)
    throw Exception("builtin function requires at least one argument");
  if (!(arg[0].isString()))
    throw Exception("first argument to builtin must be the name of a function (i.e., a string)");
  FuncPtr funcDef;
  FMString fname = arg[0].asString();
  Context *context = eval->getContext();
  if (!context->lookupFunction(fname,funcDef))
    throw Exception("function " + fname + " undefined!");
  funcDef->updateCode(eval);
  if (funcDef->scriptFlag)
    throw Exception("cannot use feval on a script");
  ArrayVector newarg(arg);
  newarg.pop_front();
  bool flagsave = eval->getStopOverload();
  eval->setStopOverload(true);
  ArrayVector tmp(eval->doFunction(funcDef,newarg,nargout));
  eval->setStopOverload(flagsave);
  return tmp;
}

//DOCBLOCK freemat_startup

//@@Signature
//sfunction docli DoCLIFunction
//inputs none
//outputs none
//DOCBLOCK freemat_docli
ArrayVector DoCLIFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  Context *context = eval->getContext();
  context->deactivateCurrentScope();
  FuncPtr funcDef;
  if (eval->lookupFunction("startup",funcDef)) {
    funcDef->updateCode(eval);
    if (funcDef->scriptFlag) {
      try {
	eval->block(((MFunctionDef*)funcDef)->code);
      } catch (Exception& e) {
	eval->errorMessage("Startup script error:\n" + e.msg());
      }
    } else {
      eval->outputMessage(FMString("startup.m must be a script"));
    }
  }
  eval->doCLI();
  return ArrayVector();
}

//@@Signature
//function getenv GetEnvFunction
//inputs var
//outputs value
//DOCBLOCK os_getenv
ArrayVector GetEnvFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) throw Exception("getenv requires one string argument");
  FMString name(arg[0].asString());
  return Array(FMString(getenv(qPrintable(name))));
}

#if 0
//TODO - move to Python
//Signature
//function system SystemFunction
//inputs cmd
//outputs results
//DOCBLOCK os_system
ArrayVector SystemFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("System function takes one string argument");
  FMString systemArg(arg[0].asString());
  if (systemArg.size() == 0) 
    return ArrayVector();
  StringVector cp(DoSystemCallCaptured(systemArg));
  return CellArrayFromStringVector(cp);
}
#endif




