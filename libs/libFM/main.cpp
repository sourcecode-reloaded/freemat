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


void compileFunc(ThreadContext *ctxt, FMString name)
{
  try {
    bool failed;
    FMString body = ReadFileIntoString(name+".m",failed);
    body += "\n\n";
    ctxt->_compiler->compile(body);
    Object p = ctxt->_asm->run(ctxt->_compiler->module()->_main);
    Disassemble(ctxt,p);
    ctxt->_globals->insert(std::make_pair(name,p));
  } catch (const FM::Exception &e) {
    std::cout << "Exception: " << e.msg() << "\n";
  }
  //  ctxt->_vm->defineBaseVariable(name,p);
}

int main(int argc, char *argv[])
{

  assert(sizeof(Complex<char>) == 2*sizeof(char));

  StdIOTermIF io;
  ThreadContext *ctxt = BuildNewThreadContext(&io);

  boost::timer::cpu_timer timer;

  // Create a new meta class
  Object fooMeta = ctxt->_meta->empty();
  ctxt->_meta->setName(fooMeta,"foo");
  ctxt->_meta->addProperty(fooMeta,ctxt->_string->makeString("color"),
			   ctxt->_string->makeString("RED"));
  ctxt->_meta->addProperty(fooMeta,ctxt->_string->makeString("length"),
			   ctxt->_double->makeScalar(32));

  ctxt->_globals->insert(std::make_pair("foo_meta",fooMeta));
  Object foo = ctxt->_meta->construct(fooMeta);
  ctxt->_globals->insert(std::make_pair("foo",foo));


  // For now - hard code a single function to preload
  compileFunc(ctxt,"three");
  compileFunc(ctxt,"add");
  compileFunc(ctxt,"fixa");
  compileFunc(ctxt,"dima");
  compileFunc(ctxt,"t4");
  compileFunc(ctxt,"swit");
  compileFunc(ctxt,"exc");
  compileFunc(ctxt,"excb");
  
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
	Object p = ctxt->_asm->run(ctxt->_compiler->module()->_main);
	std::cout << "Code object: " << p.description() << "\n";
	Disassemble(ctxt,p);
	timer.start();
	ctxt->_vm->executeScript(p);
	timer.stop();
	std::cout << " Execution time " << timer.elapsed().wall/1.0e9 << "\n";
      } catch (const FM::Exception &e) {
	std::cout << "Exception: " << e.msg() << "\n";
      }
    }

  return 0;


}
