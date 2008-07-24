#include "Array.hpp"
#include "Struct.hpp"
#include "Interpreter.hpp"
#include "Algorithms.hpp"

#define LOOKUP(x,field) x.constStructPtr()[field].get(1)

// Suppose we have: func1/func2, filename = path1/func1.m
// And we have locally: func1/func2, filename = path2/func1.m
// If we lookup func1/func2, and compare filename to our location indicator
// we can easily spot that we do not have the correct definition for
// the function in question.  But we cannot resurrect the function.  This
// is because if we execute the function in question, it will overwrite
// functions that are locally defined. 

static FuncPtr FuncPtrLookup(Interpreter *eval, Array ptr) {
  if ((!ptr.isUserClass()) || (ptr.className() != "functionpointer"))
    throw Exception("expected function pointer here");
  const StructArray &rp(ptr.constStructPtr());
  QString name = LOOKUP(ptr,"name").asString();
  QString type = LOOKUP(ptr,"type").asString();
  if (type == "builtin") 
    return eval->getContext()->lookupBuiltinFunction(name);
  if (type == "mfunction") {
    // Retrieve it from the cache of 
    QString filename = LOOKUP(ptr,"location").asString();
    FuncPtr val = eval->getContext()->lookupCapturedMFunction(name,filename);
    if (!val)
      throw Exception("Function pointed to by function pointer does not exist!");
    return val;
  }
  throw Exception("Not yet finished");
}

Array FuncPtrConstructor(Interpreter *eval, FuncPtr val) {
  StringVector fields;
  fields.push_back("name");
  fields.push_back("type");
  fields.push_back("location");
  ArrayVector values;
  values.push_back(Array(val->name));
  QString typecode;
  switch (val->type()) {
  default:
    typecode = "unknown";
    break;
  case FM_M_FUNCTION:
    typecode = "mfunction";
    break;
  case FM_BUILT_IN_FUNCTION:
  case FM_SPECIAL_FUNCTION:
    typecode = "builtin";
    break;
  case FM_IMPORTED_FUNCTION:
    typecode = "import";
    throw Exception("Cannot use function pointers on imported functions");
    break;
  }
  values.push_back(typecode);
  QString location = "";
  if (val->type() == FM_M_FUNCTION) {
    MFunctionDef *mptr;
    mptr = (MFunctionDef *) val;
    eval->getContext()->captureMFunction(val);
    location = mptr->fileName;
  }
  values.push_back(Array(location));
  Array ret(StructConstructor(fields,values));
  ret.structPtr().setClassPath(StringVector() << "functionpointer");
  return ret;
}

//@@Signature
//sfunction @functionpointer:display FuncPtrDispFunction
//input x
//output none
ArrayVector FuncPtrDispFunction(int nargout, const ArrayVector& arg, 
				Interpreter *eval) {
  for (int i=0;i<arg.size();i++) 
    eval->outputMessage(" @" + LOOKUP(arg[i],"name").asString() + "\n");
  return ArrayVector();
}

ArrayVector FevalFunction(int nargout, const ArrayVector& arg, Interpreter* eval);

//@@Signature
//function @functionpointer:horzcat FuncPtrHorzCatFunction
//input varargin
//output x
ArrayVector FuncPtrHorzCatFunction(int nargout, const ArrayVector& arg) {
  for (int i=0;i<arg.size();i++) 
    if ((!arg[i].isUserClass()) || (arg[i].className() != "functionpointer"))
      throw Exception("Cannot concatenate classes of different types");
  ArrayMatrix t;
  t.push_back(arg);
  return MatrixConstructor(t);
}

//@@Signature
//sfunction @functionpointer:subsref FuncPtrSubsrefFunction
//input x s
//output varargout
ArrayVector FuncPtrSubsrefFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() == 0) return ArrayVector();
  FuncPtr fptr = FuncPtrLookup(eval,arg[0]);
  if (!fptr)
    throw Exception("Unable to find a definition for function:" + 
		    LOOKUP(arg[0],"name").asString());
  ArrayVector fevalArgs;
  if (arg.size() == 2) {
    if (LOOKUP(arg[1],"type").asString() != "()")
      throw Exception("for function pointers, only p(x) is defined");
    const BasicArray<Array> &rp(LOOKUP(arg[1],"subs").constReal<Array>());
    for (index_t i=1;i<=rp.length();i++)
      fevalArgs.push_back(rp[i]);
  }
  fptr->updateCode(eval);
  return (fptr->evaluateFunction(eval,fevalArgs,nargout));
}
