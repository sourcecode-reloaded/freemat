#include <boost/timer/timer.hpp>
#include <readline/readline.h>
#include <readline/history.h>
#include "Object.hpp"
#include "DoubleType.hpp"
#include "Scanner.hpp"
#include "Parser.hpp"
#include "BaseTypes.hpp"
#include "Compiler.hpp"
#include "Assembler.hpp"

// What is the strategy for get?
//
// get(ObjectVector)
//
// Option 1. Convert all arguments to index arrays.
//   Each Type object has a "asIndex" function.
//   Something like:
//      Object asIndex(const Object &a, const Tuple &dims)
//  The index type object can then be used to index the output.
//  Need to make a decision about slicing.
//

using namespace FM;

BaseTypes base;

Object ADDFUNC(const Object &a, const Object &b)
{
  return a.type()->add(a,b);
}

double SCALARVALUE(const Object &a)
{
  return static_cast<const double*>(base._double->readOnlyData(a))[0];
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
	    regfile[2] = ADDFUNC(regfile[0],regfile[1]);
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
  return SCALARVALUE(regfile[3]);
}


void printMatrix(const Object &a)
{
  int rows = a.rows();
  int cols = a.cols();
  const double *p = static_cast<DoubleType*>(a.type())->readOnlyData(a);
  for (int i=0;i<rows;i++)
    {
      for (int j=0;j<cols;j++)
	std::cout << p[i+j*rows] << " ";
      std::cout << "\n";
    }
}


int main(int argc, char *argv[])
{

  assert(sizeof(Complex<char>) == 2*sizeof(char));

  BaseTypes *mytype = new BaseTypes;

  Object d = mytype->_double->makeMatrix(4,4);
  double *dp = mytype->_double->readWriteData(d);
  for (int i=0;i<16;i++) dp[i] = i;
  printMatrix(d);

  Object r = mytype->_double->makeMatrix(3,1);
  double *rp = mytype->_double->readWriteData(r);
  for (int i=0;i<3;i++) rp[i] = i+2;

  printMatrix(r);

  Object nd = mytype->_list->makeMatrix(2,1);
  Object *ndp = mytype->_list->readWriteData(nd);
  ndp[0] = mytype->_int32->makeScalar(0); // For Paren indexing  
  Object p = mytype->_list->makeMatrix(2,1);
  Object *pp = mytype->_list->readWriteData(p);
  pp[0] = r;
  pp[1] = r;
  ndp[1] = p; // For a(r,r)
  
  printMatrix(d.type()->get(d,nd));

  // return 0;

  boost::timer::cpu_timer t;
  
  for (int reps=0;reps<0;reps++)
    {
      t.start();
      for (int i=0;i<1000000;i++)
	{
	  Object nd = mytype->_list->makeMatrix(2,1);
	  Object *ndp = mytype->_list->readWriteData(nd);
	  ndp[0] = mytype->_int32->makeScalar(0); // For Paren indexing  
	  Object p = mytype->_list->makeMatrix(2,1);
	  Object *pp = mytype->_list->readWriteData(p);
	  pp[0] = r;
	  pp[1] = r;
	  ndp[1] = p; // For a(r,r)
	  Object b = d.type()->get(d,nd);
	}
      t.stop();
      std::cout << "get " << t.elapsed().wall/1.0e9 << "\n";
    }

  //  printMatrix(b);

  // Object c = mytype->_double->makeMatrix(3,1);
  // double *cp = mytype->_double->readWriteData(c);
  
  while (1)
    {
      char *p = readline("--> ");
      if (p && *p)
	add_history(p);
      FMString body(p);
      body += "\n\n";
      try {
	Scanner S(body,"");
	Parser P(S);
	Tree b(P.process());
	b.print();
	Compiler C(mytype);
	C.compile(b);
	//	C.dump();
	Assembler A(C.module()->_main);
	A.run();
	Object p = A.codeObject(mytype);
	std::cout << "Code object: " << p.description() << "\n";
	Disassemble(mytype,p);
      } catch (const FM::Exception &e) {
	std::cout << "Exception: " << e.msg() << "\n";
      }
      // A.run();
      // Array p = A.codeObject();
      // Disassemble(p);
    }


  boost::timer::cpu_timer timer;

  {
    double p = 1.0;
    base._double->makeScalar(p);
    for (int i=0;i<5;i++)
      {
	timer.start();
	std::cout << "Result = " << simVM<Object>(base._double->makeScalar(p)) << "\n";
	timer.stop();
	std::cout << "simVM <Object3> "  << timer.elapsed().wall/1.0e9 << "\n";
      }
  }

  if (1){
    // Check out the logic
    double p = 3.0;
    Object a = base._double->makeScalar(p);
    std::cout << "a = " << SCALARVALUE(a) << "\n";
    Object b = a;
    Object c = b;
    std::cout << "b = " << SCALARVALUE(b) << "\n";
    double *pb = static_cast<double*>(base._double->readWriteData(b));
    pb[0] = 7.0;
    std::cout << "b = " << SCALARVALUE(b) << "\n";
    std::cout << "a = " << SCALARVALUE(a) << "\n";
    std::cout << "c = " << SCALARVALUE(c) << "\n";
    Object d = base._double->makeMatrix(4,4);
    printMatrix(d);
    double *q = static_cast<double*>(base._double->readWriteData(d));
    for (int i=0;i<16;i++) q[i] = double(i);
    printMatrix(d);
    Object e = base._double->sliceColumn(d,3);
    printMatrix(e);
    double *w = static_cast<double*>(base._double->readWriteData(e));
    w[3] = 52;
    printMatrix(e);
    printMatrix(d);
  }
}
