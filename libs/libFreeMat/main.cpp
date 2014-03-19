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

int main(int argc, char *argv[])
{
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
  while (1)
    {
      char *p = readline("--> ");
      if (p && *p)
	add_history(p);
      //      p_eval->evaluateString(FMString(p)+"\n",false);
      bool success;
      //      FMString body = ReadFileIntoString(p, success);
      FMString body(p);
      body += "\n\n";
      try {
	Scanner S(body,"");
	Parser P(S);
	Tree b = P.process();
	b.print();
	Compiler C;
	C.compile(b);
	//	C.dump();
	Assembler A(C.module()->_main);
	A.run();
	Array p = A.codeObject();
	Disassemble(p);
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
}
