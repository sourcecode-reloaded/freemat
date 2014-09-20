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
  bool failed;
  FMString body = ReadFileIntoString(name+".m",failed);
  body += "\n\n";
  ctxt->_compiler->compile(body);
  Object p = ctxt->_asm->run(ctxt->_compiler->module()->_main);
  Disassemble(ctxt,p);
  ctxt->_vm->defineBaseVariable(name,p);
}

void testMap(int cnt)
{
  FMStringList words;
  // Generate a set of random words
  for (int i=0;i<cnt;i++)
    {
      char buffer[4096];
      int wlen = rand() % 32 + 1;
      for (int j=0;j<wlen;j++)
	buffer[j] = rand() % 26 + 'a';
      buffer[wlen] = 0;
      std::cout << "word: " << buffer << "\n";
      words.push_back(FMString(buffer));
    }
}

int main(int argc, char *argv[])
{

  assert(sizeof(Complex<char>) == 2*sizeof(char));

  StdIOTermIF io;
  ThreadContext *ctxt = BuildNewThreadContext(&io);

  boost::timer::cpu_timer timer;

  testMap(50);

  // For now - hard code a single function to preload
  compileFunc(ctxt,"three");
  compileFunc(ctxt,"add");
  compileFunc(ctxt,"fixa");
  
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
