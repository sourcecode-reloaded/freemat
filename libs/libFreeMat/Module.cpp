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

#include "FMLib.hpp"
#include "Module.hpp"
#include "FunctionDef.hpp"
#include "Interpreter.hpp"
#include "Exception.hpp"
#include "Context.hpp"
#include "Parser.hpp"
#include "PathSearch.hpp"

FM::SymbolTable<FMLibrary*> libPointers;
StringVector DynamicFunctions;

void ClearLibs(Interpreter* eval) {
  StringVector libnames(libPointers.getCompletions(""));
  for (int i=0;i<libnames.size();i++) {
    FMLibrary **ptr = libPointers.findSymbol(libnames[i]);
    libPointers.deleteSymbol(libnames[i]);
    delete *ptr;
  }

  //Warning: DynamicFunctions may not contain the same name list
  //of dynamic functions in current Context in case of using thread.
  for (int i=0;i<DynamicFunctions.size();i++) {
    eval->getContext()->deleteFunction(DynamicFunctions[i]);
  }
  DynamicFunctions.clear();
}

//@@Signature
//sfunction loadlib LoadLibFunction
//inputs libfile symbolname functionname nargin nargout
//outputs none
//DOCBLOCK external_loadlib

ArrayVector LoadLibFunction(int c_nargout,const ArrayVector& narg,
			    Interpreter* eval) {
  FMString libfile;
  FMString symbolName;
  FMString funcName;
  int nargin;
  int nargout;
  ArrayVector arg(narg);

  if (arg.size() < 2) 
    throw Exception("Must supply at least the library file and symbol name");
  libfile = arg[0].asString();
  symbolName = arg[1].asString();
  if (arg.size() < 5)
    nargout = 0;
  else
    nargout = (int) arg[4].asInteger();
  if (arg.size() < 4)
    nargin = 0;
  else
    nargin = (int) arg[3].asInteger();
  if (arg.size() < 3)
    funcName = symbolName;
  else
    funcName = arg[2].asString();
  void *func;
  FMLibrary *lib = new FMLibrary(libfile);
  func = lib->resolve(symbolName);
  BuiltInFunctionDef *fdef = new BuiltInFunctionDef;
  fdef->retCount = nargout;
  fdef->argCount = nargin;
  fdef->name = funcName;
  fdef->fptr = (BuiltInFuncPtr) func;
  eval->getContext()->insertFunction(fdef,false);
  DynamicFunctions.push_back(fdef->name);
  return ArrayVector();
}
  
void skipWS(const char* &cp) {
  while (*cp == ' ' || *cp == '\t' || *cp == '\n')
    cp++;
}
  
const char* matchTest(const char* &cp, const char* tmplate) {
  if (strncmp(cp,tmplate,strlen(tmplate)) == 0) {
    cp += strlen(tmplate);
    return tmplate;
  } else 
    return NULL;
}

const char* parseTypeName(const char* &cp) {
  const char* rp;
  skipWS(cp);
  rp = matchTest(cp,"int8");
  if (rp) return rp;
  rp = matchTest(cp,"uint8");
  if (rp) return rp;
  rp = matchTest(cp,"int16");
  if (rp) return rp;
  rp = matchTest(cp,"uint16");
  if (rp) return rp;
  rp = matchTest(cp,"int32");
  if (rp) return rp;
  rp = matchTest(cp,"uint32");
  if (rp) return rp;
  rp = matchTest(cp,"float");
  if (rp) return rp;
  rp = matchTest(cp,"complex");
  if (rp) return rp;
  rp = matchTest(cp,"double");
  if (rp) return rp;
  rp = matchTest(cp,"dcomplex");
  if (rp) return rp;
  rp = matchTest(cp,"string");
  if (rp) return rp;
  return NULL;
}

char* parseArgumentName(const char* &cp) {
  bool byRef;
  const char *rp;
  char *op;
  int identLength;
  skipWS(cp);
  byRef = false;
  if (*cp == '&') {
    byRef = true;
    cp++;
  }
  skipWS(cp);
  if (!isalpha(*cp))
    throw Exception(FMString("malformed import function prototype") + 
		    " - error starting at " + cp);
  rp = cp;
  cp++;
  while (isalnum(*cp) || (*cp == '_'))
    cp++; 
  identLength = (cp - rp);
  if (byRef) {
    op = (char*) malloc(identLength+2);
    op[0] = '&';
    op[identLength+1] = 0;
    memcpy(op+1,rp,identLength);
  } else {
    op = (char*) malloc(identLength+1);
    op[identLength] = 0;
    memcpy(op,rp,identLength);
  }
  return op;
}

char* parseBoundsCheck(const char* &cp) {
  int bracketDepth;
  if (*cp != '[') return NULL;
  cp++;
  bracketDepth = 1;
  const char *rp;
  rp = cp;
  while ((bracketDepth > 0) && (*cp != 0)) {
    if (*cp == '[') bracketDepth++;
    if (*cp == ']') bracketDepth--;
    cp++;
  }
  if (bracketDepth > 0)
    throw Exception(FMString("malformed bounds check - error starting at ") + 
		    cp);
  char *op;
  int bcLength;
  bcLength = (cp - rp);
  op = (char*) malloc(bcLength+1);
  memcpy(op,rp,bcLength-1);
  op[bcLength-1] = '\n';
  op[bcLength] = 0;
  return op;
}

//@@Signature
//sfunction import ImportFunction
//inputs libraryname symbol function returntype arguments
//outputs none 
//DOCBLOCK external_import

static inline bool issep(FMChar t) {
  return ((t=='/') || (t=='\\'));
}

// This is defined in MainApp.cpp
void ImportPrintMessage(const char* t);

typedef void (*strfunc)(const char*);
typedef void (*handler)(strfunc);

void InitializeIOHandlers(FMLibrary *lib) {
  void* func;
  try {
    func = lib->resolve("freemat_io_handler");
  } catch (Exception&) {
    return;
  }
  if (!func) return;
  handler h_func = (handler) func;
  h_func(ImportPrintMessage);
}

ArrayVector ImportFunction(int nargout, const ArrayVector& arg, 
			   Interpreter* eval)  {
  FMString libfile;
  FMString symbolname;
  FMString funcname;
  FMString rettype;
  FMString arglist;

  PathSearcher psearch(eval->getPath());

  FMString libfullpath;

  if (arg.size() < 5)
    throw Exception(FMString("import requires 5 arguments:") + 
		    "library name, symbol name, imported function name" +
		    "return type, argument list");
  libfile = arg[0].asString();
  libfullpath = psearch.ResolvePath(libfile);
  
  if( libfullpath.size() == 0 )
	  throw Exception( "unable to find file " + libfile );

  FMString current(FMDir::currentPath());
  // Prepend the current working directory... ugly, but necessary
#ifdef WIN32
  if (!(issep(libfullpath[0]) || ((libfullpath[1] == ':') && 
				  issep(libfullpath[2]))))
    libfullpath = current + "/" + libfullpath;
#else
  if (!issep(libfullpath[0]))
    libfullpath = current + "/" + libfullpath;
#endif
  symbolname = arg[1].asString();
  funcname = arg[2].asString();
  FuncPtr val;
  if (eval->getContext()->lookupFunction(funcname, val))
      return ArrayVector(); //don't import the same function again
  rettype = arg[3].asString();
  arglist = arg[4].asString();
  void *func;
  FMLibrary *lib, **ptr;
  ptr = libPointers.findSymbol(libfullpath);
  if (!ptr) {
    lib = new FMLibrary(libfullpath);
    libPointers.insertSymbol(libfullpath,lib);
    InitializeIOHandlers(lib);
  } else
    lib = *ptr;
  func = lib->resolve(symbolname);
  StringVector types;
  StringVector arguments;
  TreeList checks;
  /**
   * Parse the arglist...
   */
  const char *cp;
  cp = strdup(qPrintable(arglist));

  while (*cp != 0) {
    /**
     * Parse off the typename
     */
    const char *tn;
    tn = parseTypeName(cp);
    if (tn == NULL) 
      throw Exception(FMString("illegal syntax in function") + 
		      FMString(" prototype (argument list) - ") + 
		      FMString("expecting a valid type name"));
    types.push_back(tn);
    char *bc = parseBoundsCheck(cp);
    if (bc != NULL) {
      checks.push_back(ParseExpressionString(bc));
      free(bc);
    } else
      checks.push_back(Tree());
    char *ar;
    ar = parseArgumentName(cp);
    arguments.push_back(ar);
    free(ar);
    if (*cp != 0) {
      if (*cp != ',') throw Exception("expecting a ',' between arugments");
      cp++;
    }
  }
  ImportedFunctionDef *fptr = new ImportedFunctionDef((void (*)())func,
						      types,
						      arguments,
						      checks,
						      rettype,
						      funcname);
  fptr->name = funcname;
  eval->getContext()->insertFunction(fptr,false);
  if (!DynamicFunctions.contains(fptr->name))
      DynamicFunctions.push_back(fptr->name);
  return ArrayVector();
}
  
//@@Signature
//function blaslib BlaslibFunction
//inputs varargin
//outputs none
//DOCBLOCK freemat_blaslib
ArrayVector BlaslibFunction(int nargout, const ArrayVector& arg) {
  throw Exception("BLAS library is loaded statically.");
return ArrayVector();
}
