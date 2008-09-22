#ifndef __FuncPtr_hpp__
#define __FuncPtr_hpp__

#include "Array.hpp"
#include "FunctionDef.hpp"
#include "Interpreter.hpp"

Array FuncPtrConstructor(Interpreter*, FuncPtr);
void CaptureFunctionPointers(ArrayVector &, Interpreter*, MFunctionDef*);
#endif
