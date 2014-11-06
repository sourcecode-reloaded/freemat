#include <boost/timer/timer.hpp>
#include <boost/unordered_map.hpp>
#include <unordered_map>
#include <readline/readline.h>
#include <readline/history.h>
#include "Object.hpp"
#include "DoubleType.hpp"
#include "Scanner.hpp"
#include "Parser.hpp"
#include "ThreadContext.hpp"
#include "AllTypes.hpp"
#include "Compiler.hpp"
#include "Assembler.hpp"
#include "VM.hpp"
#include "fnv.hh"
#include "HashMap.hpp"

//#include <valarray>

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

class TestData {
public:
  Object m_defaults;
  HashMap<int> m_props;
  FMString m_name;
  TestData(ThreadContext *_ctxt) : m_defaults(_ctxt) {}
};

class TestType : public AggregateType<TestData,HandleSemantics> {
public:
  TestType(ThreadContext *ctxt) {_ctxt = ctxt;}
  TestData* makeEmptyDataType() {return new TestData(_ctxt);}
  const FMString& name() const {const static FMString _name = "Test"; return _name;}
  bool equals(const Object &a, const Object &b) { return false;}
  FMString describe(const Object &a) {return 
      FMString("Junk:") + this->ro(a)->m_name + "\n" + 
      this->ro(a)->m_defaults.description() + "\n";}
  FMString brief(const Object &a) {return this->describe(a);}
  DataCode code() const {return TypeCode;}
};

void compileFunc(ThreadContext *ctxt, FMString name)
{
  try {
    bool failed;
    FMString body = ReadFileIntoString(name+".m",failed);
    body += "\n\n";
    ctxt->_compiler->compile(body);
    Module *mod = ctxt->_compiler->module();
    if (mod)
      {
	Object p = ctxt->_asm->run(mod->_main);
	Disassemble(ctxt,p);
	ctxt->_globals->insert(std::make_pair(name,p));
      }
  } catch (const FM::Exception &e) {
    std::cout << "Exception: " << e.msg() << "\n";
  }
}

void compileModule(ThreadContext *ctxt, FMString name)
{
  std::cout << "Compiling module: " << name << "\n";
  try {
    bool failed;
    FMString body = ReadFileIntoString(name+".m",failed);
    body += "\n\n";
    ctxt->_compiler->compile(body);
    Module *mod = ctxt->_compiler->module();
    if (mod)
      {
	Object p = ctxt->_asm->run(mod);
	Disassemble(ctxt,p);
	std::cout << "Compile: \n";
	std::cout << p << "\n";
	ctxt->_globals->insert(std::make_pair(name,p));
      }
    else
      {
	std::cout << "   Module empty\n";
      }
  } catch (const FM::Exception &e) {
    std::cout << "Exception: " << e.msg() << "\n";
  }
}

void addStuff(TestType *type, Object metaClass, Object prop) {
  type->rw(metaClass)->m_defaults = prop;
}

int main(int argc, char *argv[])
{

  assert(sizeof(Complex<char>) == 2*sizeof(char));

  StdIOTermIF io;
  ThreadContext *ctxt = BuildNewThreadContext(&io);

  boost::timer::cpu_timer timer;

  TestType *ttype = new TestType(ctxt);
  {
    Object junk = ttype->makeScalar();
    Object value = ctxt->_string->makeString("FooFoo");
    addStuff(ttype,junk,value);
    std::cout << "junk = " << junk.description() << "\n";
    ctxt->_globals->insert(std::make_pair("junk",junk));
  }
  Object junk2 = ctxt->_globals->at("junk");
  std::cout << "junk2 = " << junk2.description() << "\n";

  // Create a new meta class
  Object fooMeta = ctxt->_meta->empty();
  ctxt->_meta->setName(fooMeta,"foo");
  ctxt->_meta->addProperty(fooMeta,ctxt->_string->makeString("color"),
			   ctxt->_string->makeString("RED"));
  ctxt->_meta->addProperty(fooMeta,ctxt->_string->makeString("length"),
			   ctxt->_double->makeScalar(32));
  // ctxt->_meta->addMethod(fooMeta,ctxt->_string->makeString("incr"),
  // 			 compileFunc(ctxt,"incr"));
  // ctxt->_meta->addMethod(fooMeta,ctxt->_string->makeString("decr"),
  // 			 compileFunc(ctxt,"decr"));

  ctxt->_globals->insert(std::make_pair("?foo",fooMeta));
  Object foo = ctxt->_meta->construct(fooMeta);
  ctxt->_globals->insert(std::make_pair("foo",foo));


  boost::filesystem::path cwd(boost::filesystem::current_path());
  boost::filesystem::directory_iterator p(cwd);
  while (p != boost::filesystem::directory_iterator()) {
    std::cout << "File: " << *p << " Extension " << p->path().extension() << " Stem " << p->path().stem() << "\n";
    if (p->path().extension() == ".m" || p->path().extension() == ".M")
      {
	FMString func = p->path().stem().string();
	std::cout << "  Parsing function " << func << "\n";
	//compileModule(ctxt,func);
      }
    ++p;
  }

  // For now - hard code a single function to preload
  /*
  ctxt->_globals->insert(std::make_pair("three",compileFunc(ctxt,"three")));
  ctxt->_globals->insert(std::make_pair("add",compileFunc(ctxt,"add")));
  ctxt->_globals->insert(std::make_pair("fixa",compileFunc(ctxt,"fixa")));
  ctxt->_globals->insert(std::make_pair("dima",compileFunc(ctxt,"dima")));
  ctxt->_globals->insert(std::make_pair("t4",compileFunc(ctxt,"t4")));
  ctxt->_globals->insert(std::make_pair("swit",compileFunc(ctxt,"swit")));
  ctxt->_globals->insert(std::make_pair("exc",compileFunc(ctxt,"exc")));
  ctxt->_globals->insert(std::make_pair("excb",compileFunc(ctxt,"excb")));
  ctxt->_globals->insert(std::make_pair("bind",compileFunc(ctxt,"bind")));
  */

  // Object sclassMeta = ctxt->_globals->at("?sclass");
  // Object soo = ctxt->_meta->construct(sclassMeta);
  // ctxt->_globals->insert(std::make_pair("soo",soo));

  //  compileFunc(ctxt,"sclass");
  
  //  compileModule(ctxt,"localfn");
  //  compileModule(ctxt,"three");

  compileModule(ctxt,"nest1");


  {
    // Create a variable
    Object a = ctxt->_double->makeScalar(3.14);
    // Create a capture of it
    Object cap1 = ctxt->_captured->makeScalar();
    ctxt->_captured->set(cap1,a);
    // Insert cap1 as a variable in the global workspace
    ctxt->_globals->insert(std::make_pair("cap1",cap1));

    // Insert a copy of cap1 as another variable in the global workspace
    Object cap2 = ctxt->_captured->makeScalar();
    cap2 = cap1;
    ctxt->_globals->insert(std::make_pair("cap2",cap2));
  }

  // Global symbols
  for (auto i=ctxt->_globals->begin(); i != ctxt->_globals->end(); ++i)
    {
      std::cout << "Global symbol: " << i->first << ": " << i->second.brief() << "\n";
    }

  // Look up the global symbol for cap1
  {
    Object cap1ref = ctxt->_globals->at("cap1");
    ctxt->_captured->set(cap1ref,ctxt->_double->makeScalar(2.78));
  }

  while (1)
    {
      char *p = readline("--> ");
      if (p && *p)
	add_history(p);
      if (!p)
	return 0;
      FMString body(p);
      // bool failed;
      // body = ReadFileIntoString(body,failed);
      body += "\n\n";
      try {
	ctxt->_compiler->compile(body);
	Module *mod = ctxt->_compiler->module();
	if (mod)
	  {
	    Object p = ctxt->_asm->run(mod->_main);
	    std::cout << "Code object: " << p.description() << "\n";
	    Disassemble(ctxt,p);
	    timer.start();
	    ctxt->_vm->executeScript(p);
	    timer.stop();
	    std::cout << " Execution time " << timer.elapsed().wall/1.0e9 << "\n";
	  }
      } catch (const FM::Exception &e) {
	std::cout << "Exception: " << e.msg() << "\n";
      }
    }

  return 0;


}
