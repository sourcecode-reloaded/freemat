#include "CJitFuncClang.hpp"
#include "CArray.hpp"
#include <iostream>
#include <fstream>

#include "clang/CodeGen/CodeGenAction.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Tool.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/DiagnosticOptions.h"
#include "clang/Frontend/FrontendDiagnostic.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"

#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Config/config.h"
#include "llvm/ADT/OwningPtr.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Config/config.h"
#include "llvm/LLVMContext.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/System/Host.h"
#include "llvm/System/Path.h"
#include "llvm/Target/TargetSelect.h"
#include "llvm/Target/TargetOptions.h"

#include <QTemporaryFile>
#include <QDir>

using namespace clang;
using namespace clang::driver;


CJitFuncClang::CJitFuncClang(Interpreter* eval)
{
  llvm::InitializeNativeTarget();
  m_eval = eval;
}

CJitFuncClang::~CJitFuncClang()
{
  delete EE;
  delete comp;
}
QString GetRootPath();

bool CJitFuncClang::compile(const std::string &filename, 
			    const std::string &funcname) 
{
  TextDiagnosticPrinter *DiagClient =
    new TextDiagnosticPrinter(llvm::errs(), DiagnosticOptions());
  Diagnostic Diags(DiagClient);
  Driver TheDriver("",
		   llvm::sys::getHostTriple(),
                   "a.out", /*IsProduction=*/false, /*CXXIsProduction=*/false,
                   Diags);
  TheDriver.setTitle("FreeMat JIT");
  llvm::SmallVector<const char *, 16> Args;
  Args.push_back("FreeMat");
  Args.push_back(filename.c_str());
  Args.push_back("-fsyntax-only");
  Args.push_back("-O3");
  Args.push_back("-v");
  llvm::OwningPtr<Compilation> C(TheDriver.BuildCompilation(Args.size(),
                                                            Args.data()));
  if (!C) return false;
  const driver::JobList &Jobs = C->getJobs();
  if (Jobs.size() != 1 || !isa<driver::Command>(Jobs.begin())) {
    llvm::SmallString<256> Msg;
    llvm::raw_svector_ostream OS(Msg);
    C->PrintJob(OS, C->getJobs(), "; ", true);
    Diags.Report(diag::err_fe_expected_compiler_job) << OS.str();
    return false;
  }
  const driver::Command *Cmd = cast<driver::Command>(*Jobs.begin());
  if (llvm::StringRef(Cmd->getCreator().getName()) != "clang") {
    Diags.Report(diag::err_fe_expected_clang_command);
    return false;
  }
  // Initialize a compiler invocation object from the clang (-cc1) arguments.
  const driver::ArgStringList &CCArgs = Cmd->getArguments();
  llvm::OwningPtr<CompilerInvocation> CI(new CompilerInvocation);
  CompilerInvocation::CreateFromArgs(*CI,
                                     const_cast<const char **>(CCArgs.data()),
                                     const_cast<const char **>(CCArgs.data()) +
                                       CCArgs.size(),
                                     Diags);
  QString path = GetRootPath() + "/toolbox/jit";
  CI->getHeaderSearchOpts().AddPath(path.toStdString().c_str(),frontend::Quoted,true,false,false);
 
 // Show the invocation, with -v.
  if (CI->getHeaderSearchOpts().Verbose) {
    llvm::errs() << "clang invocation:\n";
    C->PrintJob(llvm::errs(), C->getJobs(), "\n", true);
    llvm::errs() << "\n";
  }

  // FIXME: This is copied from cc1_main.cpp; simplify and eliminate.
  // Create a compiler instance to handle the actual work.
  comp = new clang::CompilerInstance;
  comp->setLLVMContext(new llvm::LLVMContext);
  comp->setInvocation(CI.take());
  // Create the compilers actual diagnostics engine.
  comp->createDiagnostics(int(CCArgs.size()),const_cast<char**>(CCArgs.data()));
  if (!comp->hasDiagnostics()) return false;
  // Create and execute the frontend to generate an LLVM bitcode module.
  llvm::errs() << "Generating code...\n";
  llvm::OwningPtr<CodeGenAction> Act(new EmitLLVMOnlyAction());
  if (!comp->ExecuteAction(*Act)) return false;
  llvm::errs() << "Done...\n";
  llvm::JITExceptionHandling = true;
  if (llvm::Module *Module = Act->takeModule())
    {
      llvm::errs() << (*Module);
      std::string Error;
      EE = llvm::ExecutionEngine::createJIT(Module,&Error);
      if (!EE)
	{
	  llvm::errs() << "Unable to initialize JIT: " << Error;
	  return false;
	}
      func =  Module->getFunction(funcname);
      return true;
    }
  return false;
}

bool CJitFuncClang::compile(const Tree & t)
{
  CJitFunc mcomp;
  mcomp.set_interpreter(m_eval);
  mcomp.compile_tree(t,std::string("testfunc"));
  // QTemporaryFile file(QDir::tempPath()+"/jitXXXXXX.cpp");
  // file.open();
  // std::string codename = file.fileName().toStdString();
  std::string codename = "/tmp/jit.cpp";
  mcomp.writeCode(codename);
  return compile(codename,"testfunc");
}

int CJitFuncClang::run()
{
  std::vector<llvm::GenericValue> args(1);
  args[0].PointerVal = m_eval;
  llvm::GenericValue ret = EE->runFunction(func,args);
  int retval = ret.IntVal.getSExtValue();
  if (retval == CJIT_Runfail)
    throw Exception(m_eval->getLastErrorString());
  return retval;
}

static void __force_linkage()
{
  carray_empty();
}