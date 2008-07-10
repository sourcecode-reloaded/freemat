#include "Array.hpp"
#include "Interpreter.hpp"
#include "PathTool.hpp"
#include <QSettings>

//!
//@Module GETPATH Get Current Search Path
//@@Section OS
//@@Usage
//Returns a @|string| containing the current FreeMat search path.  The general syntax for
//its use is
//@[
//  y = getpath
//@]
//The delimiter between the paths depends on the system being used.  For Win32, the
//delimiter is a semicolon.  For all other systems, the delimiter is a colon.
//
//@@Example
//The @|getpath| function is straightforward.
//@<
//getpath
//@>
//@@Signature
//function getpath GetPathFunction
//inputs none
//outputs y
//!
ArrayVector GetPathFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  return ArrayVector(Array(eval->getPath()));
}

//!
//@Module SETPATH Set Current Search Path
//@@Section OS
//@@Usage
//Changes the current FreeMat search path.  The general syntax for
//its use is
//@[
//  setpath(y)
//@]
//where @|y| is a @|string| containing a delimited list of directories
//to be searched for M files and libraries.  
//The delimiter between the paths depends on the system being used.  For Win32, the
//delimiter is a semicolon.  For all other systems, the delimiter is a colon.
//
//@Example
//The @|setpath| function is straightforward.
//@<
//getpath
//setpath('/usr/local/FreeMat/MFiles:/localhome/basu/MFiles')
//getpath
//@>
//@@Signature
//sfunction setpath SetPathFunction
//inputs y
//outputs none
//!
ArrayVector SetPathFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() != 1)
    throw Exception("setpath function requires exactly one string argument");
  eval->setPath(arg[0].asString());
  return ArrayVector();
}

//!
//@Module PATHTOOL Open Path Setting Tool
//@@Section FREEMAT
//@@Usage
//Brings up the pathtool dialog.  The @|pathtool| function takes
//no arguments:
//@[
//  pathtool
//@]
//@@Signature
//sgfunction pathtool PathToolFunction
//inputs none
//outputs none
//!
ArrayVector PathToolFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  PathTool p;
  p.exec();
  QSettings settings("FreeMat","FreeMat");
  QStringList userPath = settings.value("interpreter/path").toStringList();
  eval->setUserPath(userPath);
  eval->rescanPath();
  return ArrayVector();
}
