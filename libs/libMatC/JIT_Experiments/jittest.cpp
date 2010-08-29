#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS
#include <stdio.h>
#include <iostream>

#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QTextStream>


#include <fstream>



//
int main( void )
{
    InitializeNativeTarget();

    //std::filebuf fb;
    //fb.open("t.err",std::ios::out);
    //std::ostream ostr( &fb );
    //llvm::raw_os_ostream ost( ostr );
    std::string ErrorInfo1;
    llvm::raw_ostream * ost = new llvm::raw_fd_ostream("t.err",ErrorInfo1);

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
    lang.GNUKeywords = 1;
    lang.GNUMode = 1;
    lang.Microsoft = 0;
    lang.Bool = 1;
    lang.RTTI = 1;
    //lang.AccessControl = 1;
    lang.ShortWChar = 1;
    lang.Exceptions = 1;
    lang.MathErrno = 1;
    //lang.HeinousExtensions = 1;

    SourceManager sm( diag );
    FileManager fm;

    HeaderSearch headers(fm);


    TargetOptions target_opts;
    target_opts.Triple = LLVM_HOSTTRIPLE;
    
    //target_opts.CXXABI = "microsoft";

    TargetInfo *ti = TargetInfo::CreateTargetInfo(diag, target_opts);
    Preprocessor pp(diag, lang, *ti, sm, headers);

    PreprocessorOptions ppio;
    HeaderSearchOptions hsopt;

    QFile f("header_paths.txt");
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&f);
    QString path;
    while (!in.atEnd()) {
        path = in.readLine();
	
        hsopt.AddPath(path.toStdString(), clang::frontend::System, false, false, false );
    }
    f.close();


    FrontendOptions feopt;
    InitializePreprocessor(pp, ppio, hsopt, feopt );
    pp.getBuiltinInfo().InitializeBuiltins(pp.getIdentifierTable(), pp.getLangOptions().NoBuiltin);
    
    tdp->BeginSourceFile( lang, &pp );

    f.setFileName("sources.txt");
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    in.setDevice(&f);
    path = in.readLine();

    const FileEntry *file = fm.getFile(path.toStdString());
    sm.createMainFileID(file);

    while (!in.atEnd()) {
        path = in.readLine();
        file = fm.getFile(path.toStdString());
        hsopt.AddPath(path.toStdString(), clang::frontend::System, false, false, false );
        sm.createFileID( file, SourceLocation(), SrcMgr::C_User );
    }
    f.close();

    //pp.EnterMainSourceFile();


    //IdentifierTable tab(lang);
    //MyAction action(pp);
    //Parser p(pp, action);
    //p.ParseTranslationUnit();
    //tab.PrintStats();

    //SelectorTable sel;
    
    CodeGenOptions codeGenOpts;
    codeGenOpts.DebugInfo = 0;
    codeGenOpts.OptimizationLevel = 3; //TODO: change to 4

    std::string ErrorInfo;
    llvm::raw_ostream* os = new llvm::raw_fd_ostream("t.a",ErrorInfo);

    LLVMContext llvmContext;
    
    JITConsumer *c = CreateJITConsumer(Backend_EmitLL, diag,
            codeGenOpts, target_opts,
            true, "mymodule",
            os, llvmContext);

    ASTContext ctx(pp.getLangOptions(), pp.getSourceManager(), pp.getTargetInfo(), pp.getIdentifierTable(), pp.getSelectorTable(), pp.getBuiltinInfo(), 0);

    ParseAST(pp, c, ctx, true, true);

    c->ExecuteModule();

    delete c;
    
    tdp->EndSourceFile();

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

