#include "Array.hpp"
#include "Interpreter.hpp"

//!
//@Module VERSION The Current Version Number
//@@Section FREEMAT
//@@Usage
//The @|version| function returns the current version number for
//FreeMat (as a string).  The general syntax for its use is
//@[
//    v = version
//@]
//@@Example
//The current version of FreeMat is
//@<
//version
//@>
//@@Signature
//function  version VersionFunction
//inputs none
//outputs verstring
//!
ArrayVector VersionFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(QString(VERSION)));
}


//!
//@Module VERSTRING The Current Version String
//@@Section FREEMAT
//@@Usage
//The @|verstring| function returns the current version string for
//FreeMat.  The general syntax for its use is
//@[
//    version = verstring
//@]
//@@Example
//The current version of FreeMat is
//@<
//verstring
//@>
//@@Signature
//function verstring VerStringFunction
//inputs none
//output verstring
//!
ArrayVector VerStringFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(Interpreter::getVersionString()));
}
