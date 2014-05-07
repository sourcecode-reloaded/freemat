#include "Interpreter.hpp"
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <boost/regex.hpp>
#include "Scanner.hpp"
#include "Parser.hpp"
#include "Compiler.hpp"
#include <boost/timer/timer.hpp>
#include "Print.hpp"
#include "Assembler.hpp"
#include "VM.hpp"
#include "NewArray.hpp"
//#include "DenseArray.hpp"
#include "FMObject.hpp"
#include "FMObject2.hpp"
#include "FMObject5.hpp"
#include "FMValue.hpp"
#include "Math.hpp"
#include "Algorithms.hpp"

ArrayVector ZerosFunction(int nargout, const ArrayVector& arg);

// A = 32; B = A; C = B; D = A*B*C;


class TerminalDelegate : public InterpreterDelegate
{
public:
  void outputRawText(FMString txt) {
    std::cout << txt;
  }
  void Flush() {
    std::cout.flush();
  }
  void updateStackView(FMStringList) {
  }
  void CWDChanged(FMString) {
  }
  void SetPrompt(FMString) {
  }
  void doGraphicsCall(Interpreter*, FuncPtr, ArrayVector, int) {
  }
  void QuitSignal() {
  }
  void CrashedSignal() {
  }
  void RefreshBPLists() {
  }
  void IllegalLineOrCurrentPath(FMString, int) {
  }
  void ShowActiveLine(FMString, int) {
  }
  void EnableRepaint() {
  }
  void DisableRepaint() {
  }
  void UpdateStackTrace(FMStringList) {
  }
};


void ImportPrintMessage(const char* t) {
  std::cout << t;
}

void WarningMessage(FMString txt)
{
  std::cout << txt << "\r\n";
}

void  LoadBuiltinFunctionsFreeMat(Context* context,bool guimode);
void  LoadBuiltinFunctionsCore(Context* context,bool guimode);
void  LoadBuiltinFunctionsFN(Context* context,bool guimode);
void InitializeFileSubsystem();

class FFTest : public FMSharedData
{
  std::string _payload;
public:
  FFTest(std::string pyload) : _payload(pyload) {}
  void set(const std::string & a) {_payload = a;}
  std::string get() const {return _payload;}
};

typedef struct {
  FMSharedDataPointer<FFTest> p;
  int num;
} DataTest;

struct data
{
  double a[5];
};

class ShData : public FMSharedData
{
  double a[5];
};


// Provide a surrogate for the shared data concept.
class FMISharedData {
  int _refcount;
public:
  inline FMISharedData() : _refcount(0) {}
  inline void addRef() {++_refcount;}
  inline int release() {return --_refcount;}
  inline void setRef(int cnt) {_refcount = cnt;}
  inline bool unique() const {return _refcount == 1;}
  virtual ~FMISharedData() {}
};

struct ShIData 
{
  int _refcount;
  double a[5];
};


template <class T>
class FMISharedDataPointer {
  T* _baseptr;
public:
  inline FMISharedDataPointer() : _baseptr(0) 
  {
  }
  inline FMISharedDataPointer(T* pData) : _baseptr(pData) 
  {
    ++_baseptr->_refcount;
  }
  inline FMISharedDataPointer(const FMISharedDataPointer<T>& copy) : _baseptr(copy._baseptr) 
  {
    if (_baseptr) ++_baseptr->_refcount;
  }
  inline ~FMISharedDataPointer()
  {
    if (_baseptr && ((--_baseptr->_refcount) == 0))
      {
	delete _baseptr;
      }
  }
  inline FMISharedDataPointer<T>& operator=(const FMISharedDataPointer<T>& sp)
  {
    if (this != &sp)
      {
	if (_baseptr && ((--_baseptr->_refcount) == 0))
	  delete _baseptr;
	_baseptr = sp._baseptr;
	if (_baseptr) ++_baseptr->_refcount;
      }
    return *this;
  }
  inline bool operator!() const
  {
    return _baseptr == 0;
  }
  inline operator bool() const
  {
    return _baseptr != 0;
  }
  inline T* baseptr() const
  {
    return _baseptr;
  }
};

struct Info
{
  int refcnt;
  float data[1000];
};


Info **pool;
int poolsize = 0;

void initPool()
{
  pool = (Info**) malloc(sizeof(Info*)*10);
  for (int i=0;i<10;i++)
    pool[i] = new Info;
  poolsize = 0;
}

Info * allocateInfo()
{
  return pool[poolsize++];
}

void returnInfo(Info * p)
{
  pool[--poolsize] = p;
}

static double copycount = 0;
static double assignmentcount = 0;

class CNArray
{
public:
  float real;
  bool scalar;
  Info *areal;

  inline CNArray() : real(0), scalar(true), areal(0) {}
  inline ~CNArray() {
    if (areal) 
      {
	areal->refcnt--;
	if (areal->refcnt == 0) returnInfo(areal);
      }
  }
  inline CNArray(const CNArray &copy )
  {
    copycount++;
    real = copy.real;
    scalar = copy.scalar;
    areal = copy.areal;
    if (areal) 
      areal->refcnt++;
  }
  inline CNArray& operator=(const CNArray& sp)
  {
    assignmentcount++;
    if (this != &sp)
      {
	if (areal)
	  {
	    areal->refcnt--;
	    if (areal->refcnt == 0) returnInfo(areal);
	  }
	real = sp.real;
	scalar = sp.scalar;
	areal = sp.areal;
	if (areal)
	  areal->refcnt++;
      }
    return *this;
  }  
};


void case1()
{
  for (int i=0;i<1000000;i++)
    {
      Array A = Array(FMString("Hello"));
      for (int j=0;j<100;j++)
	{
	  Array B = A;
	  Array C = B;
	}
    }  
}

double case2()
{
  double total = 0;
  for (int i=0;i<1000000;i++)
    {
      Array A = Array(double(3));
      for (int j=0;j<100;j++)
	{
	  Array B = A;
	  Array C = B;
	  const Array &D = C;
	  total += D.asDouble();
	}
    }
  return total;
}

FMObject kernel1(FMObject &a, FMObject &b)
{
  return FMObject(a.scalarValue()+b.scalarValue());  
}

//FMPODArrayType<double> DoubleType;
FMDoubleType DoubleType;


double case2a()
{
  double total = 0;
  for (int i=0;i<1000000;i++)
    {
      Array A = Array(double(3));
      for (int j=0;j<1;j++)
	{
	  Array B = A;
	  Array C = B;
	  const Array &D = C;
	  total += D.asDouble();
	}
    }
  return total;
}

double case2b()
{
  double total = 0;
  for (int i=0;i<1000000;i++)
    {
      FMObject A = FMObject(3.0);
      for (int j=0;j<100;j++)
	{
	  FMObject B = A;
	  //	  B.scalarValue().real() += 1;
	  FMObject C = A;
	  //	  C.scalarValue().real() -= 1;
	  //	  total += kernel1(B,C).scalarValue().real();
	  total += C.scalarValue().real();
	}
    }
  return total;  
}

double case2d()
{
  double total = 0;
  for (int i=0;i<1000000;i++)
    {
      FMObject A = FMObject(3.0);
      for (int j=0;j<1;j++)
	{
	  FMObject B = A;
	  //	  B.scalarValue().real() += 1;
	  FMObject C = A;
	  //	  C.scalarValue().real() -= 1;
	  //	  total += kernel1(B,C).scalarValue().real();
	  total += C.scalarValue().real();
	}
    }
  return total;
}




FM::ArrayFactory fact(1000);

double case2b_old()
{
  double total = 0;
  for (int i=0;i<1000000;i++)
    {
      FM::Array A = fact.scalar(3);
      for (int j=0;j<100;j++)
	{
	  FM::Array B = A;
	  FM::Array C = B;
	  total += FM::RealScalarValue(C);
	}
    }
  return total;
}

double case2d_old()
{
  double total = 0;
  for (int i=0;i<1000000;i++)
    {
      FM::Array A = fact.scalar(3);
      for (int j=0;j<1;j++)
	{
	  FM::Array B = A;
	  FM::Array C = B;
	  total += FM::RealScalarValue(C);
	}
    }
  return total;
}



void case3()
{
  for (int i=0;i<1000000;i++)
    {
      Array *A = new Array;
      for (int j=0;j<100;j++)
	{
	  Array *B = A;
	  Array *C = B;
	}
      delete A;
    }	  
}

void case4()
{
  for (int i=0;i<1000000;i++)
    {
      boost::shared_ptr<data> A = boost::shared_ptr<data>(new data);
      for (int j=0;j<100;j++)
	{
	  boost::shared_ptr<data> B = A;
	  boost::shared_ptr<data> C = B;
	}
    }
}

double case5()
{
  double total = 0;
  for (int i=0;i<1000000;i++)
    {
      ShIData *p = new ShIData;
      p->a[0] = 3;
      FMISharedDataPointer<ShIData> A = FMISharedDataPointer<ShIData>(p);
      
      for (int j=0;j<100;j++)
	{
	  FMISharedDataPointer<ShIData> B = A;
	  FMISharedDataPointer<ShIData> C = B;
	  if (j == 99)
	    {
	      total += C.baseptr()->a[0];
	    }
	}
    }
  return total;
}

void case6()
{
  FMISharedDataPointer<ShIData> Q = FMISharedDataPointer<ShIData>(new ShIData);
  for (int i=0;i<1000000;i++)
    {
      FMISharedDataPointer<ShIData> A = Q;
      for (int j=0;j<100;j++)
	{
	  FMISharedDataPointer<ShIData> B = A;
	  FMISharedDataPointer<ShIData> C = B;
	}
    }
}

void case7()
{
  for (int i=0;i<1000000;i++)
    {
      FMSharedDataPointer<ShData> A = FMSharedDataPointer<ShData>(new ShData);
      for (int j=0;j<100;j++)
	{
	  FMSharedDataPointer<ShData> B = A;
	  FMSharedDataPointer<ShData> C = B;
	}
    }  
}

int case8()
{
  int cnt = 0;
  for (int i=0;i<1000000;i++)
    {
      CNArray A;
      for (int j=0;j<100;j++)
	{
	  CNArray B = A;
	  B.scalar += j;
	  CNArray C = A;
	  cnt += B.scalar;
	}
    }  
  return cnt;
}

int case9()
{
  int cnt = 1;
  for (int i=0;i<1000000;i++)
    {
      CNArray A;
      A.areal = allocateInfo();
      A.areal->refcnt = 1;
      for (int j=0;j<100;j++)
	{
	  CNArray B = A;
	  cnt += B.areal->refcnt;
	  CNArray C = B;
	  C = A;
	  cnt += C.areal->refcnt;
	}
    }
  return cnt;  
}

typedef FMISharedDataPointer<ShIData> IArray;

typedef boost::shared_ptr<double> BoostArray;

template <class T>
T ADDFUNC(const T& a, const T&b);

template <>
BoostArray ADDFUNC(const BoostArray &a, const BoostArray &b)
{
  double *p = new double(*a + *b);
  return BoostArray(p);
}

template <>
double ADDFUNC(const double &a, const double &b)
{
  return a + b;
}

template <>
float ADDFUNC(const float &a, const float &b)
{
  return a + b;
}

template <>
Array ADDFUNC(const Array &a, const Array &b)
{
  //  return Array(a.asDouble() + b.asDouble());
  return Add(a,b);
}

template <>
FMValue ADDFUNC(const FMValue &a, const FMValue &b)
{
  return FMValue(a.doubleValue() + b.doubleValue());
}

template <>
FMObject ADDFUNC(const FMObject &a, const FMObject &b)
{
  return FMObject(a.scalarValue() + b.scalarValue());
}

template <>
FMObject2 ADDFUNC(const FMObject2 &a, const FMObject2 &b)
{
  return MakeScalar2(ScalarValue(a) + ScalarValue(b));
}

template <>
FMObject3 ADDFUNC(const FMObject3 &a, const FMObject3 &b)
{
  return a.type()->add(a,b);
}

template <>
FM::Array ADDFUNC(const FM::Array &a, const FM::Array &b)
{
  return fact.scalar(FM::RealScalarValue(a) + FM::RealScalarValue(b));
}

template <>
IArray ADDFUNC(const IArray &a, const IArray &b)
{
  ShIData *p = new ShIData;
  p->a[0] = a.baseptr()->a[0] + b.baseptr()->a[0];
  return IArray(p);
}

template <>
CNArray ADDFUNC(const CNArray &a, const CNArray &b)
{
  CNArray ret;
  ret.real = a.real + b.real;
  return ret;
}

template <class T>
double SCALARVALUE(const T& a);

template <>
double SCALARVALUE(const BoostArray &a)
{
  return *a;
}

template <>
double SCALARVALUE(const FMValue &a)
{
  return a.doubleValue();
}

template <>
double SCALARVALUE(const double &a)
{
  return a;
}

template <>
double SCALARVALUE(const float &a)
{
  return a;
}

template <>
double SCALARVALUE(const Array &a)
{
  return a.asDouble();
}

template <>
double SCALARVALUE(const FMObject &a)
{
  return a.scalarValue().real();
}

template <>
double SCALARVALUE(const FMObject2 &a)
{
  return ScalarValue(a);
}

template <>
double SCALARVALUE(const FMObject3 &a)
{
  return static_cast<const double*>(DoubleType.readOnlyData(a))[0];
}

template <>
double SCALARVALUE(const FM::Array &a)
{
  return FM::RealScalarValue(a);
}

template <>
double SCALARVALUE(const CNArray &a)
{
  return a.real;
}

template <>
double SCALARVALUE(const IArray &a)
{
  return a.baseptr()->a[0];
}

template <class T>
double simVM(const T& const_val)
{
  enum opcode
  {
    OP_LOAD_A = 0,
    OP_LOAD_B,
    OP_STORE_C,
    OP_ADD,
    OP_JMP
  };
  int *insn = new int[16];
  insn[0] = OP_LOAD_A;
  insn[1] = OP_LOAD_B;
  insn[2] = OP_ADD;
  insn[3] = OP_STORE_C;
  insn[4] = OP_LOAD_A;
  insn[5] = OP_LOAD_B;
  insn[6] = OP_ADD;
  insn[7] = OP_STORE_C;
  insn[8] = OP_LOAD_A;
  insn[9] = OP_LOAD_B;
  insn[10] = OP_ADD;
  insn[11] = OP_STORE_C;
  insn[12] = OP_JMP;
  std::vector<T> regfile;
  regfile.resize(4);
  int ip = 0;
  for (int j=0;j<100;j++)
    for (int i=0;i<1000000;i++)
      {
	switch(insn[ip])
	  {
	  case OP_LOAD_A:
	    regfile[0] = const_val;
	    ip++;
	    break;
	  case OP_LOAD_B:
	    regfile[1] = const_val;
	    ip++;
	    break;
	  case OP_ADD:
	    regfile[2] = ADDFUNC<T>(regfile[0],regfile[1]);
	    ip++;
	    break;
	  case OP_STORE_C:
	    regfile[3] = regfile[2];
	    ip++;
	    break;
	  case OP_JMP:
	    ip = 0;
	    break;
	  }
      }
  return SCALARVALUE<T>(regfile[3]);
}

void printMatrix(const FMObject3 &a)
{
  int rows = a.rows();
  int cols = a.cols();
  const double *p = static_cast<const double*>(DoubleType.readOnlyData(a));
  for (int i=0;i<rows;i++)
    {
      for (int j=0;j<cols;j++)
	std::cout << p[i+j*rows] << " ";
      std::cout << "\n";
    }
}


int main(int argc, char *argv[])
{


  boost::timer::cpu_timer timer;


  if (0) {
    double p = 1.0;
    DoubleType.makeScalar(&p);
    for (int i=0;i<500;i++)
      {
	timer.start();
	std::cout << "Result = " << simVM<FMObject3>(DoubleType.makeScalar(&p)) << "\n";
	timer.stop();
	std::cout << "simVM <Object3> "  << timer.elapsed().wall/1.0e9 << "\n";
      }
    return 0;
  }

  //  DenseArray<float> *p = new DenseArray<float>(NTuple(5,5),false);

  Scope *m_global = new Scope("global",false);
  m_global->mutexSetup();
  Context *context = new Context(m_global);
  LoadBuiltinFunctionsFreeMat(context,false);
  LoadBuiltinFunctionsCore(context,false);
  LoadBuiltinFunctionsFN(context,false);
  Interpreter *p_eval = new Interpreter(context);
  TerminalDelegate delegate;
  p_eval->setDelegate(&delegate);
  InitializeFileSubsystem();
  // Test a bunch of array copy operations


  if (1){
    // Check out the logic
    double p = 3.0;
    FMObject3 a = DoubleType.makeScalar(&p);
    std::cout << "a = " << SCALARVALUE(a) << "\n";
    FMObject3 b = a;
    FMObject3 c = b;
    std::cout << "b = " << SCALARVALUE(b) << "\n";
    double *pb = static_cast<double*>(DoubleType.readWriteData(b));
    pb[0] = 7.0;
    std::cout << "b = " << SCALARVALUE(b) << "\n";
    std::cout << "a = " << SCALARVALUE(a) << "\n";
    std::cout << "c = " << SCALARVALUE(c) << "\n";
    FMObject3 d = DoubleType.makeMatrix(4,4);
    printMatrix(d);
    double *q = static_cast<double*>(DoubleType.readWriteData(d));
    for (int i=0;i<16;i++) q[i] = double(i);
    printMatrix(d);
    FMObject3 e = DoubleType.sliceColumn(d,3);
    printMatrix(e);
    double *w = static_cast<double*>(DoubleType.readWriteData(e));
    w[3] = 52;
    printMatrix(e);
    printMatrix(d);
  }


  {
    Array p(int(21504));
    Array q(int(4000));
    ArrayVector args;
    args.push_back(p);
    args.push_back(q);
    timer.start();
    ArrayVector zz = ZerosFunction(1,args);
    timer.stop();
  }
  std::cout << "Allocation of large array " << timer.elapsed().wall/1.0e9 << "\n";

  /*
  {
    Array p(int(10000));
    Array q(int(4000));
    ArrayVector args;
    args.push_back(p);
    args.push_back(q);
    timer.start();
    Array P = ZerosFunction(1,args)[0];
    Array Q = ZerosFunction(1,args)[0];
    ArrayVector vcat_args;
    vcat_args.push_back(P);
    vcat_args.push_back(Q);
    Array R = NCat(vcat_args,0);
    timer.stop();
  }
  std::cout << "Allocation & concatenation of large arrays " << timer.elapsed().wall/1.0e9 << "\n";
  */


  timer.start();
  std::cout << "Result = " << simVM<double>(1.0) << "\n";
  timer.stop();
  std::cout << "simVM <double> "  << timer.elapsed().wall/1.0e9 << "\n";

  timer.start();
  std::cout << "Result = " << simVM<float>(1.0) << "\n";
  timer.stop();
  std::cout << "simVM <float> "  << timer.elapsed().wall/1.0e9 << "\n";

  timer.start();
  std::cout << "Result = " << simVM<Array>(Array(1.0)) << "\n";
  timer.stop();
  std::cout << "simVM <Array> "  << timer.elapsed().wall/1.0e9 << "\n";

  timer.start();
  std::cout << "Result = " << simVM<FMObject>(FMObject(1.0)) << "\n";
  timer.stop();
  std::cout << "simVM <Object> "  << timer.elapsed().wall/1.0e9 << "\n";

  timer.start();
  std::cout << "Result = " << simVM<FMObject2>(MakeScalar2(1.0)) << "\n";
  timer.stop();
  std::cout << "simVM <Object2> "  << timer.elapsed().wall/1.0e9 << "\n";

  double kk = 1.0;
  DoubleType.makeScalar(&kk);
  for (int i=0;i<5;i++)
    {
      timer.start();
      std::cout << "Result = " << simVM<FMObject3>(DoubleType.makeScalar(&kk)) << "\n";
      timer.stop();
      std::cout << "simVM <Object3> "  << timer.elapsed().wall/1.0e9 << "\n";
    }

  timer.start();
  std::cout << "Result = " << simVM<FMValue>(FMValue(1.0)) << "\n";
  timer.stop();
  std::cout << "simVM <Value> "  << timer.elapsed().wall/1.0e9 << "\n";

  timer.start();
  std::cout << "Result = " << simVM<FM::Array>(fact.scalar(1.0)) << "\n";
  timer.stop();
  std::cout << "simVM <FM::Array> "  << timer.elapsed().wall/1.0e9 << "\n";

  timer.start();
  double *p = new double;
  p[0] = 1.0;
  std::cout << "Result = " << simVM<BoostArray>(BoostArray(p)) << "\n";
  timer.stop();
  std::cout << "simVM <BoostArray> "  << timer.elapsed().wall/1.0e9 << "\n";

  timer.start();
  CNArray k;
  k.real = 1.0;
  std::cout << "Result = " << simVM<CNArray>(k) << "\n";
  timer.stop();
  std::cout << "simVM <CNArray> "  << timer.elapsed().wall/1.0e9 << "\n";

  timer.start();
  ShIData *g = new ShIData;
  g->a[0] = 1.0;
  std::cout << "Result = " << simVM<IArray>(IArray(g)) << "\n";
  timer.stop();
  std::cout << "simVM <IArray> "  << timer.elapsed().wall/1.0e9 << "\n";

  timer.start();
  case1();
  timer.stop();
  std::cout << "Array copy test: (Array - string) " << timer.elapsed().wall/1.0e9 << "\n";


  timer.start();
  case2();
  timer.stop();
  std::cout << "Array copy test: (Array - scalar) " << timer.elapsed().wall/1.0e9 << "\n";

  timer.start();
  case2a();
  timer.stop();
  std::cout << "Array copy test: (Array - scalar - alloc emph) " << timer.elapsed().wall/1.0e9 << "\n";

  timer.start();
  case2b();
  timer.stop();
  std::cout << "FMArray copy test: (FMObject - scalar) " << timer.elapsed().wall/1.0e9 << "\n";
  // std::cout << "Allocate count : " << FM::_allocate_count << "\n";
  // std::cout << "BuildScalar count : " << FM::_buildscalar_count << "\n";
  // std::cout << "Duplicate count : " << FM::_duplicate_count << "\n";
  // std::cout << "Copy count : " << FM::_copy_count << "\n";

  timer.start();
  case2d();
  timer.stop();
  std::cout << "FMArray copy test: (FMObject - scalar - alloc emph) " << timer.elapsed().wall/1.0e9 << "\n";


  timer.start();
  case2b_old();
  timer.stop();
  std::cout << "FMArray copy test: (FMArray - scalar) " << timer.elapsed().wall/1.0e9 << "\n";
  // std::cout << "Allocate count : " << FM::_allocate_count << "\n";
  // std::cout << "BuildScalar count : " << FM::_buildscalar_count << "\n";
  // std::cout << "Duplicate count : " << FM::_duplicate_count << "\n";
  // std::cout << "Copy count : " << FM::_copy_count << "\n";

  timer.start();
  case2d_old();
  timer.stop();
  std::cout << "FMArray copy test: (FMArray - scalar - alloc emph) " << timer.elapsed().wall/1.0e9 << "\n";

  timer.start();
  //  case3();
  timer.stop();
  std::cout << "Array copy test: (Pointer - leaky) " << timer.elapsed().wall/1.0e9 << "\n";

  timer.start();
  case4();
  timer.stop();

  std::cout << "Array copy test: (Boost - threads) " << timer.elapsed().wall/1.0e9 << "\n";


  timer.start();
  case5();
  timer.stop();

  std::cout << "Array copy test: (Inlined FMShared - no virtual funcs) " << timer.elapsed().wall/1.0e9 << "\n";

  timer.start();
  case6();
  timer.stop();

  std::cout << "Array copy test: (Inlined FMShared - no new in loop) " << timer.elapsed().wall/1.0e9 << "\n";

  timer.start();
  case7();
  timer.stop();

  std::cout << "Array copy test: (Classic FMShared) " << timer.elapsed().wall/1.0e9 << "\n";



  struct NArray
  {
    float real;
    float imag;
    bool scalar;
    int cnt;
  };

  timer.start();
  for (int i=0;i<1000000;i++)
    {
      NArray A;
      for (int j=0;j<100;j++)
	{
	  NArray B = A;
	  A.cnt++;
	  NArray C = A;
	  A.cnt++;
	  A.cnt--;
	  A.cnt--;
	}
    }
  timer.stop();

  std::cout << "Array copy test: (Struct) " << timer.elapsed().wall/1.0e9 << "\n";

  initPool();

  timer.start();
  case8();
  timer.stop();

  std::cout << "Array copy test: (Class - scalar) " << timer.elapsed().wall/1.0e9 << "\n";

  {
  timer.start();
  case9();
  timer.stop();

  }
  std::cout << "Array copy test: (Class - heap) " << timer.elapsed().wall/1.0e9 << "\n";

  std::cout << "Assignment count " << assignmentcount << "\n";
  std::cout << "copy count " << copycount << "\n";
  

  /*
  timer.start();
  for (int i=0;i<1000000;i++)
    {
      Info *p = new Info;
      p->refcnt = 1; // Force allocation
      delete p;
    }
  timer.stop();

  std::cout << "Array copy test: (heap - only) " << timer.elapsed().wall/1.0e9 << "\n";
  */

  return 1;
#if 0

  // Create the VM
  VM vm;
  // Need to populate functions here...
  while (1)
    {
      char *p = readline("--> ");
      if (p && *p)
	add_history(p);
      //      p_eval->evaluateString(FMString(p)+"\n",false);
      bool success;
      //FMString body = ReadFileIntoString(p, success);
      FMString body(p);
      body += "\n\n";
      try {
	Scanner S(body,"");
	Parser P(S);
	Tree b = P.process();
	b.print();
	boost::timer::cpu_timer timer;
	timer.start();
	Compiler C;
	C.compile(b);
	timer.stop();
	std::cout << "Compiler time " << timer.elapsed().wall/1.0e9 << "\n";
	//	C.dump();
	timer.start();
	Assembler A(C.module()->_main);
	A.run();
	timer.stop();
	std::cout << "Assembler time " << timer.elapsed().wall/1.0e9 << "\n";
	Array p = A.codeObject();
	Disassemble(p);
	timer.start();
	vm.executeScript(p);
	timer.stop();
	std::cout << "Execution time " << timer.elapsed().wall/1.0e9 << "\n";
	//A.dump();
      } catch (Exception &e) {
	std::cout << "Caught exception " << e.msg() << "\n";
      }
      
      /*
      VM V;
      try
	{
	  // boost::timer::cpu_timer timer;
	  // timer.start();
	  // for (int i=0;i<1000000;++i)
	  V.executeBlock(C.code());
	  // std::cout << "elapsed time " << timer.elapsed().wall/1.0e9;
	}
      catch (Exception & e)
	{
	  std::cout << "Caught exception " << e.msg() << "\n";
	}

      free(p);
      V.dump();
      */
    }
  return 0;
#endif
}
