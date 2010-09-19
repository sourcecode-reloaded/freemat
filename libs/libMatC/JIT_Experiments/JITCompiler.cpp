#include "JITCompiler.h"
#include <stdio.h>

#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Support/raw_ostream.h>

#include <clang/Frontend/DiagnosticOptions.h>
#include <clang/Basic/Builtins.h>
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
//#include <clang/Parse/Action.h>
#include <clang/Parse/Parser.h>
#include "clang/Parse/ParseAST.h"
#include <llvm/Target/TargetSelect.h>
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/GenericValue.h"

#include "clang/CodeGen/CodeGenAction.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/DeclGroup.h"
#include "clang/CodeGen/BackendUtil.h"

#include "clang/CodeGen/ModuleBuilder.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendDiagnostic.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/Linker.h"
#include "llvm/ADT/OwningPtr.h"
#include "llvm/Support/IRReader.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/StandardPasses.h"
#include "llvm/Support/Timer.h"
#include "llvm/Target/TargetData.h"

using namespace clang;
using namespace llvm;


class JITConsumer : public ASTConsumer {
    Diagnostic &Diags;
    BackendAction Action;
    const CodeGenOptions &CodeGenOpts;
    const TargetOptions &TargetOpts;
    llvm::raw_ostream *AsmOutStream;
    ASTContext *Context;
    Timer LLVMIRGeneration;
    llvm::OwningPtr<CodeGenerator> Gen;
    llvm::OwningPtr<llvm::Module> TheModule;

public:
    JITConsumer(BackendAction action, Diagnostic &_Diags,
                const CodeGenOptions &compopts,
                const TargetOptions &targetopts, bool TimePasses,
                const std::string &infile, llvm::raw_ostream *OS,
                LLVMContext &C) :
            Diags(_Diags),
            Action(action),
            CodeGenOpts(compopts),
            TargetOpts(targetopts),
            AsmOutStream(OS),
            LLVMIRGeneration("LLVM IR Generation Time"),
            Gen(CreateLLVMCodeGen(Diags, infile, compopts, C)) {
        llvm::TimePassesIsEnabled = TimePasses;
    }

    llvm::Module *takeModule() {
        return TheModule.take();
    }

    virtual void Initialize(ASTContext &Ctx) {
        Context = &Ctx;

        if (llvm::TimePassesIsEnabled)
            LLVMIRGeneration.startTimer();

        Gen->Initialize(Ctx);

        TheModule.reset(Gen->GetModule());

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

        // Silently ignore if we weren't initialized for some reason.
        if (!TheModule)
            return;

        // Make sure IR generation is happy with the module. This is released by
        // the module provider.
        Module *M = Gen->ReleaseModule();
        if (!M) {
            // The module has been released by IR gen on failures, do not double
            // free.
            TheModule.take();
            return;
        }

        assert(TheModule.get() == M &&
               "Unexpected module change during IR generation");

        // Install an inline asm handler so that diagnostics get printed through
        // our diagnostics hooks.
        LLVMContext &Ctx = TheModule->getContext();
        void *OldHandler = Ctx.getInlineAsmDiagnosticHandler();
        void *OldContext = Ctx.getInlineAsmDiagnosticContext();
        Ctx.setInlineAsmDiagnosticHandler((void*)(intptr_t)InlineAsmDiagHandler,
                                          this);



        EmitBackendOutput(Diags, CodeGenOpts, TargetOpts,
                          TheModule.get(), Action, AsmOutStream);

        Ctx.setInlineAsmDiagnosticHandler(OldHandler, OldContext);
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

    virtual void HandleVTable(CXXRecordDecl *RD, bool DefinitionRequired) {
        Gen->HandleVTable(RD, DefinitionRequired);
    }

    static void InlineAsmDiagHandler(const llvm::SMDiagnostic &SM,void *Context,
                                     unsigned LocCookie) {
        SourceLocation Loc = SourceLocation::getFromRawEncoding(LocCookie);
        ((JITConsumer*)Context)->InlineAsmDiagHandler2(SM, Loc);
    }

    void InlineAsmDiagHandler2(const llvm::SMDiagnostic &,
                               SourceLocation LocCookie);

};

/// ConvertBackendLocation - Convert a location in a temporary llvm::SourceMgr
/// buffer to be a valid FullSourceLoc.
static FullSourceLoc ConvertBackendLocation(const llvm::SMDiagnostic &D,
        SourceManager &CSM) {
    // Get both the clang and llvm source managers.  The location is relative to
    // a memory buffer that the LLVM Source Manager is handling, we need to add
    // a copy to the Clang source manager.
    const llvm::SourceMgr &LSM = *D.getSourceMgr();

    // We need to copy the underlying LLVM memory buffer because llvm::SourceMgr
    // already owns its one and clang::SourceManager wants to own its one.
    const MemoryBuffer *LBuf =
        LSM.getMemoryBuffer(LSM.FindBufferContainingLoc(D.getLoc()));

    // Create the copy and transfer ownership to clang::SourceManager.
    llvm::MemoryBuffer *CBuf =
        llvm::MemoryBuffer::getMemBufferCopy(LBuf->getBuffer(),
                                             LBuf->getBufferIdentifier());
    FileID FID = CSM.createFileIDForMemBuffer(CBuf);

    // Translate the offset into the file.
    unsigned Offset = D.getLoc().getPointer()  - LBuf->getBufferStart();
    SourceLocation NewLoc =
        CSM.getLocForStartOfFile(FID).getFileLocWithOffset(Offset);
    return FullSourceLoc(NewLoc, CSM);
}

/// InlineAsmDiagHandler2 - This function is invoked when the backend hits an
/// error parsing inline asm.  The SMDiagnostic indicates the error relative to
/// the temporary memory buffer that the inline asm parser has set up.
void JITConsumer::InlineAsmDiagHandler2(const llvm::SMDiagnostic &D,
                                        SourceLocation LocCookie) {
    // There are a couple of different kinds of errors we could get here.  First,
    // we re-format the SMDiagnostic in terms of a clang diagnostic.

    // Strip "error: " off the start of the message string.
    llvm::StringRef Message = D.getMessage();
    if (Message.startswith("error: "))
        Message = Message.substr(7);

    // If the SMDiagnostic has an inline asm source location, translate it.
    FullSourceLoc Loc;
    if (D.getLoc() != SMLoc())
        Loc = ConvertBackendLocation(D, Context->getSourceManager());

    // If this problem has clang-level source location information, report the
    // issue as being an error in the source with a note showing the instantiated
    // code.
    if (LocCookie.isValid()) {
        Diags.Report(FullSourceLoc(LocCookie, Context->getSourceManager()),
                     diag::err_fe_inline_asm).AddString(Message);

        if (D.getLoc().isValid())
            Diags.Report(Loc, diag::note_fe_inline_asm_here);

        return;
    }

    // Otherwise, report the backend error as occuring in the generated .s file.
    // If Loc is invalid, we still need to report the error, it just gets no
    // location info.
    Diags.Report(Loc, diag::err_fe_inline_asm).AddString(Message);
}

//

CodeGenAction::CodeGenAction(unsigned _Act) : Act(_Act) {}

CodeGenAction::~CodeGenAction() {}

bool CodeGenAction::hasIRSupport() const {
    return true;
}

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

static raw_ostream *GetOutputStream(CompilerInstance &CI,
                                    llvm::StringRef InFile,
                                    BackendAction Action) {

    switch (Action) {
    case Backend_EmitAssembly:
        return CI.createDefaultOutputFile(false, InFile, "s");

    case Backend_EmitLL:
        return CI.createDefaultOutputFile(false, InFile, "ll");

    case Backend_EmitBC:
        return CI.createDefaultOutputFile(true, InFile, "bc");

    case Backend_EmitNothing:
        return 0;
    case Backend_EmitMCNull:

    case Backend_EmitObj:
        return CI.createDefaultOutputFile(true, InFile, "o");

    }

    assert(0 && "Invalid action!");
    return 0;
}

ASTConsumer *CodeGenAction::CreateASTConsumer(CompilerInstance &CI,
        llvm::StringRef InFile) {
    BackendAction BA = static_cast<BackendAction>(Act);
    llvm::OwningPtr<llvm::raw_ostream> OS(GetOutputStream(CI, InFile, BA));
    if (BA != Backend_EmitNothing && !OS)
        return 0;

    return new JITConsumer(BA, CI.getDiagnostics(),
                           CI.getCodeGenOpts(), CI.getTargetOpts(),
                           CI.getFrontendOpts().ShowTimers, InFile, OS.take(),
                           CI.getLLVMContext());
}

void CodeGenAction::ExecuteAction() {
    // If this is an IR file, we have to treat it specially.
    if (getCurrentFileKind() == IK_LLVM_IR) {
        BackendAction BA = static_cast<BackendAction>(Act);
        CompilerInstance &CI = getCompilerInstance();
        raw_ostream *OS = GetOutputStream(CI, getCurrentFile(), BA);
        if (BA != Backend_EmitNothing && !OS)
            return;

        bool Invalid;
        SourceManager &SM = CI.getSourceManager();
        const llvm::MemoryBuffer *MainFile = SM.getBuffer(SM.getMainFileID(),
                                             &Invalid);
        if (Invalid)
            return;

        // FIXME: This is stupid, IRReader shouldn't take ownership.
        llvm::MemoryBuffer *MainFileCopy =
            llvm::MemoryBuffer::getMemBufferCopy(MainFile->getBuffer(),
                                                 getCurrentFile().c_str());

        llvm::SMDiagnostic Err;
        TheModule.reset(ParseIR(MainFileCopy, Err, CI.getLLVMContext()));
        if (!TheModule) {
            // Translate from the diagnostic info to the SourceManager location.
            SourceLocation Loc = SM.getLocation(
                                     SM.getFileEntryForID(SM.getMainFileID()), Err.getLineNo(),
                                     Err.getColumnNo() + 1);

            // Get a custom diagnostic for the error. We strip off a leading
            // diagnostic code if there is one.
            llvm::StringRef Msg = Err.getMessage();
            if (Msg.startswith("error: "))
                Msg = Msg.substr(7);
            unsigned DiagID = CI.getDiagnostics().getCustomDiagID(Diagnostic::Error,
                              Msg);

            CI.getDiagnostics().Report(FullSourceLoc(Loc, SM), DiagID);
            return;
        }

        EmitBackendOutput(CI.getDiagnostics(), CI.getCodeGenOpts(),
                          CI.getTargetOpts(), TheModule.get(),
                          BA, OS);
        return;
    }

    // Otherwise follow the normal AST path.
    this->ASTFrontendAction::ExecuteAction();
}

//
EmitAssemblyAction::EmitAssemblyAction()
        : CodeGenAction(Backend_EmitAssembly) {}

EmitBCAction::EmitBCAction() : CodeGenAction(Backend_EmitBC) {}

EmitLLVMAction::EmitLLVMAction() : CodeGenAction(Backend_EmitLL) {}

EmitLLVMOnlyAction::EmitLLVMOnlyAction() : CodeGenAction(Backend_EmitNothing) {}

EmitCodeGenOnlyAction::EmitCodeGenOnlyAction() : CodeGenAction(Backend_EmitMCNull) {}

EmitObjAction::EmitObjAction() : CodeGenAction(Backend_EmitObj) {}

JITConsumer *CreateJITConsumer(BackendAction Action,
                               Diagnostic &Diags,
                               const CodeGenOptions &CodeGenOpts,
                               const TargetOptions &TargetOpts,
                               bool TimePasses,
                               const std::string& InFile,
                               llvm::raw_ostream* OS,
                               LLVMContext& C) {
    return new JITConsumer(Action, Diags, CodeGenOpts,
                           TargetOpts, TimePasses, InFile, OS, C);
}



JITCompiler::JITCompiler()
{
    InitializeNativeTarget();
    TheModule = NULL;
}

JITCompiler::~JITCompiler()
{
    delete TheModule;
}

void JITCompiler::add_user_include_path( QString path)
{
    this->user_includes.push_back(path);
}

void JITCompiler::add_system_include_path(QString path)
{
    this->system_includes.push_back(path);
}

void JITCompiler::add_source_from_string( QString code, QString name)
{
    llvm::StringRef src( *(new std::string(code.toStdString()))); //TODO: fix mem leak here

    llvm::MemoryBuffer *buffer = llvm::MemoryBuffer::getMemBuffer(src,  name.toStdString());
    if (!buffer) {
        printf("couldn't create buffer\n");
    }
    this->sources_buffer.push_back(buffer);
}
//add_source_file(const QString& code);

void JITCompiler::compile( void )
{

    //std::filebuf fb;
    //fb.open("t.err",std::ios::out);
    //std::ostream ostr( &fb );
    //llvm::raw_os_ostream ost( ostr );
    std::string ErrorInfo1;
    llvm::raw_ostream * ost = new llvm::raw_fd_ostream("t.err",ErrorInfo1); //TODO: fix this memory leak

    //llvm::raw_stderr_ostream ost;

    DiagnosticOptions diag_opts;
    diag_opts.BinaryOutput = 0;
    diag_opts.ShowCarets = 1;

    TextDiagnosticPrinter *tdp = new TextDiagnosticPrinter(*ost, diag_opts);
    Diagnostic diag(tdp);
    LangOptions lang;
    lang.CPlusPlus = 1;
    lang.C99 = 1;
    lang.CPlusPlus0x = 0;
    lang.GNUKeywords = 0;
    lang.GNUMode = 0;
    lang.Microsoft = 0;
    lang.Bool = 1;
    lang.RTTI = 1;
    //lang.AccessControl = 1;
    lang.ShortWChar = 1;
    lang.Exceptions = 1;
    lang.MathErrno = 1;
    //lang.HeinousExtensions = 1;

    FileManager fm;

    HeaderSearch headers(fm);


    TargetOptions target_opts;
    target_opts.Triple = LLVM_HOSTTRIPLE;

    //target_opts.CXXABI = "microsoft";

    TargetInfo *ti = TargetInfo::CreateTargetInfo(diag, target_opts);
    PreprocessorOptions ppio;
    HeaderSearchOptions hsopt;

    foreach( QString p, system_includes ) {
        hsopt.AddPath(p.toStdString(), clang::frontend::System, false, false, false );
    }
    foreach( QString p, user_includes ) {
        hsopt.AddPath(p.toStdString(), clang::frontend::Angled, true, false, false );
    }

    
    CodeGenOptions codeGenOpts;
    codeGenOpts.DebugInfo = 0;
    codeGenOpts.OptimizationLevel = 4; //TODO: change to 4
    codeGenOpts.TimePasses = 1;

    foreach( llvm::MemoryBuffer* pBuf, sources_buffer ) {
        SourceManager sm( diag );

        FrontendOptions feopt;
        headers.ClearFileInfo();
        ApplyHeaderSearchOptions(headers, hsopt, lang, ti->getTriple() );
        Preprocessor pp(diag, lang, *ti, sm, headers);
        
        InitializePreprocessor(pp, ppio, hsopt, feopt );
        pp.getBuiltinInfo().InitializeBuiltins(pp.getIdentifierTable(), pp.getLangOptions().NoBuiltin);
        
        std::string predefines = pp.getPredefines();
        predefines.append("#define QT_ARCH_GENERIC\n");
        pp.setPredefines(predefines);
        
        
        printf("Predefs: %s\n", pp.getPredefines().c_str() );
        
        tdp->BeginSourceFile( lang, &pp );


       // printf("File: %s\n%s\n",pBuf->getBufferIdentifier(), pBuf->getBufferStart());
        
        sm.createMainFileIDForMemBuffer(pBuf);


        std::string ErrorInfo;
        std::string llout_name( pBuf->getBufferIdentifier() );
        std::string llout_name2( pBuf->getBufferIdentifier() );
        llout_name.append(".a");

        llvm::raw_ostream* os = new llvm::raw_fd_ostream(llout_name.c_str(),ErrorInfo);

//         PreprocessorOutputOptions ppopts;
//         ppopts.ShowHeaderIncludes=1;
//         DoPrintPreprocessedInput(pp,os,ppopts);
//         continue;

        jit = CreateJITConsumer(Backend_EmitLL, diag,
                                           codeGenOpts, target_opts,
                                           true, llout_name2,
                                           os, llvmContext);

                                           
        ASTContext ctx(pp.getLangOptions(), pp.getSourceManager(), pp.getTargetInfo(), pp.getIdentifierTable(), pp.getSelectorTable(), pp.getBuiltinInfo(), 0);

        jit->Initialize(ctx);
        ParseAST(pp, jit, ctx, false, true);

        if ( !TheModule ) {
            TheModule = jit->takeModule();
        }
        else {
            std::string err;
            llvm::Module * child = jit->takeModule();
            if( child ){
              llvm::Linker::LinkModules(TheModule, child, &err);
              delete child;
            }
            if (err.length()) {
                printf("link error %s\n", err.data());
            }

        }
        tdp->EndSourceFile();
        delete jit;

    }

}

void JITCompiler::run_function(QString name)
{
    std::string errorstring;
    ExecutionEngine* ee = ExecutionEngine::createJIT(TheModule, &errorstring);
    
    llvm::raw_ostream * os = new llvm::raw_fd_ostream("moduledump.txt",errorstring); //TODO: fix this memory leak
    // Create the optimizer thingy
    
  PassManager* opt = new PassManager();
  
  opt->add(new TargetData(TheModule));
  
  createStandardFunctionPasses(opt,4);
  createStandardModulePasses(opt, 4, false, true, true, true, true, NULL);
  createStandardLTOPasses(opt, false, true, true);
  createStandardFunctionPasses(opt,4);
    opt->run(*TheModule);
    TheModule->print(*os,NULL);
    
    
    Function* fn = ee->FindFunctionNamed( name.toStdString().c_str() );
    delete os;
    if ( fn ) {
        std::vector<GenericValue> arg;
        GenericValue ret = ee->runFunction( fn, arg );
        ee->freeMachineCodeForFunction(fn);
    }
    else {
        printf("Error: %s\n", errorstring.c_str());
    }
}

// kate: indent-mode cstyle; space-indent on; indent-width 0; 
