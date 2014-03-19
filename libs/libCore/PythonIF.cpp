#include <Python.h>
#include "/usr/lib64/python2.7/site-packages/numpy/core/include/numpy/arrayobject.h"
#include "FunctionDef.hpp"
#include "Interpreter.hpp"
#include "Array.hpp"
#include "Struct.hpp"
#include "Algorithms.hpp"



FMString getPythonTraceback()
{
    // Python equivilant:
    // import traceback, sys
    // return "".join(traceback.format_exception(sys.exc_type,
    //    sys.exc_value, sys.exc_traceback))

    PyObject *type, *value, *traceback;
    PyObject *tracebackModule;
    FMString chrRetval;

    PyErr_Fetch(&type, &value, &traceback);

    tracebackModule = PyImport_ImportModule("traceback");
    if (tracebackModule != NULL)
    {
        PyObject *tbList, *emptyString, *strRetval;

        tbList = PyObject_CallMethod(
            tracebackModule,
            "format_exception",
            "OOO",
            type,
            value == NULL ? Py_None : value,
            traceback == NULL ? Py_None : traceback);

        emptyString = PyString_FromString("");
        strRetval = PyObject_CallMethod(emptyString, "join",
            "O", tbList);

        chrRetval = FMString(PyString_AsString(strRetval));

        Py_DECREF(tbList);
        Py_DECREF(emptyString);
        Py_DECREF(strRetval);
        Py_DECREF(tracebackModule);
    }
    else
    {
        chrRetval = FMString("Unable to import traceback module.");
    }

    Py_DECREF(type);
    Py_XDECREF(value);
    Py_XDECREF(traceback);

    return chrRetval;
}


Array MakePythonObject(PyObject* p, Interpreter *eval, bool nodelete)
{
  StructArray *sap = new StructArray;
  Py_INCREF(p);
  sap->insert("pointer",Array(reinterpret_cast<uint64>(p)));
  sap->insert("nodelete",Array(nodelete));
  sap->setClassName(FMString("pythonobject"));
  // Make it a handle-semantics class
  StructArray hsap(sap,eval);
  return Array(hsap);
}

PyObject* GetPythonPointer(Array arg)
{
  if (arg.dataClass() != Struct)
    throw Exception("Cannot convert non python-objects to python objects");
  if (!arg.isScalar())
    throw Exception("only scalar instances are supported currently");
  if (arg.className() != "pythonobject")
    throw Exception("Cannot convert non python-objects to python objects");
  StructArray sa = arg.structPtr();
  int pindex = sa.fieldIndex("pointer");
  BasicArray<Array>& val = sa[pindex];
  return reinterpret_cast<PyObject*>(val[1].constRealScalar<uint64>());
}

bool PyIsNumeric(PyObject *t)
{
  return (PyInt_Check(t) || PyFloat_Check(t));
}

bool PyListIsAllNumeric(PyObject *t)
{
  index_t list_size = PyList_Size(t);
  for (index_t i=0;i<list_size;i++)
    if (!PyIsNumeric(PyList_GetItem(t,i))) return false;
  return true;
}

Array PyListAsNumericArray(PyObject* t)
{
  int list_size = PyList_Size(t);
  Array A(Double,NTuple(1,list_size));
  double *a = (double*) A.getVoidPointer();
  for (int i=0;i<list_size;i++)
    {
      if (PyInt_Check(PyList_GetItem(t,i)))
	a[i] = PyInt_AsLong(PyList_GetItem(t,i));
      else
	a[i] = PyFloat_AsDouble(PyList_GetItem(t,i));
    }
  return A;
}

bool PyTupleIsAllNumeric(PyObject *t)
{
  index_t list_size = PyTuple_Size(t);
  for (index_t i=0;i<list_size;i++)
    if (!PyIsNumeric(PyTuple_GetItem(t,i))) return false;
  return true;  
}

Array PyTupleAsNumericArray(PyObject* t)
{
  int list_size = PyTuple_Size(t);
  Array A(Double,NTuple(1,list_size));
  double *a = (double *) A.getVoidPointer();
  for (int i=0;i<list_size;i++)
    {
      if (PyInt_Check(PyTuple_GetItem(t,i)))
	a[i] = PyInt_AsLong(PyTuple_GetItem(t,i));
      else
	a[i] = PyFloat_AsDouble(PyTuple_GetItem(t,i));
    }
  return A;
}

template <class T>
Array NumPyToArraySimple(PyObject *t)
{
  NTuple dims;
  for (int i=0;i<PyArray_NDIM(t);i++)
    dims[i] = PyArray_DIMS(t)[i];
  if (PyArray_NDIM(t) == 1)
    {
      dims[0] = 1;
      dims[1] = PyArray_DIMS(t)[0];
    }
  Array A(GetDataClass(T(0)),dims);
  BasicArray<T> &data = A.real<T>();
  // Copy in the data
  NTuple pos(1,1);
  int copy_count = dims.count();

//  std::cout << "Checks:\r\n";
//  std::cout << "  int:" << PyArray_ISINTEGER(t) << "\r\n";
//  std::cout << "  float:" << PyArray_ISFLOAT(t) << "\r\n";
//  std::cout << "  complex:" << PyArray_ISCOMPLEX(t) << "\r\n";
//  std::cout << "  bool:" << PyArray_ISBOOL(t) << "\r\n";

  int ndim = PyArray_NDIM(t);
  for (int i=0;i<copy_count;i++)
    {
      void *baseptr = PyArray_BYTES(t);
      for (int j=0;j<ndim;j++)
	baseptr += PyArray_STRIDES(t)[j]*((size_t)(pos.get(j)-1));
      data.setNoBoundsCheck(i+1,static_cast<T*>(baseptr)[0]);
      dims.increment(pos);
    }
  return A;
}


// Convert a python object into a FreeMat Array
Array PythonToArray(PyObject* t, Interpreter *eval)
{
  //  std::cout << "Python to array: " << PyString_AsString(PyObject_Repr(t)) << "\r\n";
  if (PyString_Check(t))
    return Array(FMString(PyString_AsString(t)));
  if (PyUnicode_Check(t))
    return Array(FMString(PyString_AsString(PyUnicode_AsASCIIString(t))));
  if (PyInt_Check(t))
    return Array::create(int(PyInt_AsLong(t)));
  if (PyFloat_Check(t))
    return Array::create(double(PyFloat_AsDouble(t)));
  if (PyArray_Check(t))
    {
//      std::cout << "Array returned\r\n";
//      std::cout << "NumDim = " << PyArray_NDIM(t) << "\r\n";
//      std::cout << "Dims = ";
//      for (int i=0;i<PyArray_NDIM(t);i++)
//	std::cout << PyArray_DIMS(t)[i] << " ";
//      std::cout << "\r\n";

      // Convert numpy array to FM array
      // TODO - convert to templated function
      return NumPyToArraySimple<double>(t);
    }
  if (PyList_Check(t))
    {
      if (PyListIsAllNumeric(t))
	return PyListAsNumericArray(t);
      //      std::cout << "List returned\r\n";
      //      std::cout << "Size is " << PyList_Size(t) << "\r\n";
      index_t list_size = PyList_Size(t);
      Array retval(Struct,NTuple(list_size,1));
      retval.structPtr().setClassName(FMString("pythonobject"));
      for (index_t i=1;i<=list_size;i++)
	retval.set(i,MakePythonObject(PyList_GetItem(t,i-1),eval,true));
      return retval;
    }
  if (PyTuple_Check(t))
    {
      if (PyTupleIsAllNumeric(t))
	return PyTupleAsNumericArray(t);
      //      std::cout << "Tuple returned\r\n";
      //      std::cout << "Size is " << PyTuple_Size(t) << "\r\n";
      index_t list_size = PyTuple_Size(t);
      Array retval(Struct,NTuple(list_size,1));
      retval.structPtr().setClassName(FMString("pythonobject"));
      for (index_t i=1;i<=list_size;i++)
	retval.set(i,MakePythonObject(PyTuple_GetItem(t,i-1),eval,true));
      return retval;      
    }
  return MakePythonObject(t,eval,true);
}

// A cell array of strings is mapped to an NDArray of strings also
bool IsArrayCellStrings(const Array &o)
{
  if (o.dataClass() != CellArray) return false;
  const BasicArray<Array> &dp(o.constReal<Array>());
  const Array *cp = dp.constData();
  for (int i=0;i<dp.length();i++)
    if (!(cp[i].isString() || cp[i].isEmpty())) return false;
  return true;
}

PyObject* ArrayToPython(Array o);

// Convert a FreeMat Array (cell str) case
PyObject* CellArrayToPython(const Array &o)
{
  int nd = o.dimensions().lastNotOne();
  npy_intp *dims = new npy_intp[nd];
  for (int i=0;i<nd;i++)
    dims[i] = o.dimensions()[i];
  const BasicArray<Array> & o_data(o.constReal<Array>());
  NPY_TYPES pType = NPY_OBJECT;
  PyObject *t = PyArray_ZEROS(nd,dims,pType,true);
  NTuple pos(1,1);
  int copy_count = o.dimensions().count();
  int ndim = PyArray_NDIM(t);
  for (int i=0;i<copy_count;i++)
    {
      void *baseptr = PyArray_BYTES(t);
      for (int j=0;j<ndim;j++)
	baseptr += PyArray_STRIDES(t)[j]*((size_t)(pos.get(j)-1));
      static_cast<PyObject**>(baseptr)[0] = ArrayToPython(o_data.get(i+1));
      // Py_XINCREF(static_cast<PyObject**>(baseptr)[0]);
      o.dimensions().increment(pos);
    }
  //  Py_XINCREF(t);
  return t;
}

// Convert a FreeMat Array into a python object
PyObject* ArrayToPython(Array o)
{
  if (o.isString())
    return PyString_FromString(qPrintable(o.asString()));
  if (o.isScalar() && (o.dataClass() >= Int8) && (o.dataClass() <= UInt32))
    return PyInt_FromLong(o.asInteger());
  if (o.isScalar() && o.isDouble())
    {
      if (o.asDouble() == int(o.asDouble()))
	return PyInt_FromLong(o.asInteger());
      else
	return PyFloat_FromDouble(o.asDouble());
    }
  if (o.isScalar() && o.dataClass() == Bool)
    return PyBool_FromLong(o.asInteger());
  if (o.dataClass() == CellArray)
    return CellArrayToPython(o);
  if (o.isArray() && o.isVector() && o.isDouble())
    {
      npy_intp *dims = new npy_intp[1];
      dims[0] = o.length();
      BasicArray<double> o_data;
      int mult = 1;
      NPY_TYPES pType = NPY_DOUBLE;
      if (!o.isComplex())
	{
	  o_data = o.constReal<double>();
	}
      else
	{
	  pType = NPY_CDOUBLE;
	  o_data = o.fortran<double>();
	  mult = 2;
	}
      PyObject *array = PyArray_ZEROS(1,dims,pType,false);
      double *data = static_cast<double*>(PyArray_DATA(array));
      memcpy(data,o_data.constData(),o.length()*sizeof(double)*mult);
      //      Py_XINCREF(array);
      return array;
    }
  if (o.isArray() && o.isDouble())
    {
      int nd = o.dimensions().lastNotOne();
      npy_intp *dims = new npy_intp[nd];
      for (int i=0;i<nd;i++)
	dims[i] = o.dimensions()[i];
      BasicArray<double> o_data;
      int mult = 1;
      NPY_TYPES pType = NPY_DOUBLE;
      if (!o.isComplex())
	{
	  o_data = o.constReal<double>();
	}
      else
	{
	  pType = NPY_CDOUBLE;
	  o_data = o.fortran<double>();
	  mult = 2;
	}
      PyObject *array = PyArray_ZEROS(nd,dims,pType,true);
      double *data = static_cast<double*>(PyArray_DATA(array));
      memcpy(data,o_data.constData(),o.dimensions().count()*sizeof(double)*mult);
      //      Py_XINCREF(array);
      return array;
    }      
  PyObject *t = GetPythonPointer(o);
  Py_XINCREF(t); // Because we still need it!
  return t;
}

ArrayVector PyTupleToArrayVector(PyObject *ptr, Interpreter *eval)
{
  ArrayVector ret;
  if (!ptr || (ptr == Py_None)) return ret;
  if (!PyTuple_Check(ptr))
    return ArrayVector(PythonToArray(ptr, eval));
  //  std::cout << "PyTuple has " << PyTuple_Size(ptr) << " elements\r\n";
  for (int i=0;i<PyTuple_Size(ptr);i++)
    {
      //      std::cout << "Processing item " << i << "\r\n";
      //      std::cout << PyString_AsString(PyObject_Repr(PyTuple_GetItem(ptr,i))) << "\r\n";
      ret.push_back(PythonToArray(PyTuple_GetItem(ptr,i),eval));
    }
  return ret;
}

PyObject* ArrayVectorToPyTuple(ArrayVector &a,Interpreter *eval)
{
  PyObject *av = PyTuple_New(a.size());
  for (int i=0;i<a.size();i++)
    PyTuple_SetItem(av, i, ArrayToPython(a[i]));
  return av;
}

class PythonFunctionDef : public FunctionDef {
  PyObject *pFunc;
public:
  PythonFunctionDef(PyObject *ptr)
  {
    pFunc = ptr;
    graphicsFunction = true;
  }
  const FunctionType type() {return FM_PYTHON_FUNCTION;}
  FMString functionName() {return name;}
  FMString detailedName() {return name;}
  int inputArgCount() {return -1;}
  int outputArgCount() {return -1;}
  void printMe(Interpreter *eval) {
    eval->outputMessage(" Function name:" + name);
    eval->outputMessage(" Function class: Python\n");
  }
  virtual ArrayVector evaluateFunc(Interpreter *interp,
				   ArrayVector &args,
				   int nargout,
				   VariableTable *workspace)  {
    PyObject *pArgs = ArrayVectorToPyTuple(args,interp);
    PyObject *pValue;
    pValue = PyObject_CallObject(pFunc, pArgs);
    Py_XDECREF(pArgs);
    if (PyErr_Occurred())
      throw Exception(getPythonTraceback());
    ArrayVector ret = PyTupleToArrayVector(pValue,interp);
    Py_XDECREF(pValue);
    return ret;
  }  
};


//@@Signature
//function @pythonobject:horzcat PythonObjectHorzCatFunction
//input varargin
//output x
ArrayVector PythonObjectHorzCatFunction(int nargout, const ArrayVector& arg) {
  for (int i=0;i<arg.size();i++) 
    if ((!arg[i].isUserClass()) || (arg[i].className() != "pythonobject"))
      throw Exception("Cannot concatenate classes of different types");
  ArrayMatrix t;
  t.push_back(arg);
  return MatrixConstructor(t);
}

//@@Signature
//function @pythonobject:delete PythonObjectDeleteFunction
//input x
//output none
ArrayVector PythonObjectDeleteFunction(int nargout, const ArrayVector& arg) {
  Py_XDECREF(GetPythonPointer(arg[0]));
  return ArrayVector();  
}

//@@Signature
//sfunction @pythonobject:display PythonObjectDispFunction
//input x
//output none
ArrayVector PythonObjectDispFunction(int nargout, const ArrayVector& arg,
				     Interpreter *eval) {
  if (arg.size() == 0) return ArrayVector();
  Array x = arg[0];
  if (x.length() > 1)
    {
      eval->outputMessage(" Array of python objects of size " + x.dimensions().toString() + "\n");
      return ArrayVector();
    }
  PyObject* t = ArrayToPython(x);
  PyObject* t_str = PyObject_Repr(t);
  eval->outputMessage(" " + FMString(PyString_AsString(t_str)) + "\n");
  //  Py_XDECREF(t);
  //  Py_XDECREF(t_str);
  return ArrayVector();
}


static bool python_interpreter_initialized = false;
static PyObject *mainModule = NULL;

const char* catcher_code = 
"import sys\n"
"class StdoutCatcher:\n"
"    def __init__(self):\n"
"        self.data = ''\n"
"    def write(self,stuff):\n"
"        self.data = self.data + stuff\n"
"    def reset(self):\n"
"        self.data = ''\n"
"catcher = StdoutCatcher()\n"
  "sys.stdout = catcher\n";


static void PyInit() {
  if (!python_interpreter_initialized)
    {
      Py_Initialize();
      import_array();
      mainModule = PyImport_AddModule("__main__");
      //      PyRun_SimpleString(catcher_code);
      python_interpreter_initialized = true;
    }  
}

//@@Signature
//sfunction pyimport PyImportFunction
//inputs name
//outputs module
ArrayVector PyImportFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 1)
    throw Exception("pyimport requires one argument - the module to load.");
  PyObject *pName = ArrayToPython(arg[0]);
  PyInit();
  PyObject *pModule = PyImport_Import(pName);
  Py_DECREF(pName);
  if (pModule == NULL)
    {
      PyErr_Print();
      throw Exception("Failed to load module " + arg[0].asString());
    }
  pModule = PyImport_ReloadModule(pModule);
  if (!pModule)
    {
      PyErr_Print();
      throw Exception("Failed to load module " + arg[0].asString());
    }
  PyObject *pDict = PyModule_GetDict(pModule);
  PyObject *key, *value;
  Py_ssize_t pos = 0;
  while (PyDict_Next(pDict, &pos, &key, &value)) {
    if (PyCallable_Check(value))
      {
	//	std::cout << "Key: " << PyString_AsString(key) << "\r\n";
	PythonFunctionDef *pfunc = new PythonFunctionDef(value);
	pfunc->name = FMString(PyString_AsString(key));
	eval->getContext()->insertFunction(pfunc,false);
      }
  }  
  return ArrayVector();
}


//@@Signature
//function pyhook PyHookFunction
//inputs none
//outputs none
ArrayVector PyHookFunction(int nargout, const ArrayVector& arg) {
  if (PyOS_InputHook)
    {
      PyOS_InputHook();
    }
  else
    {
      std::cout << "Input hook not defined!\n";
    }
  return ArrayVector();
}

//@@Signature
//function pyeval PyEvalFunction
//inputs command
//outputs none
ArrayVector PyEvalFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("pyeval requires one argument - the string to execute");
  PyInit();
  //  PyRun_SimpleString("catcher.reset()");
  PyRun_SimpleString(qPrintable(arg[0].asString()));
  //  PyObject *catcher = PyObject_GetAttrString(mainModule,"catcher");
  //  PyObject *output = PyObject_GetAttrString(catcher,"data");
  //  return ArrayVector(PythonToArray(output));
  return ArrayVector();
}

