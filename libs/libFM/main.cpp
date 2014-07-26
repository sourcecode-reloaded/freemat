#include <boost/timer/timer.hpp>
#include <boost/unordered_map.hpp>
#include <unordered_map>
#include <readline/readline.h>
#include <readline/history.h>
#include "Object.hpp"
#include "DoubleType.hpp"
#include "Scanner.hpp"
#include "Parser.hpp"
#include "BaseTypes.hpp"
#include "Compiler.hpp"
#include "Assembler.hpp"
#include "VM.hpp"
#include "fnv.hh"

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
  return a.type()->Add(a,b);
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
	printf("%6g ",p[i+j*rows]);
      std::cout << "\n";
    }
  std::cout << "\n";
}

void printList(const Object &a)
{
  int rows = a.rows();
  const Object *p = static_cast<ListType*>(a.type())->readOnlyData(a);
  for (int i=0;i<rows;i++)
    {
      std::cout << "i = " << i << "\n";
      std::cout << "  " << p[i].description() << "\n";
    }
}

std::vector<std::string> & operator<< (std::vector<std::string> & vec, const std::string &s) {
  vec.push_back(s);
  return vec;
}

int main(int argc, char *argv[])
{

  assert(sizeof(Complex<char>) == 2*sizeof(char));

  /*
  std::cout << "Sizeof VMInstruction = " << sizeof(VMInstruction) << "\n";

  assert(sizeof(VMInstruction) == sizeof(uint64_t));

    {
  VMInstruction p;

  p.opcode = 143;
  p.reg1 = 412;
  p.reg2 = 54;
  p.constant = 5123;

  std::cout << "Opcode = " << p.opcode << "\n";
    }
  */ 
  {
    boost::timer::cpu_timer x;
    FMStringList fields;
    fields << "name" << "code" << "captured" << "free" << "foo" << "A" << "x";
    FMMap<FMString,int> tmap;
    int fieldnum = fields.size();
    for (int i=0;i<fieldnum;i++)
      tmap[fields[i]] = i;
    int hits = 0;
    x.start();
    for (int i=0;i<10000000;i++)
      {
	int j = random() % 7;
	bool pop = tmap.contains(fields[j]);
	if (pop) hits++;
      }
    x.stop();
    std::cout << "Test 1:" << hits << " Execution time " << x.elapsed().wall/1.0e9 - 0.05 << " secs\n";
  }

  {
    boost::timer::cpu_timer x;
    std::vector<std::string> fields;
    fields << "name" << "code" << "captured" << "free" << "foo" << "A" << "x";
    boost::unordered_map<std::string,int> tmap;
    int fieldnum = fields.size();
    for (int i=0;i<fieldnum;i++)
      tmap[fields[i]] = i;
    int hits = 0;
    x.start();
    for (int i=0;i<10000000;i++)
      {
	int j = random() % 7;
	bool pop = tmap.find(fields[j]) != tmap.end();
	if (pop) hits++;
      }
    x.stop();
    std::cout << "Test 1b:" << hits << " Execution time " << x.elapsed().wall/1.0e9 - 0.05 << " secs\n";
  }
    
#if 0
 {
    boost::timer::cpu_timer x;
    FMStringList fields;
    fields << "name" << "code" << "captured" << "free" << "foo" << "A" << "x";
    FMMap<FMString,int> tmap;
    int fieldnum = fields.size();
    for (int i=0;i<fieldnum;i++)
      tmap[fields[i]] = i;
    int hits = 0;
    x.start();
    hash::fnv<32> hsh;
    for (int i=0;i<10000000;i++)
      {
	int j = random() % 7;
	const char *p = fields[j].c_str();
	int len = fields[j].size();
	int q = hsh(p,len);
        hits += q;
      }
    x.stop();
    std::cout << "Test 2:" << hits << " Execution time " << x.elapsed().wall/1.0e9 - 0.05 << " secs\n";
  }
    

 {
    boost::timer::cpu_timer x;
    FMStringList fields;
    fields << "name" << "code" << "captured" << "free" << "foo" << "A" << "x";
    FMMap<FMString,int> tmap;
    int fieldnum = fields.size();
    for (int i=0;i<fieldnum;i++)
      tmap[fields[i]] = i;
    int hits = 0;
    x.start();
    for (int i=0;i<10000000;i++)
      {
	int j = random() % 7;
	const char *p = fields[j].c_str();
	int len = fields[j].size();
        hits += len;
      }
    x.stop();
    std::cout << "Test 3:" << hits << " Execution time " << x.elapsed().wall/1.0e9 - 0.05 << " secs\n";
  }
    


  {
    boost::timer::cpu_timer x;
    FMStringList fields;
    fields << "name" << "code" << "captured" << "free" << "foo" << "A" << "x";
    FMMap<int,int> tmap;
    int fieldnum = fields.size();
    for (int i=0;i<fieldnum;i++)
      tmap[fields[i].size()] = i;
    int hits = 0;
    x.start();
    for (int i=0;i<10000000;i++)
      {
	int j = random() % 7;
	bool pop = tmap.contains(fields[j].size());
	if (pop) hits++;
      }
    x.stop();
    std::cout << "Test 4:" << hits << " Execution time " << x.elapsed().wall/1.0e9  - 0.05  << " secs\n";
  }

  {
    boost::timer::cpu_timer x;
    FMStringList fields;
    fields << "name" << "code" << "captured" << "free" << "foo" << "A" << "x";
    FMMap<int,int> tmap;
    hash::fnv<32> hsh;
    int fieldnum = fields.size();
    for (int i=0;i<fieldnum;i++)
      tmap[hsh(fields[i].c_str(),fields[i].size())] = i;
    int hits = 0;
    x.start();
    for (int i=0;i<10000000;i++)
      {
	int j = random() % 7;
	bool pop = tmap.contains(hsh(fields[j].c_str(),fields[j].size()));
	if (pop) hits++;
      }
    x.stop();
    std::cout << "Test 4h:" << hits << " Execution time " << x.elapsed().wall/1.0e9  - 0.05  << " secs\n";
  }


  {
    boost::timer::cpu_timer x;
    FMStringList fields;
    fields << "name" << "code" << "captured" << "free" << "foo" << "A" << "x";
    FMMap<int,int> tmap;
    int fieldnum = fields.size();
    for (int i=0;i<fieldnum;i++)
      tmap[fields[i].size()] = i;
    int hits = 0;
    x.start();
    for (int i=0;i<10000000;i++)
      {
	int j = random() % 7;
	bool pop = tmap.contains(fields[j].size());
	if (pop) hits++;
      }
    x.stop();
    std::cout << "Test 5:" << hits << " Execution time " << x.elapsed().wall/1.0e9  - 0.05 << " secs\n";
  }


  {
    boost::timer::cpu_timer x;
    FMStringList fields;
    fields << "name" << "code" << "captured" << "free" << "foo" << "A" << "x";
    int cache[256];
    int fieldnum = fields.size();
    for (int i=0;i<fieldnum;i++)
      cache[fields[i].size() % 256] = i;
    int hits = 0;
    x.start();
    for (int i=0;i<10000000;i++)
      {
	int j = random() % 7;
	bool pop = cache[fields[j].size() % 256];
	if (pop) hits++;
      }
    x.stop();
    std::cout << "Test 6:" << hits << " Execution time " << x.elapsed().wall/1.0e9  - 0.05 << " secs\n";
  }
    
  {
    boost::timer::cpu_timer x;
    FMStringList fields;
    fields << "name" << "code" << "captured" << "free" << "foo" << "A" << "x";
    std::vector<int> cache[256];
    int fieldnum = fields.size();
    for (int i=0;i<fieldnum;i++)
      cache[fields[i].size() % 256].push_back(fields[i].size());
    int hits = 0;
    x.start();
    for (int i=0;i<10000000;i++)
      {
	int j = random() % 7;
	int m = fields[j].size();
	std::vector<int> &q = cache[m % 256];
	bool pop = false;
	for (int k=0;k<q.size();k++)
	  if (q[k] == m) pop = true;
	if (pop) hits++;
      }
    x.stop();
    std::cout << "Test 7:" << hits << " Execution time " << x.elapsed().wall/1.0e9 - 0.05  << " secs\n";
  }
    

#endif

  BaseTypes *mytype = new BaseTypes;

  Object d = mytype->_double->makeMatrix(4,4);
  double *dp = mytype->_double->readWriteData(d);
  for (int i=0;i<16;i++) dp[i] = i;
  printMatrix(d);

  Object c = d;
  mytype->_double->resize(c,Tuple(6,5));
  printMatrix(c);

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
  
  printMatrix(mytype->_list->first(d.type()->get(d,nd)));

  d.type()->set(d,nd,mytype->_double->makeScalar(42));
  
  printMatrix(d);

  /*
  Object lst = mytype->_list->empty();
  for (int i=0;i<15;i++)
    {
      mytype->_list->push(lst,mytype->_double->makeScalar(3));
      mytype->_list->push(lst,mytype->_double->makeScalar(5));
    }
  std::cout << "Loop 1 completed\n"; 
  Object lst2 = lst;

  for (int j=0;j<10;j++)
    mytype->_list->push(lst,mytype->_string->makeString("Hello"));
  printList(lst);
  printList(lst2);
  */

  for (int i=0;i<50000;i++)
    {
      Object p = mytype->_list->empty();
      mytype->_list->push(p,mytype->_double->makeScalar(42));
      mytype->_list->push(p,mytype->_double->makeScalar(47));
      Object q = mytype->_list->empty();
      mytype->_list->push(q,p);
    }

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

  boost::timer::cpu_timer timer;
  
  VM vm(mytype);
  while (1)
    {
      char *p = readline("--> ");
      if (p && *p)
	add_history(p);
      if (!p)
	return 0;
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
	timer.start();
	vm.executeScript(p);
	timer.stop();
	std::cout << " Execution time " << timer.elapsed().wall/1.0e9 << "\n";
	//	vm.dump();
      } catch (const FM::Exception &e) {
	std::cout << "Exception: " << e.msg() << "\n";
      }
      // A.run();
      // Array p = A.codeObject();
      // Disassemble(p);
    }



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
