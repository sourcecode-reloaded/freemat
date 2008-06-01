#include "Interpreter.hpp"
#include "Array.hpp"

//!
//@Module JITCONTROL Control the Just In Time Compiler
//@@Section FREEMAT
//@@Usage
//The @|jitcontrol| functionality in FreeMat allows you to control
//the use of the Just In Time (JIT) compiler.  
//@@Signature
//sfunction jitcontrol JITControlFunction
//inputs flag
//outputs flag
//!
ArrayVector JITControlFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 1) {
    if (eval->JITControl())
      return SingleArrayVector(Array(QString("on")));
    else
      return SingleArrayVector(Array(QString("off")));
  } else {
    if (!arg[0].isString())
      throw Exception("jitcontrol function takes only a single, string argument");
    QString txt = arg[0].asString().toUpper();
    if (txt == "ON")
      eval->setJITControl(true);
    else if (txt == "OFF")
      eval->setJITControl(false);
    else
      throw Exception("jitcontrol function argument needs to be 'on/off'");
  }
  return ArrayVector();
}

//!
//@Module DBAUTO Control Dbauto Functionality
//@@Section DEBUG
//@@Usage
//The dbauto functionality in FreeMat allows you to debug your
//FreeMat programs.  When @|dbauto| is @|on|, then any error
//that occurs while the program is running causes FreeMat to 
//stop execution at that point and return you to the command line
//(just as if you had placed a @|keyboard| command there).  You can
//then examine variables, modify them, and resume execution using
//@|return|.  Alternately, you can exit out of all running routines
//via a @|retall| statement.  Note that errors that occur inside of
//@|try|/@|catch| blocks do not (by design) cause auto breakpoints.  The
//@|dbauto| function toggles the dbauto state of FreeMat.  The
//syntax for its use is
//@[
//   dbauto(state)
//@]
//where @|state| is either
//@[
//   dbauto('on')
//@]
//to activate dbauto, or
//@[
//   dbauto('off')
//@]
//to deactivate dbauto.  Alternately, you can use FreeMat's string-syntax
//equivalence and enter
//@[
//   dbauto on
//@]
//or 
//@[
//   dbauto off
//@]
//to turn dbauto on or off (respectively).  Entering @|dbauto| with no arguments
//returns the current state (either 'on' or 'off').
//@@Signature
//sfunction dbauto DbAutoFunction
//inputs flag
//outputs flag
//!
ArrayVector DbAutoFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 1) {
    if (eval->AutoStop()) 
      return SingleArrayVector(Array(QString("on")));
    else 
      return SingleArrayVector(Array(QString("off")));
  } else {
    if (!arg[0].isString())
      throw Exception("dbauto function takes only a single, string argument");
    QString txt = arg[0].asString().toUpper();
    if (txt == "ON")
      eval->setAutoStop(true);
    else if (txt == "OFF")
      eval->setAutoStop(false);
    else
      throw Exception("dbauto function argument needs to be 'on/off'");
  }
  return ArrayVector();
}


