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
  fields.push_back("captured");
  fields.push_back("workspace");
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
  values.push_back(Array(typecode));
  QString location = "";
  if (val->type() == FM_M_FUNCTION) {
    MFunctionDef *mptr;
    mptr = (MFunctionDef *) val;
    eval->getContext()->captureMFunction(val);
    location = mptr->fileName;
  }
  values.push_back(Array(location));
  values.push_back(Array(false));
  values.push_back(EmptyConstructor());
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

static ArrayVector FuncPtrCall(int nargout, ArrayVector& args, 
			       FuncPtr fptr, Interpreter* eval, 
			       Array& optr) {
  fptr->updateCode(eval);
  StructArray &rp(optr.structPtr());
  Array workspace(rp["workspace"].get(1));
  StructArray &qp(workspace.structPtr());
  StringVector fieldNames(qp.fieldNames());
  Context* context = eval->getContext();
  VariableTable ws;
  for (int i=0;i<fieldNames.size();i++)
    ws.insertSymbol(fieldNames[i],qp[fieldNames[i]].get(1));
  ArrayVector ret(fptr->evaluateFunction(eval,args,nargout,&ws));
//   for (int i=0;i<fieldNames.size();i++) {
//     Array *ptr = context->lookupVariableLocally(fieldNames[i]);
//     qp["workspace"].set(1,*ptr);
//   }
  return ret;
}

//@@Signature
//sfunction @functionpointer:subsref FuncPtrSubsrefFunction
//input x s
//output varargout
ArrayVector FuncPtrSubsrefFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() == 0) return ArrayVector();
  Array mp(arg[0]);
  FuncPtr fptr = FuncPtrLookup(eval,mp);
  if (!fptr)
    throw Exception("Unable to find a definition for function:" + 
		    LOOKUP(mp,"name").asString());
  ArrayVector fevalArgs;
  if (arg.size() == 2) {
    if (LOOKUP(arg[1],"type").asString() != "()")
      throw Exception("for function pointers, only p(x) is defined");
    const Array &sub(LOOKUP(arg[1],"subs"));
    const BasicArray<Array> &rp(sub.constReal<Array>());
    for (index_t i=1;i<=rp.length();i++)
      fevalArgs.push_back(rp[i]);
  }
  return FuncPtrCall(nargout,fevalArgs,fptr,eval,mp);
}

//@@Signature
//sfunction @functionpointer:feval FuncPtrFevalFunction
//input x varargin
//output varargout
ArrayVector FuncPtrFevalFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {
  if (arg.size() == 0) return ArrayVector();
  Array mp(arg[0]);
  FuncPtr fptr = FuncPtrLookup(eval,mp);
  if (!fptr)
    throw Exception("Unable to find a definition for function:" + 
		    LOOKUP(mp,"name").asString());
  ArrayVector fevalArgs;
  for (int i=1;i<arg.size();i++) 
    fevalArgs.push_back(arg[i]);
  return FuncPtrCall(nargout,fevalArgs,fptr,eval,mp);
}

//@@Signature
//sfunction @functionpointer:subsasgn FuncPtrSubsasgnFunction
//input x s y
//output z
ArrayVector FuncPtrSubsasgnFunction(int nargout, const ArrayVector &arg, Interpreter* eval) {
  if (arg.size() < 3) throw Exception("subsasgn requires three input arguments");
  Array x(arg[0]);
  Array s(arg[1]);
  Array y(arg[2]);
  if (!y.isEmpty() && (!y.isUserClass() || y.className() != "functionpointer"))
    throw Exception("cannot convert arrays to function pointer type");
  if (LOOKUP(s,"type").asString() != "()")
    throw Exception("for function pointers, only p(x) = y is defined");
  const Array &sub(LOOKUP(s,"subs"));
  const BasicArray<Array> &rp(sub.constReal<Array>());
  if (rp.length() != 1) throw Exception("Expression p(x) = y is invalid");
  x.set(rp.get(1),y);
  return ArrayVector(x);
}

static void CaptureFunctionPointer(Array &inp, Interpreter *walker,
				   MFunctionDef *parent) {
  if (!(LOOKUP(inp,"type").asString() == "mfunction")) return;
  FuncPtr ptr = FuncPtrLookup(walker,inp);
  MFunctionDef* mptr = (MFunctionDef*) ptr;
  if (LOOKUP(inp,"captured").toClass(Bool).constRealScalar<bool>()) return;
  Context* context = walker->getContext();
  QString myScope = context->scopeName();
  context->bypassScope(1);
  QString parentScope = context->scopeName();
  context->restoreScope(1);
  if (!Scope::nests(parentScope,myScope)) {
    // We need to capture the variables referenced by the
    // function into the workspace
    StringVector names;
    ArrayVector values;
    for (int i=0;i<mptr->variablesAccessed.size();i++) {
      ArrayReference ptr(context->lookupVariable(mptr->variablesAccessed[i]));
      if (ptr.valid()) {
	names.push_back(mptr->variablesAccessed[i]);
	values.push_back(*ptr);
      }
    }
    inp.structPtr()["workspace"].set(1,StructConstructor(names,values));
  }
}

void CaptureFunctionPointers(ArrayVector& outputs, Interpreter *walker, 
			     MFunctionDef *parent) {
  for (int i=0;i<outputs.size();i++) {
    if (outputs[i].isUserClass() &&
	(outputs[i].className() == "functionpointer")) {
      StructArray &rp(outputs[i].structPtr());
      for (index_t j=1;j<=rp.length();j++) {
	Array fp(outputs[i].get(j));
	CaptureFunctionPointer(fp,walker,parent);
	outputs[i].set(j,fp);
      }
    }
  }
}