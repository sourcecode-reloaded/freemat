#include <iostream>

#include <llvm/Support/raw_ostream.h>

#include <clang/Frontend/DiagnosticOptions.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/TargetOptions.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Basic/FileManager.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Lex/HeaderSearch.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Frontend/HeaderSearchOptions.h>
#include <clang/Frontend/PreprocessorOptions.h>
#include <clang/Frontend/FrontendOptions.h>
#include <clang/Frontend/Utils.h>
#include <clang/Basic/IdentifierTable.h>
#include <clang/Parse/Action.h>
#include <clang/Parse/Parser.h>
#include "clang/Sema/ParseAST.h"

#include "clang/Frontend/CodeGenAction.h"
#include "clang/CodeGen/CodeGenOptions.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/DeclGroup.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/CodeGen/CodeGenOptions.h"
#include "clang/CodeGen/ModuleBuilder.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendDiagnostic.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/ADT/OwningPtr.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/CodeGen/RegAllocRegistry.h"
#include "llvm/CodeGen/SchedulerRegistry.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/StandardPasses.h"
#include "llvm/Support/Timer.h"
#include "llvm/Target/SubtargetFeature.h"
#include "llvm/Target/TargetData.h"
#include <llvm/Target/TargetSelect.h>
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetRegistry.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/LLVMContext.h"

#include <iostream>

using namespace clang;
using namespace llvm;

namespace clang {
  enum BackendAction {
    Backend_EmitAssembly,  ///< Emit native assembly files
    Backend_EmitBC,        ///< Emit LLVM bitcode files
    Backend_EmitLL,        ///< Emit human-readable LLVM assembly
    Backend_EmitNothing,   ///< Don't emit anything (benchmarking mode)
    Backend_EmitObj        ///< Emit native object files
  };

  class JITConsumer : public ASTConsumer {
    Diagnostic &Diags;
    BackendAction Action;
    const CodeGenOptions &CodeGenOpts;
    const LangOptions &LangOpts;
    const TargetOptions &TargetOpts;
    llvm::raw_ostream *AsmOutStream;
    llvm::formatted_raw_ostream FormattedOutStream;
    ASTContext *Context;

    Timer LLVMIRGeneration;
    Timer CodeGenerationTime;

    llvm::OwningPtr<CodeGenerator> Gen;

    llvm::OwningPtr<llvm::Module> TheModule;
    llvm::TargetData *TheTargetData;

    mutable FunctionPassManager *CodeGenPasses;
    mutable PassManager *PerModulePasses;
    mutable FunctionPassManager *PerFunctionPasses;

    FunctionPassManager *getCodeGenPasses() const;
    PassManager *getPerModulePasses() const;
    FunctionPassManager *getPerFunctionPasses() const;

    void CreatePasses();

    /// AddEmitPasses - Add passes necessary to emit assembly or LLVM IR.
    ///
    /// \return True on success.
    bool AddEmitPasses();

    void EmitAssembly();

  public:
    JITConsumer(BackendAction action, Diagnostic &_Diags,
                    const LangOptions &langopts, const CodeGenOptions &compopts,
                    const TargetOptions &targetopts, bool TimePasses,
                    const std::string &infile, llvm::raw_ostream *OS,
                    LLVMContext& C) :
      Diags(_Diags),
      Action(action),
      CodeGenOpts(compopts),
      LangOpts(langopts),
      TargetOpts(targetopts),
      AsmOutStream(OS),
      LLVMIRGeneration("LLVM IR Generation Time"),
      CodeGenerationTime("Code Generation Time"),
      Gen(CreateLLVMCodeGen(Diags, infile, compopts, C)),
      TheTargetData(0),
      CodeGenPasses(0), PerModulePasses(0), PerFunctionPasses(0) {

      if (AsmOutStream)
        FormattedOutStream.setStream(*AsmOutStream,
                                     formatted_raw_ostream::PRESERVE_STREAM);

      llvm::TimePassesIsEnabled = TimePasses;
	  InitializeNativeTarget();

    }

    ~JITConsumer() {
      delete TheTargetData;
      delete CodeGenPasses;
      delete PerModulePasses;
      delete PerFunctionPasses;
    }

    llvm::Module *takeModule() { return TheModule.take(); }

    virtual void Initialize(ASTContext &Ctx) {
      Context = &Ctx;

      if (llvm::TimePassesIsEnabled)
        LLVMIRGeneration.startTimer();

      Gen->Initialize(Ctx);

      TheModule.reset(Gen->GetModule());
      TheTargetData = new llvm::TargetData(Ctx.Target.getTargetDescription());

      if (llvm::TimePassesIsEnabled)
        LLVMIRGeneration.stopTimer();
    }

    virtual void HandleTopLevelDecl(DeclGroupRef D) {
      PrettyStackTraceDecl CrashInfo(*D.begin(), SourceLocation(),
                                     Context->getSourceManager(),
                                     "LLVM IR generation of declaration");

      if (llvm::TimePassesIsEnabled)
        LLVMIRGeneration.startTimer();

      Gen->HandleTopLevelDecl(D);

      if (llvm::TimePassesIsEnabled)
        LLVMIRGeneration.stopTimer();
    }

    virtual void HandleTranslationUnit(ASTContext &C) {
      {
        PrettyStackTraceString CrashInfo("Per-file LLVM IR generation");
        if (llvm::TimePassesIsEnabled)
          LLVMIRGeneration.startTimer();

        Gen->HandleTranslationUnit(C);

        if (llvm::TimePassesIsEnabled)
          LLVMIRGeneration.stopTimer();
      }

      // EmitAssembly times and registers crash info itself.
      EmitAssembly();

      // Force a flush here in case we never get released.
      if (AsmOutStream)
        FormattedOutStream.flush();
    }

    virtual void HandleTagDeclDefinition(TagDecl *D) {
      PrettyStackTraceDecl CrashInfo(D, SourceLocation(),
                                     Context->getSourceManager(),
                                     "LLVM IR generation of declaration");
      Gen->HandleTagDeclDefinition(D);
    }

    virtual void CompleteTentativeDefinition(VarDecl *D) {
      Gen->CompleteTentativeDefinition(D);
    }

	virtual void ExecuteModule( void ){
		std::string errorstring;

		ExecutionEngine* ee = ExecutionEngine::createJIT(TheModule.get(), &errorstring);
		Function* fn = ee->FindFunctionNamed( "_Z3f_tv" );
		std::vector<GenericValue> arg;
		GenericValue ret = ee->runFunction( fn, arg );
	}
  };
}

FunctionPassManager *JITConsumer::getCodeGenPasses() const {
  if (!CodeGenPasses) {
    CodeGenPasses = new FunctionPassManager(&*TheModule);
    CodeGenPasses->add(new TargetData(*TheTargetData));
  }

  return CodeGenPasses;
}

PassManager *JITConsumer::getPerModulePasses() const {
  if (!PerModulePasses) {
    PerModulePasses = new PassManager();
    PerModulePasses->add(new TargetData(*TheTargetData));
  }

  return PerModulePasses;
}

FunctionPassManager *JITConsumer::getPerFunctionPasses() const {
  if (!PerFunctionPasses) {
    PerFunctionPasses = new FunctionPassManager(&*TheModule);
    PerFunctionPasses->add(new TargetData(*TheTargetData));
  }

  return PerFunctionPasses;
}

bool JITConsumer::AddEmitPasses() {
  if (Action == Backend_EmitNothing)
    return true;

  if (Action == Backend_EmitBC) {
    getPerModulePasses()->add(createBitcodeWriterPass(FormattedOutStream));
  } else if (Action == Backend_EmitLL) {
    getPerModulePasses()->add(createPrintModulePass(&FormattedOutStream));
  } else {
    bool Fast = CodeGenOpts.OptimizationLevel == 0;

    // Create the TargetMachine for generating code.
    std::string Error;
    std::string Triple = TheModule->getTargetTriple();
    const llvm::Target *TheTarget = TargetRegistry::lookupTarget(Triple, Error);
    if (!TheTarget) {
      Diags.Report(diag::err_fe_unable_to_create_target) << Error;
      return false;
    }

    // FIXME: Expose these capabilities via actual APIs!!!! Aside from just
    // being gross, this is also totally broken if we ever care about
    // concurrency.
    llvm::NoFramePointerElim = CodeGenOpts.DisableFPElim;
    if (CodeGenOpts.FloatABI == "soft")
      llvm::FloatABIType = llvm::FloatABI::Soft;
    else if (CodeGenOpts.FloatABI == "hard")
      llvm::FloatABIType = llvm::FloatABI::Hard;
    else {
      assert(CodeGenOpts.FloatABI.empty() && "Invalid float abi!");
      llvm::FloatABIType = llvm::FloatABI::Default;
    }
    NoZerosInBSS = CodeGenOpts.NoZeroInitializedInBSS;
    llvm::UseSoftFloat = CodeGenOpts.SoftFloat;
    UnwindTablesMandatory = CodeGenOpts.UnwindTables;

    TargetMachine::setAsmVerbosityDefault(CodeGenOpts.AsmVerbose);

    // FIXME: Parse this earlier.
    if (CodeGenOpts.RelocationModel == "static") {
      TargetMachine::setRelocationModel(llvm::Reloc::Static);
    } else if (CodeGenOpts.RelocationModel == "pic") {
      TargetMachine::setRelocationModel(llvm::Reloc::PIC_);
    } else {
      assert(CodeGenOpts.RelocationModel == "dynamic-no-pic" &&
             "Invalid PIC model!");
      TargetMachine::setRelocationModel(llvm::Reloc::DynamicNoPIC);
    }
    // FIXME: Parse this earlier.
    if (CodeGenOpts.CodeModel == "small") {
      TargetMachine::setCodeModel(llvm::CodeModel::Small);
    } else if (CodeGenOpts.CodeModel == "kernel") {
      TargetMachine::setCodeModel(llvm::CodeModel::Kernel);
    } else if (CodeGenOpts.CodeModel == "medium") {
      TargetMachine::setCodeModel(llvm::CodeModel::Medium);
    } else if (CodeGenOpts.CodeModel == "large") {
      TargetMachine::setCodeModel(llvm::CodeModel::Large);
    } else {
      assert(CodeGenOpts.CodeModel.empty() && "Invalid code model!");
      TargetMachine::setCodeModel(llvm::CodeModel::Default);
    }

    std::vector<const char *> BackendArgs;
    BackendArgs.push_back("clang"); // Fake program name.
    if (!CodeGenOpts.DebugPass.empty()) {
      BackendArgs.push_back("-debug-pass");
      BackendArgs.push_back(CodeGenOpts.DebugPass.c_str());
    }
    if (!CodeGenOpts.LimitFloatPrecision.empty()) {
      BackendArgs.push_back("-limit-float-precision");
      BackendArgs.push_back(CodeGenOpts.LimitFloatPrecision.c_str());
    }
    if (llvm::TimePassesIsEnabled)
      BackendArgs.push_back("-time-passes");
    BackendArgs.push_back(0);
    llvm::cl::ParseCommandLineOptions(BackendArgs.size() - 1,
                                      (char**) &BackendArgs[0]);

    std::string FeaturesStr;
    if (TargetOpts.CPU.size() || TargetOpts.Features.size()) {
      SubtargetFeatures Features;
      Features.setCPU(TargetOpts.CPU);
      for (std::vector<std::string>::const_iterator
             it = TargetOpts.Features.begin(),
             ie = TargetOpts.Features.end(); it != ie; ++it)
        Features.AddFeature(*it);
      FeaturesStr = Features.getString();
    }
    TargetMachine *TM = TheTarget->createTargetMachine(Triple, FeaturesStr);

    // Set register scheduler & allocation policy.
    RegisterScheduler::setDefault(createDefaultScheduler);
    RegisterRegAlloc::setDefault(Fast ? createLocalRegisterAllocator :
                                 createLinearScanRegisterAllocator);

    // From llvm-gcc:
    // If there are passes we have to run on the entire module, we do codegen
    // as a separate "pass" after that happens.
    // FIXME: This is disabled right now until bugs can be worked out.  Reenable
    // this for fast -O0 compiles!
    FunctionPassManager *PM = getCodeGenPasses();
    CodeGenOpt::Level OptLevel = CodeGenOpt::Default;

    switch (CodeGenOpts.OptimizationLevel) {
    default: break;
    case 0: OptLevel = CodeGenOpt::None; break;
    case 3: OptLevel = CodeGenOpt::Aggressive; break;
    }

    // Request that addPassesToEmitFile run the Verifier after running
    // passes which modify the IR.
#ifndef NDEBUG
    bool DisableVerify = false;
#else
    bool DisableVerify = true;
#endif

    // Normal mode, emit a .s or .o file by running the code generator. Note,
    // this also adds codegenerator level optimization passes.
    TargetMachine::CodeGenFileType CGFT = TargetMachine::CGFT_AssemblyFile;
    if (Action == Backend_EmitObj)
      CGFT = TargetMachine::CGFT_ObjectFile;
    if (TM->addPassesToEmitFile(*PM, FormattedOutStream, CGFT, OptLevel,
                                DisableVerify)) {
      Diags.Report(diag::err_fe_unable_to_interface_with_target);
      return false;
    }
  }

  return true;
}

void JITConsumer::CreatePasses() {
  unsigned OptLevel = CodeGenOpts.OptimizationLevel;
  CodeGenOptions::InliningMethod Inlining = CodeGenOpts.Inlining;

  // Handle disabling of LLVM optimization, where we want to preserve the
  // internal module before any optimization.
  if (CodeGenOpts.DisableLLVMOpts) {
    OptLevel = 0;
    Inlining = CodeGenOpts.NoInlining;
  }

  // In -O0 if checking is disabled, we don't even have per-function passes.
  if (CodeGenOpts.VerifyModule)
    getPerFunctionPasses()->add(createVerifierPass());

  // Assume that standard function passes aren't run for -O0.
  if (OptLevel > 0)
    llvm::createStandardFunctionPasses(getPerFunctionPasses(), OptLevel);

  llvm::Pass *InliningPass = 0;
  switch (Inlining) {
  case CodeGenOptions::NoInlining: break;
  case CodeGenOptions::NormalInlining: {
    // Set the inline threshold following llvm-gcc.
    //
    // FIXME: Derive these constants in a principled fashion.
    unsigned Threshold = 225;
    if (CodeGenOpts.OptimizeSize)
      Threshold = 75;
    else if (OptLevel > 2)
      Threshold = 275;
    InliningPass = createFunctionInliningPass(Threshold);
    break;
  }
  case CodeGenOptions::OnlyAlwaysInlining:
    InliningPass = createAlwaysInlinerPass();         // Respect always_inline
    break;
  }

  // For now we always create per module passes.
  PassManager *PM = getPerModulePasses();
  llvm::createStandardModulePasses(PM, OptLevel, CodeGenOpts.OptimizeSize,
                                   CodeGenOpts.UnitAtATime,
                                   CodeGenOpts.UnrollLoops,
                                   /*SimplifyLibCalls=*/!LangOpts.NoBuiltin,
                                   /*HaveExceptions=*/true,
                                   InliningPass);
}

/// EmitAssembly - Handle interaction with LLVM backend to generate
/// actual machine code.
void JITConsumer::EmitAssembly() {
  // Silently ignore if we weren't initialized for some reason.
  if (!TheModule || !TheTargetData)
    return;

  TimeRegion Region(llvm::TimePassesIsEnabled ? &CodeGenerationTime : 0);

  // Make sure IR generation is happy with the module. This is
  // released by the module provider.
  Module *M = Gen->ReleaseModule();
  if (!M) {
    // The module has been released by IR gen on failures, do not
    // double free.
    TheModule.take();
    return;
  }

  assert(TheModule.get() == M &&
         "Unexpected module change during IR generation");

  CreatePasses();
  if (!AddEmitPasses())
    return;

  // Run passes. For now we do all passes at once, but eventually we
  // would like to have the option of streaming code generation.

  if (PerFunctionPasses) {
    PrettyStackTraceString CrashInfo("Per-function optimization");

    PerFunctionPasses->doInitialization();
    for (Module::iterator I = M->begin(), E = M->end(); I != E; ++I)
      if (!I->isDeclaration())
        PerFunctionPasses->run(*I);
    PerFunctionPasses->doFinalization();
  }

  if (PerModulePasses) {
    PrettyStackTraceString CrashInfo("Per-module optimization passes");
    PerModulePasses->run(*M);
  }

  if (CodeGenPasses) {
    PrettyStackTraceString CrashInfo("Code generation");
    CodeGenPasses->doInitialization();
    for (Module::iterator I = M->begin(), E = M->end(); I != E; ++I)
      if (!I->isDeclaration())
        CodeGenPasses->run(*I);
    CodeGenPasses->doFinalization();
  }

  

}

//

CodeGenAction::CodeGenAction(unsigned _Act) : Act(_Act) {}

CodeGenAction::~CodeGenAction() {}

void CodeGenAction::EndSourceFileAction() {
  // If the consumer creation failed, do nothing.
  if (!getCompilerInstance().hasASTConsumer())
    return;

  // Steal the module from the consumer.
  JITConsumer *Consumer = static_cast<JITConsumer*>(
    &getCompilerInstance().getASTConsumer());

  TheModule.reset(Consumer->takeModule());
}

llvm::Module *CodeGenAction::takeModule() {
  return TheModule.take();
}

ASTConsumer *CodeGenAction::CreateASTConsumer(CompilerInstance &CI,
                                              llvm::StringRef InFile) {
  BackendAction BA = static_cast<BackendAction>(Act);
  llvm::OwningPtr<llvm::raw_ostream> OS;
  switch (BA) {
  case Backend_EmitAssembly:
    OS.reset(CI.createDefaultOutputFile(false, InFile, "s"));
    break;
  case Backend_EmitLL:
    OS.reset(CI.createDefaultOutputFile(false, InFile, "ll"));
    break;
  case Backend_EmitBC:
    OS.reset(CI.createDefaultOutputFile(true, InFile, "bc"));
    break;
  case Backend_EmitNothing:
    break;
  case Backend_EmitObj:
    OS.reset(CI.createDefaultOutputFile(true, InFile, "o"));
    break;
  }
  if (BA != Backend_EmitNothing && !OS)
    return 0;

  return new JITConsumer(BA, CI.getDiagnostics(), CI.getLangOpts(),
                             CI.getCodeGenOpts(), CI.getTargetOpts(),
                             CI.getFrontendOpts().ShowTimers, InFile, OS.take(),
                             CI.getLLVMContext());
}

EmitAssemblyAction::EmitAssemblyAction()
  : CodeGenAction(Backend_EmitAssembly) {}

EmitBCAction::EmitBCAction() : CodeGenAction(Backend_EmitBC) {}

EmitLLVMAction::EmitLLVMAction() : CodeGenAction(Backend_EmitLL) {}

EmitLLVMOnlyAction::EmitLLVMOnlyAction() : CodeGenAction(Backend_EmitNothing) {}

EmitObjAction::EmitObjAction() : CodeGenAction(Backend_EmitObj) {}



//
JITConsumer *CreateJITConsumer(BackendAction Action,
                                          Diagnostic &Diags,
                                          const LangOptions &LangOpts,
                                          const CodeGenOptions &CodeGenOpts,
                                          const TargetOptions &TargetOpts,
                                          bool TimePasses,
                                          const std::string& InFile,
                                          llvm::raw_ostream* OS,
                                          LLVMContext& C) {
  return new JITConsumer(Action, Diags, LangOpts, CodeGenOpts,
                             TargetOpts, TimePasses, InFile, OS, C);
}

//
int main( void )
{
	llvm::raw_stderr_ostream ost;
	DiagnosticOptions diag_opts;
	diag_opts.BinaryOutput = 0;
	diag_opts.ShowCarets = 1;
	
	TextDiagnosticPrinter tdp(ost, diag_opts);
	Diagnostic diag(&tdp);
	LangOptions lang;
	lang.CPlusPlus = 1;
	lang.Microsoft = 1;

	SourceManager sm;
	FileManager fm;

	HeaderSearch headers(fm);
	

	TargetOptions target_opts;
	target_opts.Triple = LLVM_HOSTTRIPLE;

	TargetInfo *ti = TargetInfo::CreateTargetInfo(diag, target_opts);
	Preprocessor pp(diag, lang, *ti, sm, headers);

	PreprocessorOptions ppio;
	HeaderSearchOptions hsopt;
	//hsopt.AddPath("C:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\include", clang::frontend::System, false, false );

	FrontendOptions feopt;
	InitializePreprocessor(pp, ppio, hsopt, feopt );

	tdp.BeginSourceFile( lang, &pp );

	const FileEntry *file = fm.getFile("t.cpp");
	sm.createMainFileID(file, SourceLocation());
	//pp.EnterMainSourceFile();


	IdentifierTable tab(lang);
	//MyAction action(pp);
	//Parser p(pp, action);
	//p.ParseTranslationUnit();
	//tab.PrintStats();

	SelectorTable sel;
	Builtin::Context builtins(*ti);
	CodeGenOptions codeGenOpts;
	codeGenOpts.DebugInfo = 0;
	codeGenOpts.OptimizationLevel = 3; //TODO: change to 4

	std::string ErrorInfo;
	llvm::raw_ostream* os = new llvm::raw_fd_ostream("t.a",ErrorInfo);

	LLVMContext llvmContext;

	JITConsumer *c = CreateJITConsumer(Backend_EmitLL, diag, lang,
                               codeGenOpts, target_opts,
                               true, "mymodule",
                               os, llvmContext);

	
	ASTContext ctx(lang, sm, *ti, tab, sel, builtins);
	ParseAST(pp, c, ctx, true, true);
	
	c->ExecuteModule();

	tdp.EndSourceFile();

	//Token Tok;

	//do {
	//	pp.Lex(Tok);
	//	if(diag.hasErrorOccurred())
	//		break;
	//	pp.DumpToken(Tok);
	//	std::cerr << std::endl;
	//} while(Tok.isNot(tok::eof));
	//

	return 0;
}


//using namespace clang;
//
//
//class MyASTConsumer : public ASTConsumer {
//public:
//	virtual void HandleTopLevelDecl(DeclGroupRef D) {
//		static int count = 0;
//		DeclGroupRef::iterator it;
//		for(it = D.begin();
//		    it != D.end();
//		    it++) {
//			//std::cout << *it << std::endl;
//			count++;
//			//std::cout << "count: " << count << std::endl;
//			VarDecl *VD = dyn_cast<VarDecl>(*it);
//			if(!VD) continue;
//			std::cout << VD << std::endl;
//			if(VD->isFileVarDecl() &&
//			   VD->getStorageClass() != VarDecl::Extern) {
//				std::cerr << "Read top-level variable decl: '" << VD->getDeclName().getAsString() << "'\n";
//			}
//		}
//	}
//};
//
//
//class MyAction : public MinimalAction {
//	const Preprocessor& pp;
//public:
//	MyAction(Preprocessor& prep)
//		: MinimalAction(prep), pp(prep) {}
//	
//	virtual Action::DeclPtrTy
//	ActOnDeclarator(Scope *S, Declarator &D) {
//		// Print names of global variables. Differentiating between
//		// global variables and global functions is Hard in C, so this
//		// is only an approximation.
//		
//		const DeclSpec& DS = D.getDeclSpec();
//		SourceLocation loc = D.getIdentifierLoc();
//		
//		if (
//			// Only global declarations...
//			D.getContext() == Declarator::FileContext
//			
//			// ...that aren't typedefs or `extern` declarations...
//			&& DS.getStorageClassSpec() != DeclSpec::SCS_extern
//			&& DS.getStorageClassSpec() != DeclSpec::SCS_typedef
//			
//			// ...and no functions...
//			&& !D.isFunctionDeclarator()
//			
//			// ...and in a user header
//			&& !pp.getSourceManager().isInSystemHeader(loc)
//			) {
//			IdentifierInfo *II = D.getIdentifier();
//			std::cerr << "Found global user declarator " << II->getName().str() << std::endl;
//		}
//		
//		return MinimalAction::ActOnDeclarator(S, D);
//	}
//};


//namespace {
//  enum BackendAction {
//    Backend_EmitAssembly,  ///< Emit native assembly files
//    Backend_EmitBC,        ///< Emit LLVM bitcode files
//    Backend_EmitLL,        ///< Emit human-readable LLVM assembly
//    Backend_EmitNothing,   ///< Don't emit anything (benchmarking mode)
//    Backend_EmitObj        ///< Emit native object files
//  };
//
//  class JITConsumer : public ASTConsumer {
//    Diagnostic &Diags;
//    BackendAction Action;
//    const CodeGenOptions &CodeGenOpts;
//    const LangOptions &LangOpts;
//    const TargetOptions &TargetOpts;
//    llvm::raw_ostream *AsmOutStream;
//    llvm::formatted_raw_ostream FormattedOutStream;
//    ASTContext *Context;
//
//    Timer LLVMIRGeneration;
//    Timer CodeGenerationTime;
//
//    llvm::OwningPtr<CodeGenerator> Gen;
//
//    llvm::Module *TheModule;
//    llvm::TargetData *TheTargetData;
//
//    mutable FunctionPassManager *CodeGenPasses;
//    mutable PassManager *PerModulePasses;
//    mutable FunctionPassManager *PerFunctionPasses;
//
//    FunctionPassManager *getCodeGenPasses() const;
//    PassManager *getPerModulePasses() const;
//    FunctionPassManager *getPerFunctionPasses() const;
//
//    void CreatePasses();
//
//    /// AddEmitPasses - Add passes necessary to emit assembly or LLVM IR.
//    ///
//    /// \return True on success.
//    bool AddEmitPasses();
//
//    void EmitAssembly();
//
//  public:
//    JITConsumer(BackendAction action, Diagnostic &_Diags,
//                    const LangOptions &langopts, const CodeGenOptions &compopts,
//                    const TargetOptions &targetopts, bool TimePasses,
//                    const std::string &infile, llvm::raw_ostream *OS,
//                    LLVMContext& C) :
//      Diags(_Diags),
//      Action(action),
//      CodeGenOpts(compopts),
//      LangOpts(langopts),
//      TargetOpts(targetopts),
//      AsmOutStream(OS),
//      LLVMIRGeneration("LLVM IR Generation Time"),
//      CodeGenerationTime("Code Generation Time"),
//      Gen(CreateLLVMCodeGen(Diags, infile, compopts, C)),
//      TheModule(0), TheTargetData(0),
//      CodeGenPasses(0), PerModulePasses(0), PerFunctionPasses(0) {
//
//      if (AsmOutStream)
//        FormattedOutStream.setStream(*AsmOutStream,
//                                     formatted_raw_ostream::PRESERVE_STREAM);
//
//      llvm::TimePassesIsEnabled = TimePasses;
//    }
//
//    ~JITConsumer() {
//      delete TheTargetData;
//      delete CodeGenPasses;
//      delete PerModulePasses;
//      delete PerFunctionPasses;
//    }
//
//    virtual void Initialize(ASTContext &Ctx) {
//      Context = &Ctx;
//
//      if (llvm::TimePassesIsEnabled)
//        LLVMIRGeneration.startTimer();
//
//      Gen->Initialize(Ctx);
//
//      TheModule = Gen->GetModule();
//      //ModuleProvider = new ExistingModuleProvider(TheModule);
//      TheTargetData = new llvm::TargetData(Ctx.Target.getTargetDescription());
//
//      if (llvm::TimePassesIsEnabled)
//        LLVMIRGeneration.stopTimer();
//    }
//
//    virtual void HandleTopLevelDecl(DeclGroupRef D) {
//      PrettyStackTraceDecl CrashInfo(*D.begin(), SourceLocation(),
//                                     Context->getSourceManager(),
//                                     "LLVM IR generation of declaration");
//
//      if (llvm::TimePassesIsEnabled)
//        LLVMIRGeneration.startTimer();
//
//      Gen->HandleTopLevelDecl(D);
//
//      if (llvm::TimePassesIsEnabled)
//        LLVMIRGeneration.stopTimer();
//    }
//
//    virtual void HandleTranslationUnit(ASTContext &C) {
//      {
//        PrettyStackTraceString CrashInfo("Per-file LLVM IR generation");
//        if (llvm::TimePassesIsEnabled)
//          LLVMIRGeneration.startTimer();
//
//        Gen->HandleTranslationUnit(C);
//
//        if (llvm::TimePassesIsEnabled)
//          LLVMIRGeneration.stopTimer();
//      }
//
//      // EmitAssembly times and registers crash info itself.
//      EmitAssembly();
//
//      // Force a flush here in case we never get released.
//      if (AsmOutStream)
//        FormattedOutStream.flush();
//    }
//
//    virtual void HandleTagDeclDefinition(TagDecl *D) {
//      PrettyStackTraceDecl CrashInfo(D, SourceLocation(),
//                                     Context->getSourceManager(),
//                                     "LLVM IR generation of declaration");
//      Gen->HandleTagDeclDefinition(D);
//    }
//
//    virtual void CompleteTentativeDefinition(VarDecl *D) {
//      Gen->CompleteTentativeDefinition(D);
//    }
//    virtual void PrintStats() {
//	std::string str;
//	raw_string_ostream os( str );
//
//	LLVMIRGeneration.print( LLVMIRGeneration, os );
//	CodeGenerationTime.print( LLVMIRGeneration, os );
//	std::cerr << os.str().c_str() << '\n'; 
//    }
//  };
//}
//
//FunctionPassManager *JITConsumer::getCodeGenPasses() const {
//  if (!CodeGenPasses) {
//    CodeGenPasses = new FunctionPassManager(TheModule);
//    CodeGenPasses->add(new TargetData(*TheTargetData));
//  }
//
//  return CodeGenPasses;
//}
//
//PassManager *JITConsumer::getPerModulePasses() const {
//  if (!PerModulePasses) {
//    PerModulePasses = new PassManager();
//    PerModulePasses->add(new TargetData(*TheTargetData));
//  }
//
//  return PerModulePasses;
//}
//
//FunctionPassManager *JITConsumer::getPerFunctionPasses() const {
//  if (!PerFunctionPasses) {
//    PerFunctionPasses = new FunctionPassManager(TheModule);
//    PerFunctionPasses->add(new TargetData(*TheTargetData));
//  }
//
//  return PerFunctionPasses;
//}
//
//bool JITConsumer::AddEmitPasses() {
//  if (Action == Backend_EmitNothing)
//    return true;
//
//  if (Action == Backend_EmitBC) {
//    getPerModulePasses()->add(createBitcodeWriterPass(FormattedOutStream));
//  } else if (Action == Backend_EmitLL) {
//    getPerModulePasses()->add(createPrintModulePass(&FormattedOutStream));
//  } else {
//    bool Fast = CodeGenOpts.OptimizationLevel == 0;
//
//    // Create the TargetMachine for generating code.
//    std::string Error;
//    std::string Triple = TheModule->getTargetTriple();
//    const llvm::Target *TheTarget = TargetRegistry::lookupTarget(Triple, Error);
//    if (!TheTarget) {
//      Diags.Report(diag::err_fe_unable_to_create_target) << Error;
//      return false;
//    }
//
//    // FIXME: Expose these capabilities via actual APIs!!!! Aside from just
//    // being gross, this is also totally broken if we ever care about
//    // concurrency.
//    llvm::NoFramePointerElim = CodeGenOpts.DisableFPElim;
//    if (CodeGenOpts.FloatABI == "soft")
//      llvm::FloatABIType = llvm::FloatABI::Soft;
//    else if (CodeGenOpts.FloatABI == "hard")
//      llvm::FloatABIType = llvm::FloatABI::Hard;
//    else {
//      assert(CodeGenOpts.FloatABI.empty() && "Invalid float abi!");
//      llvm::FloatABIType = llvm::FloatABI::Default;
//    }
//    NoZerosInBSS = CodeGenOpts.NoZeroInitializedInBSS;
//    llvm::UseSoftFloat = CodeGenOpts.SoftFloat;
//    UnwindTablesMandatory = CodeGenOpts.UnwindTables;
//
//    TargetMachine::setAsmVerbosityDefault(CodeGenOpts.AsmVerbose);
//
//    // FIXME: Parse this earlier.
//    if (CodeGenOpts.RelocationModel == "static") {
//      TargetMachine::setRelocationModel(llvm::Reloc::Static);
//    } else if (CodeGenOpts.RelocationModel == "pic") {
//      TargetMachine::setRelocationModel(llvm::Reloc::PIC_);
//    } else {
//      assert(CodeGenOpts.RelocationModel == "dynamic-no-pic" &&
//             "Invalid PIC model!");
//      TargetMachine::setRelocationModel(llvm::Reloc::DynamicNoPIC);
//    }
//    // FIXME: Parse this earlier.
//    if (CodeGenOpts.CodeModel == "small") {
//      TargetMachine::setCodeModel(llvm::CodeModel::Small);
//    } else if (CodeGenOpts.CodeModel == "kernel") {
//      TargetMachine::setCodeModel(llvm::CodeModel::Kernel);
//    } else if (CodeGenOpts.CodeModel == "medium") {
//      TargetMachine::setCodeModel(llvm::CodeModel::Medium);
//    } else if (CodeGenOpts.CodeModel == "large") {
//      TargetMachine::setCodeModel(llvm::CodeModel::Large);
//    } else {
//      assert(CodeGenOpts.CodeModel.empty() && "Invalid code model!");
//      TargetMachine::setCodeModel(llvm::CodeModel::Default);
//    }
//
//    std::vector<const char *> BackendArgs;
//    BackendArgs.push_back("clang"); // Fake program name.
//    if (!CodeGenOpts.DebugPass.empty()) {
//      BackendArgs.push_back("-debug-pass");
//      BackendArgs.push_back(CodeGenOpts.DebugPass.c_str());
//    }
//    if (!CodeGenOpts.LimitFloatPrecision.empty()) {
//      BackendArgs.push_back("-limit-float-precision");
//      BackendArgs.push_back(CodeGenOpts.LimitFloatPrecision.c_str());
//    }
//    if (llvm::TimePassesIsEnabled)
//      BackendArgs.push_back("-time-passes");
//    BackendArgs.push_back(0);
//    llvm::cl::ParseCommandLineOptions(BackendArgs.size() - 1,
//                                      (char**) &BackendArgs[0]);
//
//    std::string FeaturesStr;
//    if (TargetOpts.CPU.size() || TargetOpts.Features.size()) {
//      SubtargetFeatures Features;
//      Features.setCPU(TargetOpts.CPU);
//      for (std::vector<std::string>::const_iterator
//             it = TargetOpts.Features.begin(),
//             ie = TargetOpts.Features.end(); it != ie; ++it)
//        Features.AddFeature(*it);
//      FeaturesStr = Features.getString();
//    }
//    TargetMachine *TM = TheTarget->createTargetMachine(Triple, FeaturesStr);
//
//    // Set register scheduler & allocation policy.
//    RegisterScheduler::setDefault(createDefaultScheduler);
//    RegisterRegAlloc::setDefault(Fast ? createLocalRegisterAllocator :
//                                 createLinearScanRegisterAllocator);
//
//    // From llvm-gcc:
//    // If there are passes we have to run on the entire module, we do codegen
//    // as a separate "pass" after that happens.
//    // FIXME: This is disabled right now until bugs can be worked out.  Reenable
//    // this for fast -O0 compiles!
//    FunctionPassManager *PM = getCodeGenPasses();
//    CodeGenOpt::Level OptLevel = CodeGenOpt::Default;
//
//    switch (CodeGenOpts.OptimizationLevel) {
//    default: break;
//    case 0: OptLevel = CodeGenOpt::None; break;
//    case 3: OptLevel = CodeGenOpt::Aggressive; break;
//    }
//
//    // Normal mode, emit a .s file by running the code generator.
//    // Note, this also adds codegenerator level optimization passes.
//    TM->addPassesToEmitFile(*PM, FormattedOutStream,
//                                    TargetMachine::CGFT_AssemblyFile, OptLevel);
//
//    //if (TM->addPassesToEmitMachineCode(*CodeGenPasses, (JITCodeEmitter *)0,
//    //                                  OptLevel)) {
//    //  Diags.Report(diag::err_fe_unable_to_interface_with_target);
//    //  return false;
//    //}
//  }
//
//  return true;
//}
//
//void JITConsumer::CreatePasses() {
//  //unsigned OptLevel = CodeGenOpts.OptimizationLevel;
//  //CodeGenOptions::InliningMethod Inlining = CodeGenOpts.Inlining;
//
//  //// Handle disabling of LLVM optimization, where we want to preserve the
//  //// internal module before any optimization.
//  //if (CodeGenOpts.DisableLLVMOpts) {
//  //  OptLevel = 0;
//  //  Inlining = CodeGenOpts.NoInlining;
//  //}
//
//  //// In -O0 if checking is disabled, we don't even have per-function passes.
//  //if (CodeGenOpts.VerifyModule)
//  //  getPerFunctionPasses()->add(createVerifierPass());
//
//  //// Assume that standard function passes aren't run for -O0.
//  //if (OptLevel > 0)
//    //llvm::createStandardFunctionPasses(getPerFunctionPasses(), OptLevel);
//
//  //llvm::Pass *InliningPass = 0;
//  //switch (Inlining) {
//  //case CodeGenOptions::NoInlining: break;
//  //case CodeGenOptions::NormalInlining: {
//  //  // Set the inline threshold following llvm-gcc.
//  //  //
//  //  // FIXME: Derive these constants in a principled fashion.
//  //  unsigned Threshold = 200;
//  //  if (CodeGenOpts.OptimizeSize)
//  //    Threshold = 50;
//  //  else if (OptLevel > 2)
//  //    Threshold = 250;
//  //  InliningPass = createFunctionInliningPass(Threshold);
//  //  break;
//  //}
//  //case CodeGenOptions::OnlyAlwaysInlining:
//  //  InliningPass = createAlwaysInlinerPass();         // Respect always_inline
//  //  break;
//  //}
//
//  // For now we always create per module passes.
//  PassManager *PM = getPerModulePasses();
//
//  //PM->add(new TargetData(*ee->getTargetData()));
//  //  PM->add(new TargetData(m));
//  PM->add((Pass*)createVerifierPass());                  // Verify that input is correct
//  PM->add((Pass*)createCFGSimplificationPass());    // Clean up disgusting code
//  PM->add((Pass*)createPromoteMemoryToRegisterPass());// Kill useless allocas
//  PM->add((Pass*)createInstructionCombiningPass()); // Clean up after IPCP & DAE
//  PM->add((Pass*)createCFGSimplificationPass());    // Clean up after IPCP & DAE
//  PM->add((Pass*)createTailDuplicationPass());      // Simplify cfg by copying code
//  PM->add((Pass*)createInstructionCombiningPass()); // Cleanup for scalarrepl.
//  PM->add((Pass*)createCFGSimplificationPass());    // Merge & remove BBs
//  PM->add((Pass*)createScalarReplAggregatesPass()); // Break up aggregate allocas
//  PM->add((Pass*)createInstructionCombiningPass()); // Combine silly seq's
////  PM->add((Pass*)createCondPropagationPass());      // Propagate conditionals
//  PM->add((Pass*)createTailCallEliminationPass());  // Eliminate tail calls
//  PM->add((Pass*)createCFGSimplificationPass());    // Merge & remove BBs
//  PM->add((Pass*)createReassociatePass());          // Reassociate expressions
//  PM->add((Pass*)createLoopRotatePass());
//  PM->add((Pass*)createLICMPass());                 // Hoist loop invariants
//  PM->add((Pass*)createLoopUnswitchPass());         // Unswitch loops.
//  PM->add((Pass*)createInstructionCombiningPass()); // Clean up after LICM/reassoc
//  PM->add((Pass*)createIndVarSimplifyPass());       // Canonicalize indvars
//  //  PM->add((Pass*) createLoopDeletionPass());         // Delete dead loops
//  PM->add((Pass*)createLoopUnrollPass());           // Unroll small loops
//  PM->add((Pass*)createInstructionCombiningPass()); // Clean up after the unroller
//  PM->add((Pass*)createGVNPass());   // GVN for load instructions
////  PM->add((Pass*)createMemCpyPMPass());                 // Remove memcpy / form memset
//  PM->add((Pass*)createSCCPPass());                 // Constant prop with SCCP
//  PM->add((Pass*)createInstructionCombiningPass());
////  PM->add((Pass*)createCondPropagationPass());      // Propagate conditionals
//  PM->add((Pass*)createDeadStoreEliminationPass()); // Delete dead stores
//  PM->add((Pass*)createAggressiveDCEPass());        // SSA based 'Aggressive DCE'
//  PM->add((Pass*)createCFGSimplificationPass());    // Merge & remove BBs
//
//  //llvm::createStandardModulePasses(PM, OptLevel, CodeGenOpts.OptimizeSize,
//  //                                 CodeGenOpts.UnitAtATime,
//  //                                 CodeGenOpts.UnrollLoops,
//  //                                 /*SimplifyLibCalls=*/!LangOpts.NoBuiltin,
//  //                                 /*HaveExceptions=*/true,
//  //                                 InliningPass);
//}
//
///// EmitAssembly - Handle interaction with LLVM backend to generate
///// actual machine code.
//void JITConsumer::EmitAssembly() {
//  // Silently ignore if we weren't initialized for some reason.
//  if (!TheModule || !TheTargetData)
//    return;
//
//  TimeRegion Region(llvm::TimePassesIsEnabled ? &CodeGenerationTime : 0);
//
//  // Make sure IR generation is happy with the module. This is
//  // released by the module provider.
//  Module *M = Gen->ReleaseModule();
//  //if (!M) {
//  //  // The module has been released by IR gen on failures, do not
//  //  // double free.
//  //  delete TheModule;
//  //  TheModule = 0;
//  //  return;
//  //}
//
//
//  assert(TheModule == M && "Unexpected module change during IR generation");
//
//  CreatePasses();
//  if (!AddEmitPasses())
//    return;
//
//  // Run passes. For now we do all passes at once, but eventually we
//  // would like to have the option of streaming code generation.
//
//  if (PerFunctionPasses) {
//    PrettyStackTraceString CrashInfo("Per-function optimization");
//
//    PerFunctionPasses->doInitialization();
//    for (Module::iterator I = M->begin(), E = M->end(); I != E; ++I)
//      if (!I->isDeclaration())
//        PerFunctionPasses->run(*I);
//    PerFunctionPasses->doFinalization();
//  }
//
//  if (PerModulePasses) {
//    PrettyStackTraceString CrashInfo("Per-module optimization passes");
//    PerModulePasses->run(*M);
//  }
//
//  if (CodeGenPasses) {
//    PrettyStackTraceString CrashInfo("Code generation");
//    CodeGenPasses->doInitialization();
//    for (Module::iterator I = M->begin(), E = M->end(); I != E; ++I)
//      if (!I->isDeclaration())
//        CodeGenPasses->run(*I);
//    CodeGenPasses->doFinalization();
//  }
//
//
//  std::string errorstring;
//  InitializeNativeTarget();
//  ExecutionEngine* ee = ExecutionEngine::createJIT(TheModule, &errorstring);
//  Function* fn = ee->FindFunctionNamed( "f_t" );
//  std::vector<GenericValue> arg;
//  ee->runFunction( fn, arg );
//
//}
