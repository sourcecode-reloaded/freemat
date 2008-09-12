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

#include "FunctionDef.hpp"
#include "Interpreter.hpp"
#include "Parser.hpp"
#include "Exception.hpp"
#include <iostream>
#include <signal.h>
#include "SymbolTable.hpp"
#include "Types.hpp"
#include "MexInterface.hpp"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <sys/stat.h>
#include "MemPtr.hpp"
#include "Algorithms.hpp"

#if HAVE_AVCALL
#include "avcall.h"
#endif

#define MSGBUFLEN 2048

QMutex functiondefmutex;

StringVector IdentifierList(Tree *t) {
  StringVector retval;
  for (int index=0;index<t->numChildren();index++) {
    if (t->child(index)->is('&'))
      retval.push_back("&" + t->child(index)->first()->text());
    else
      retval.push_back(t->child(index)->text());
  }
  return retval;
}

void VariableReferencesList(Tree *t, StringVector& idents) {
  if (t->is(TOK_NEST_FUNC)) return;
  if (t->is(TOK_VARIABLE)) {
    bool exists = false;
    for (int i=0;(i<idents.size());i++) {
      exists = (idents[i] == t->first()->text());
      if (exists) break;
    }
    if (!exists)
      idents.push_back(t->first()->text());
  }
  for (int i=0;i<t->numChildren();i++)
    VariableReferencesList(t->child(i),idents);
}

AnonymousFunctionDef::AnonymousFunctionDef() {
}

AnonymousFunctionDef::~AnonymousFunctionDef() {
}

int AnonymousFunctionDef::inputArgCount() {
  return arguments.size();
}

int AnonymousFunctionDef::outputArgCount() {
  return -1;
}

ArrayVector AnonymousFunctionDef::evaluateFunction(Interpreter *eval, ArrayVector& inputs, int nargout) {
  ArrayVector outputs;
  if (!code.tree()->valid()) return outputs;
  Context * context = eval->getContext();
  context->pushScope("anonymous");
  eval->pushDebug("anonymous","anonymous");
  StringVector workspaceVars(workspace.getCompletions(""));
  for (int i=0;i<workspaceVars.size();i++)
    context->insertVariableLocally(workspaceVars[i],*workspace.findSymbol(workspaceVars[i]));
  int minCount = (((int)inputs.size()) < arguments.size()) ? 
    inputs.size() : arguments.size();
  for (int i=0;i<minCount;i++)
    context->insertVariableLocally(arguments[i],inputs[i]);
  try {
    try {
      eval->multiexpr(code.tree(),outputs);
    } catch (InterpreterBreakException& e) {
    } catch (InterpreterContinueException& e) {
    } catch (InterpreterReturnException& e) {
    }
    context->popScope();
    eval->popDebug();
  } catch (Exception& e) {
    context->popScope();
    eval->popDebug();
    throw;
  } catch (InterpreterRetallException& e) {
    context->popScope();
    eval->popDebug();
    throw;    
  }
  return outputs;
}

void AnonymousFunctionDef::initialize(Tree *t, Interpreter *eval) {
  name = t->text();
  arguments = IdentifierList(t->first());
  code = CodeBlock(t->second(),true);
  scriptFlag = false;
  temporaryFlag = false;
  graphicsFunction = false;
  StringVector vars;
  VariableReferencesList(t->second(),vars);
  for (int i=0;i<vars.size();i++) {
    ArrayReference ptr(eval->getContext()->lookupVariable(vars[i]));
    if (ptr.valid()) {
      //      dbout << "Captured VAR: " << vars[i] << "\r\n";
      workspace.insertSymbol(vars[i],*ptr);
    }
  }
}

MFunctionDef::MFunctionDef() {
  functionCompiled = false;
  localFunction = false;
  pcodeFunction = false;
#if !defined(_MSC_VER)
#warning - check pcode
#endif
  nestedFunction = false;
  capturedFunction = false;
}

MFunctionDef::~MFunctionDef() {
}

int MFunctionDef::inputArgCount() {
  if (arguments.size() == 0) return 0;
  if (arguments[arguments.size()-1] == "varargin")
    return -1;
  else
    return arguments.size();
}

int MFunctionDef::outputArgCount() {
  if (returnVals.size() == 0) return 0;
  if (returnVals[returnVals.size()-1] == "varargout")
    return -1;
  else
    return returnVals.size();
}

void MFunctionDef::printMe(Interpreter*eval) {
  StringVector tmp;
  eval->outputMessage("Function name:" + name);
  eval->outputMessage("Function class: Compiled M function\n");
  eval->outputMessage("returnVals: ");
  tmp = returnVals;
  int i;
  for (i=0;i<tmp.size();i++)
    eval->outputMessage(tmp[i] + " ");
  eval->outputMessage("\n");
  eval->outputMessage("arguments: ");
  tmp = arguments;
  for (i=0;i<tmp.size();i++) 
    eval->outputMessage(tmp[i] + " ");
  eval->outputMessage("\ncode: \n");
  code.tree()->print();
}

#if !defined(_MSC_VER)
static void CaptureFunctionPointer(FuncPtr &val, Interpreter *walker, 
				   MFunctionDef *parent, ScopePtr &workspace) {
  if (val->type() == FM_M_FUNCTION) {
    MFunctionDef* mptr = (MFunctionDef*) val;
    if (mptr->nestedFunction && !mptr->capturedFunction) {
      MFunctionDef* optr = new MFunctionDef;
      (*optr) = (*mptr);
      Context* context = walker->getContext();
      QString myScope = context->scopeName();
      context->bypassScope(1);
      QString parentScope = context->scopeName();
      context->restoreScope(1);
      if (!Scope::nests(parentScope,myScope)) {
	// Now capture the variables in our current scope
	for (int i=0;i<optr->variablesAccessed.size();i++) {
	  ArrayReference ptr(context->lookupVariable(optr->variablesAccessed[i]));
	  if (ptr.valid()) {
	    if (!workspace)
	      workspace = new Scope("captured",false);
	    workspace->insertVariable(optr->variablesAccessed[i],*ptr);
	  }
	}
	optr->workspace = workspace;
	optr->capturedFunction = true;
      }
      val = optr;
    }
  }
}

static void CaptureFunctionPointers(ArrayVector& outputs, Interpreter *walker, 
			     MFunctionDef *parent) {
// FIXME
//   ScopePtr workspace = NULL;
//   // First check for any 
//   for (int i=0;i<((int)outputs.size());i++) {
//     if (outputs[i].dataClass() == FM_FUNCPTR_ARRAY) {
//       FuncPtr *dp = (FuncPtr*) outputs[i].getReadWriteDataPointer();
//       for (int j=0;j<outputs[i].getLength();j++)
// 	CaptureFunctionPointer(dp[j],walker,parent,workspace);
//     }
//   }
}

ArrayVector MFunctionDef::evaluateFunction(Interpreter *walker, 
					   ArrayVector& inputs, 
					   int nargout) {
  ArrayVector outputs;
  Context* context;
  bool warningIssued;
  int minCount;
    
  if (!code.tree()->valid()) return outputs;
  context = walker->getContext();
  context->pushScope(name,nestedFunction);
  context->setVariablesAccessed(variablesAccessed);
  context->setLocalVariablesList(returnVals);
  if (capturedFunction && workspace) {
    StringVector workspaceVars(workspace->getCompletions(""));
    for (int i=0;i<workspaceVars.size();i++)
      context->insertVariableLocally(workspaceVars[i],
				     *workspace->lookupVariable(workspaceVars[i]));
  }
  walker->pushDebug(fileName,name);
  // When the function is called, the number of inputs is
  // at sometimes less than the number of arguments requested.
  // Check the argument count.  If this is a non-varargin
  // argument function, then use the following logic:
  minCount = 0;
  if (inputArgCount() != -1) {
    minCount = (((int)inputs.size()) < arguments.size()) ? 
      inputs.size() : arguments.size();
    for (int i=0;i<minCount;i++) {
      QString arg(arguments[i]);
      if (arg[0] == '&')
	arg.remove(0,1);
      context->insertVariableLocally(arg,inputs[i]);
    }
    context->insertVariableLocally("nargin",Array(double(minCount)));
  } else {
    // Count the number of supplied arguments
    int inputCount = inputs.size();
    context->insertVariableLocally("nargin",Array(double(inputCount)));
    // Get the number of explicit arguments
    int explicitCount = arguments.size() - 1;
    // For each explicit argument (that we have an input for),
    // insert it into the scope.
    minCount = (explicitCount < inputCount) ? explicitCount : inputCount;
    int i;
    for (i=0;i<minCount;i++) {
      QString arg(arguments[i]);
      if (arg[0] == '&')
	arg.remove(0,1);
      context->insertVariableLocally(arg,inputs[i]);
    }
    inputCount -= minCount;
    // Put minCount...inputCount 
    ArrayVector varargin(inputs.mid(minCount,inputCount));
    context->insertVariableLocally(QString("varargin"),
				   CellArrayFromArrayVector(varargin,varargin.size()));
  }
  context->insertVariableLocally(QString("nargout"),Array(double(nargout)));
  try {
    try {
      walker->block(code.tree());
    } catch (InterpreterBreakException& e) {
    } catch (InterpreterContinueException& e) {
    } catch (InterpreterReturnException& e) {
    }
    warningIssued = false;
    if (outputArgCount() != -1) {
      // special case - if nargout == 0, and none of the
      // outputs are predefined, we don't do anything
      bool nonpredefed = true;
      for (int i=0;i<returnVals.size()&&nonpredefed;i++) {
	Array *ptr = context->lookupVariableLocally(returnVals[i]);
	nonpredefed = nonpredefed && (!ptr);
      }
      if ((nargout > 0) || 
	  ((nargout == 0) && (!nonpredefed))) {
	outputs = ArrayVector();
	for (int i=0;i<returnVals.size();i++) outputs.push_back(Array());
	//	outputs = ArrayVector(returnVals.size());
	for (int i=0;i<returnVals.size();i++) {
	  Array *ptr = context->lookupVariableLocally(returnVals[i]);
	  if (!ptr)
	    outputs[i] = EmptyConstructor();
	  else
	    outputs[i] = *ptr;
	  if (!ptr && (i < ((int)nargout)))
	    if (!warningIssued) {
	      walker->warningMessage("one or more outputs not assigned in call (1)");
	      warningIssued = true;
	    }
	}
      }
    } else {
      outputs = ArrayVector();
      for (int i=0;i<nargout;i++) outputs.push_back(Array());
      int explicitCount = returnVals.size() - 1;
      // For each explicit argument (that we have), insert it
      // into the scope.
      for (int i=0;i<explicitCount;i++) {
	Array *ptr = context->lookupVariableLocally(returnVals[i]);
	if (!ptr)
	  outputs[i] = EmptyConstructor();
	else
	  outputs[i] = *ptr;
	if (!ptr  && (i < nargout)) 
	  if (!warningIssued) {
	    walker->warningMessage("one or more outputs not assigned in call (2)");
	    warningIssued = true;
	  }
      }
      // Are there any outputs not yet filled?
      if (nargout > explicitCount) {
	Array varargout, *ptr;
	// Yes, get a pointer to the "vargout" variable that should be defined
	ptr = context->lookupVariableLocally("varargout");
	if (!ptr)
	  throw Exception("The special variable 'varargout' was not defined as expected");
	varargout = *ptr;
	if (varargout.dataClass() != CellArray)
	  throw Exception("The special variable 'varargout' was not defined as a cell-array");
	// Get the data pointer
	const BasicArray<Array> &dp(varargout.constReal<Array>());
	// Get the length
	int varlen = int(varargout.length());
	int toFill = nargout - explicitCount;
	if (toFill > varlen) 
	  throw Exception("Not enough outputs in varargout to satisfy call");
	for (int i=0;i<toFill;i++)
	  outputs[explicitCount+i] = dp[index_t(i+1)];
      }
      // Special case - nargout = 0, only variable outputs from function
      if ((nargout == 0) && (explicitCount == 0)) {
	Array varargout, *ptr;
	// Yes, get a pointer to the "vargout" variable that should be defined
	ptr = context->lookupVariableLocally("varargout");
	if (ptr) {
	  varargout = *ptr;
	  if (varargout.dataClass() != CellArray)
	    throw Exception("The special variable 'varargout' was not defined as a cell-array");
	  if (varargout.length() > 0) {
	    outputs << varargout.constReal<Array>().get(index_t(1));
	  }
	}	
      }
    }
    // Check for arguments that were passed by reference, and 
    // update their values.
    for (int i=0;i<minCount;i++) {
      QString arg(arguments[i]);
      if (arg[0] == '&')
	arg.remove(0,1);
      Array *ptr = context->lookupVariableLocally(arg);
      if (ptr)
	inputs[i] = *ptr;
    }
    // Check the outputs for function pointers
    CaptureFunctionPointers(outputs,walker,this);
    if (capturedFunction && workspace) {
      StringVector workspaceVars(workspace->getCompletions(""));
      for (int i=0;i<workspaceVars.size();i++) {
	Array *ptr = context->lookupVariableLocally(workspaceVars[i]);
	workspace->insertVariable(workspaceVars[i],*ptr);
      }
    }
    context->popScope();
    walker->popDebug();
    return outputs;
  } catch (Exception& e) {
    if (capturedFunction && workspace) {
      StringVector workspaceVars(workspace->getCompletions(""));
      for (int i=0;i<workspaceVars.size();i++) {
	Array *ptr = context->lookupVariableLocally(workspaceVars[i]);
	workspace->insertVariable(workspaceVars[i],*ptr);
      }
    }
    context->popScope();
    walker->popDebug();
    throw;
  }
  catch (InterpreterRetallException& e) {
    if (capturedFunction && workspace) {
      StringVector workspaceVars(workspace->getCompletions(""));
      for (int i=0;i<workspaceVars.size();i++) {
	Array *ptr = context->lookupVariableLocally(workspaceVars[i]);
	workspace->insertVariable(workspaceVars[i],*ptr);
      }
    }
    context->popScope();
    walker->popDebug();
    throw;
  }
}
  
inline QString ReadFileIntoString(QString filename) {
  QFile fp(filename);
  if (!fp.open(QIODevice::ReadOnly)) 
    throw Exception(QString("Unable to open file :") + filename);
  QTextStream io(&fp);
  QString txt(io.readAll());
  if (!txt.endsWith("\n")) txt += "\n";
  return txt;
}

//MFunctionDef* ConvertParseTreeToMFunctionDef(tree t, string fileName) {
//  MFunctionDef *fp = new MFunctionDef;
//  fp->returnVals = IdentifierList(t.first());
//  fp->name = t.second().text();
//  fp->arguments = IdentifierList(t.child(2));
//  fp->code = t.child(3);
//  fp->fileName = fileName;
//  return fp;
//}
//
//MFunctionDef* ConvertParseTreeToMFunctionDefs(treeVector t, 
//					      string fileName) {
//  MFunctionDef* last = NULL;
//  for (int index = t.size()-1;index>=0;index--) {
//    MFunctionDef* rp = ConvertParseTreeToMFunctionDef(t[index],fileName);
//    if (index>0)
//      rp->localFunction = true;
//    else
//      rp->localFunction = false;
//    rp->nextFunction = last;
//    if (last)
//      last->prevFunction = rp;
//    last = rp;
//  }
//  return last;
//}

static void RegisterNested(Tree *t, Interpreter *m_eval, MFunctionDef *parent) {
  if (t->is(TOK_NEST_FUNC)) {
    MFunctionDef *fp = new MFunctionDef;
    fp->localFunction = parent->localFunction;
    fp->nestedFunction = true;
    fp->returnVals = IdentifierList(t->first());
    fp->name = parent->name + "/" + t->second()->text();
    fp->arguments = IdentifierList(t->child(2));
    fp->code = CodeBlock(t->child(3),true); 
    VariableReferencesList(fp->code.tree(),fp->variablesAccessed);
    fp->fileName = parent->fileName;
    // Register any nested functions for the local functions
    m_eval->getContext()->insertFunction(fp,false);
    RegisterNested(fp->code.tree(),m_eval,fp);
  } else
    for (int i=0;i<t->numChildren();i++)
      RegisterNested(t->child(i),m_eval,parent);
}

// Compile the function...
bool MFunctionDef::updateCode(Interpreter *m_eval) {
  if (localFunction) return false;
  if (pcodeFunction) return false;
  if (nestedFunction) return false;
  // First, stat the file to get its time stamp
  QFileInfo filestat(fileName);
  if (!functionCompiled || (filestat.lastModified() != timeStamp)) {
    // Record the time stamp
    timeStamp = filestat.lastModified();
    QFile fp(fileName);
    if (!fp.open(QIODevice::ReadOnly))
      throw Exception(QString("Unable to open file :") + fileName);
    bool commentsOnly = true;
    helpText.clear();
    QTextStream io(&fp);
    QString cp;
    while (!io.atEnd() && commentsOnly) {
      cp = io.readLine();
      while ((cp.size() > 1) && (cp.at(0).isSpace()))
	cp.remove(0,1);
      if (cp == "\n" || cp.isEmpty()) continue;
      if (cp.at(0) != QChar('%'))
	commentsOnly = false;
      else {
	if ((cp.size() > 1) && (!cp.endsWith(QChar('\n'))))
	  helpText.push_back(cp + "\n");
	else
	  helpText.push_back(cp);
      }
    }
    if (helpText.size() == 0)
      helpText.push_back(cp);
    try {
      // Read the file into a string
      QString fileText = ReadFileIntoString(fileName);
      Scanner S(fileText,fileName);
      Parser P(S);
      CodeBlock pcode(P.process());
      if (pcode.tree()->is(TOK_FUNCTION_DEFS)) {
	scriptFlag = false;
	// Get the main function..
	Tree *MainFuncCode = pcode.tree()->first();
	returnVals = IdentifierList(MainFuncCode->first());
	// The name is mangled by the interpreter...  We ignore the
	// name as parsed in the function.
	//	name = MainFuncCode.second().text();
	arguments = IdentifierList(MainFuncCode->child(2));
	code = CodeBlock(MainFuncCode->child(3),true);
	VariableReferencesList(code.tree(),variablesAccessed);
	// Register any nested functions
	RegisterNested(code.tree(),m_eval,this);
	localFunction = false;
	// Process the local functions
	for (int index = 1;index < pcode.tree()->numChildren();index++) {
	  Tree* LocalFuncCode = pcode.tree()->child(index);
	  MFunctionDef *fp = new MFunctionDef;
	  fp->localFunction = true;
	  fp->returnVals = IdentifierList(LocalFuncCode->first());
	  fp->name = name + "/" + LocalFuncCode->second()->text();
	  fp->arguments = IdentifierList(LocalFuncCode->child(2));
	  fp->code = CodeBlock(LocalFuncCode->child(3),true); 
	  VariableReferencesList(fp->code.tree(),fp->variablesAccessed);
	  fp->fileName = fileName;
	  // Register any nested functions for the local functions
	  // local functions are not marked as temporary.  This yields
	  // clutter in the name space, but solves the troublesome
	  // issue of local functions being flushed by the CD command.
	  m_eval->getContext()->insertFunction(fp,false);
	  //	  qDebug() << "Registering " << QString::fromStdString(fp->name);
	  RegisterNested(fp->code.tree(),m_eval,this);
	}
	functionCompiled = true;
      } else {
	scriptFlag = true;
	functionCompiled = true;
	code = CodeBlock(pcode.tree()->first(),true);
      }
    } catch (Exception &e) {
      functionCompiled = false;
      throw;
    }
    return true;
  } 
  return false;
}

static bool StatementTypeNode(Tree* t) {
  return (t->is('=') || t->is(TOK_MULTI) || t->is(TOK_SPECIAL) ||
	  t->is(TOK_FOR) || t->is(TOK_WHILE) || t->is(TOK_IF) ||
	  t->is(TOK_BREAK) || t->is(TOK_CONTINUE) || t->is(TOK_DBSTEP) ||
	  t->is(TOK_RETURN) || t->is(TOK_SWITCH) || t->is(TOK_TRY) || 
	  t->is(TOK_QUIT) || t->is(TOK_RETALL) || t->is(TOK_KEYBOARD) ||
	  t->is(TOK_GLOBAL) || t->is(TOK_PERSISTENT) || t->is(TOK_EXPR));
}

// Find the smallest line number larger than the argument
// if our line number is larger than the target, then we
// 
static void TreeLine(Tree* t, unsigned &bestLine, unsigned lineTarget) {
  if (!t->valid()) return;
  // Nested functions are tracked separately - so that we do not
  // check them for line numbers
  if (t->is(TOK_NEST_FUNC)) return;
  if (StatementTypeNode(t)) {
    unsigned myLine = (t->context() & 0xffff);
    if ((myLine >= lineTarget) && (myLine < bestLine))
      bestLine = myLine;
  }
  for (int i=0;i<t->numChildren();i++)
    TreeLine(t->child(i),bestLine,lineTarget);
}


// Find the closest line number to the requested 
unsigned MFunctionDef::ClosestLine(unsigned line) {
  unsigned bestline;
  bestline = 1000000000;
  TreeLine(code.tree(),bestline,line);
  if (bestline == 1000000000)
    throw Exception(QString("Unable to find a line close to ") + 
		    QString::number(line) + 
		    QString(" in routine ") + name);
  return bestline;
}


BuiltInFunctionDef::BuiltInFunctionDef() {
}

BuiltInFunctionDef::~BuiltInFunctionDef() {
}

int BuiltInFunctionDef::inputArgCount() {
  return argCount;
}

int BuiltInFunctionDef::outputArgCount() {
  return retCount;
}

void BuiltInFunctionDef::printMe(Interpreter *eval) {
  eval->outputMessage(" Function name:" + name);
  eval->outputMessage(" Function class: Built in\n");
  eval->outputMessage(QString(" Return count: %1\n").arg(retCount));
  eval->outputMessage(QString(" Argument count: %1\n").arg(argCount));
}


ArrayVector BuiltInFunctionDef::evaluateFunction(Interpreter *walker, ArrayVector& inputs, 
						 int nargout) {
  ArrayVector outputs;
  walker->pushDebug(name,"built in");
  try {
    outputs = fptr(nargout,inputs);
    walker->popDebug();
    return outputs;
  } catch(Exception& e) {
    walker->popDebug();
    throw;
  }
  catch (InterpreterRetallException& e) {
    walker->popDebug();
    throw;
  }
}

SpecialFunctionDef::SpecialFunctionDef() {
}

SpecialFunctionDef::~SpecialFunctionDef() {
}

ArrayVector SpecialFunctionDef::evaluateFunction(Interpreter *walker, 
						 ArrayVector& inputs, int nargout) {
  ArrayVector outputs;
  walker->pushDebug(name,"built in");
  try {
    outputs = fptr(nargout,inputs,walker);
    walker->popDebug();
    return outputs;
  } catch(Exception& e) {
    walker->popDebug();
    throw;
  }
  catch (InterpreterRetallException& e) {
    walker->popDebug();
    throw;
  }
}

void SpecialFunctionDef::printMe(Interpreter *eval) {
}

FunctionDef::FunctionDef() {
  scriptFlag = false;
  graphicsFunction = false;
  temporaryFlag = false;
  refcount = 0;
}

void FunctionDef::lock() {
  QMutexLocker lockit(&functiondefmutex);
  refcount++;
}

void FunctionDef::unlock() {
  QMutexLocker lockit(&functiondefmutex);
  refcount--;
}

bool FunctionDef::referenced() {
  QMutexLocker lockit(&functiondefmutex);
  return (refcount>0);
}

FunctionDef::~FunctionDef() {
}


ImportedFunctionDef::ImportedFunctionDef(GenericFuncPointer address_arg,
					 StringVector types_arg,
					 StringVector arguments_arg,
					 CodeList sizeChecks,
					 QString retType_arg) {
  address = address_arg;
  types = types_arg;
  arguments = arguments_arg;
  sizeCheckExpressions = sizeChecks;
  retType = retType_arg;
  /*
   * Set up the cif...
   */
  argCount = types_arg.size();
  if (retType == "void") 
    retCount = 0;
  else
    retCount = 1;
}

ImportedFunctionDef::~ImportedFunctionDef() {
}

void ImportedFunctionDef::printMe(Interpreter *) {
}
#if HAVE_AVCALL
static DataClass mapTypeNameToClass(QString name) {
  if (name == "uint8") return UInt8;
  if (name == "int8") return Int8;
  if (name == "uint16") return UInt16;
  if (name == "int16") return Int16;
  if (name == "uint32") return UInt32;
  if (name == "int32") return Int32;
  if (name == "uint64") return UInt64;
  if (name == "int64") return Int64;
  if (name == "float") return Float;
  if (name == "double") return Double;
  if (name == "string") return StringArray;
  if (name == "void") return Int32;
  throw Exception("unrecognized type " + name + " in imported function setup");
}
#endif

ArrayVector ImportedFunctionDef::evaluateFunction(Interpreter *walker,
						  ArrayVector& inputs,
						  int nargout) {
#if HAVE_AVCALL
  walker->pushDebug(name,"imported");
  /**
   * To actually evaluate the function, we have to process each of
   * the arguments and get them into the right form.
   */
  int i;
  for (i=0;i<inputs.size();i++)
    inputs[i] = inputs[i].toClass(mapTypeNameToClass(types[i]));
  /**
   * Next, we count how many of the inputs are to be passed by
   * reference.
   */
  int passByReference = 0;
  for (int j=0;j<inputs.size();j++)
    if ((arguments[j][0] == '&') || (types[j] == "string") ||
	(sizeCheckExpressions[j].tree()->valid()))
      passByReference++;
  /**
   * Next, we check to see if any bounds-checking expressions are
   * active.
   */
  bool boundsCheckActive = false;
  int m=0;
  while (m < inputs.size() && !boundsCheckActive)
    boundsCheckActive = (sizeCheckExpressions[m++].tree()->valid());
  if (boundsCheckActive) {
    /**
     * If the bounds-checking is active, we have to create a 
     * new context, and insert the defined arguments into the
     * context (much as for an M-function call).
     */
    Context* context;
    context = walker->getContext();
    context->pushScope("temp");
    walker->pushDebug(name,"bounds check");
    try {
      for (i=0;i<inputs.size();i++) {
	if (arguments[i][0] != '&') {
	  context->insertVariableLocally(arguments[i],inputs[i]);
	} else {
	  QString nme(arguments[i]);
	  nme.remove(0,1);
	  context->insertVariableLocally(nme,inputs[i]);
	}
      }
      /*
       * Next, evaluate each size check expression
       */
      for (i=0;i<inputs.size();i++) {
	if (sizeCheckExpressions[i].tree()->valid()) {
	  Array ret(walker->expression(sizeCheckExpressions[i].tree()));
	  ret = ret.toClass(Int32);
	  int len = ret.asInteger();
	  if (len != (int)(inputs[i].length())) {
	    throw Exception("array input " + arguments[i] + 
			    " length different from computed bounds" + 
			    " check length");
	  }
	}
      }
    } catch (Exception& e) {
      context->popScope();
      throw;
    } catch (InterpreterRetallException& e) {
      context->popScope();
      throw;
    }
    context->popScope();
    walker->popDebug();
  }
      
  /**
   * Allocate an array of pointers to store for variables passed
   * by reference.
   */
  MemBlock<void*> retBlock(passByReference);
  void **refPointers = &retBlock;
  /** 
   * Allocate an array of value pointers...
   */
  MemBlock<void*> valBlock(inputs.size());
  void **values = &valBlock;
  int ptr = 0;
  for (i=0;i<inputs.size();i++) {
    if (types[i] != "string") {
      if ((arguments[i][0] == '&') || (sizeCheckExpressions[i].tree()->valid())) {
	refPointers[ptr] = inputs[i].getVoidPointer();
	values[i] = &refPointers[ptr];
	ptr++;
      } else {
	values[i] = inputs[i].getVoidPointer();
      }
    } else {
      refPointers[ptr] = strdup(qPrintable(inputs[i].asString()));
      values[i] = &refPointers[ptr];
      ptr++;
    }
  }


  Array retArray;
  // The argument list object
  av_alist alist;
  // Holders for the return values
  uint8 ret_uint8;
  int8 ret_int8;
  uint16 ret_uint16;
  int16 ret_int16;
  uint32 ret_uint32;
  int32 ret_int32;
  float ret_float;
  double ret_double;

  // First pass - based on the return type, call the right version of av_start_*
  if (retType == "uint8") {
    av_start_uchar(alist,address,&ret_uint8);
  } else if (retType == "int8") {      
    av_start_schar(alist,address,&ret_int8);
  } else if (retType == "uint16") {
    av_start_ushort(alist,address,&ret_uint16);
  } else if (retType == "int16") {
    av_start_short(alist,address,&ret_int16);
  } else if (retType == "uint32") {
    av_start_uint(alist,address,&ret_uint32);
  } else if (retType == "int32") {
    av_start_int(alist,address,&ret_int32);
  } else if (retType == "float") {
    av_start_float(alist,address,&ret_float);
  } else if (retType == "double") {
    av_start_double(alist,address,&ret_double);
  } else if (retType == "void") {
    av_start_void(alist,address);
  } else
    throw Exception("Unsupported return type " + retType + " in imported function call");
    
  // Second pass - Loop through the arguments
  for (int i=0;i<types.size();i++) {
    if (arguments[i][0] == '&' || types[i] == "string" ||
	sizeCheckExpressions[i].tree()->valid())
      av_ptr(alist,void*,*((void**)values[i]));
    else {
      if (types[i] == "uint8")
	av_uchar(alist,*((uint8*)values[i]));
      else if (types[i] == "int8")
	av_char(alist,*((int8*)values[i]));
      else if (types[i] == "uint16")
	av_ushort(alist,*((uint16*)values[i]));
      else if (types[i] == "int16")
	av_short(alist,*((int16*)values[i]));
      else if (types[i] == "uint32")
	av_uint(alist,*((uint32*)values[i]));
      else if (types[i] == "int32")
	av_int(alist,*((int32*)values[i]));
      else if (types[i] == "float")
	av_float(alist,*((float*)values[i]));
      else if (types[i] == "double")
	av_double(alist,*((double*)values[i]));
    }
  }

  // Call the function
  av_call(alist);

  // Extract the return value
  if (retType == "uint8") {
    retArray = Array(ret_uint8);
  } else if (retType == "int8") {
    retArray = Array(ret_int8);
  } else if (retType == "uint16") {
    retArray = Array(ret_uint16);
  } else if (retType == "int16") {
    retArray = Array(ret_int16);
  } else if (retType== "uint32") {
    retArray = Array(ret_uint32);
  } else if (retType == "int32") {
    retArray = Array(ret_int32);
  } else if (retType == "float") {
    retArray = Array(ret_float);
  } else if (retType == "double") {
    retArray = Array(ret_double);
  } else {
    retArray = EmptyConstructor();
  }
    
  // Strings that were passed by reference have to be
  // special-cased
  ptr = 0;
  for (i=0;i<inputs.size();i++) {
    if ((arguments[i][0] == '&') || (types[i] == "string")) {
      if ((types[i] == "string") && (arguments[i][0] == '&'))
	inputs[i] = Array(QString((char*) refPointers[ptr]));
      ptr++;
    }
  }
  walker->popDebug();
  return ArrayVector(retArray);
#else
  throw Exception("Support for the import command requires that the avcall library be installed.  FreeMat was compiled without this library being available, and hence imported functions are unavailable. To enable imported commands, please install avcall and recompile FreeMat.");
#endif
}

MexFunctionDef::MexFunctionDef(QString fullpathname) {
  fullname = fullpathname;
  importSuccess = false;
  lib = new DynLib(fullname);
  try {
    address = (mexFuncPtr) lib->GetSymbol("mexFunction");  
    importSuccess = true;
  } catch (Exception& e) {
  }
}

bool MexFunctionDef::LoadSuccessful() {
  return importSuccess;
}
  
MexFunctionDef::~MexFunctionDef() {
}

void MexFunctionDef::printMe(Interpreter *) {
}
  
ArrayVector MexFunctionDef::evaluateFunction(Interpreter *walker, 
					     ArrayVector& inputs, 
					     int nargout) {
  // Convert arguments to mxArray
  mxArray** args = new mxArray*[inputs.size()];
  for (int i=0;i<inputs.size();i++)
    args[i] = MexArrayFromArray(inputs[i]);
  // Allocate output array vector
  int lhsCount = nargout;
  lhsCount = (lhsCount < 1) ? 1 : lhsCount;
  mxArray** rets = new mxArray*[lhsCount];
  try {
    address(lhsCount,rets,inputs.size(),(const mxArray**)args);
  } catch (QString &e) {
    throw Exception(e);
  }
  ArrayVector retvec;
  for (int i=0;i<lhsCount;i++) {
    retvec.push_back(ArrayFromMexArray(rets[i]));
    mxDestroyArray(rets[i]);
  }
  delete[] rets;
  return retvec;
}

