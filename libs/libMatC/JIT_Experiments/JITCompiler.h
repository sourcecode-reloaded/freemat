#ifndef __JITCOMPILER_H_
#define __JITCOMPILER_H_

#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

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
#include "llvm/LLVMContext.h"

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
#include "llvm/ADT/OwningPtr.h"
#include "llvm/Support/IRReader.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/Timer.h"

#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QSharedPointer>
#include "llvm/Support/MemoryBuffer.h"

typedef QSharedPointer<llvm::MemoryBuffer> LLVMMemSharedPtr;

class JITCompiler {
private:
    QVector<QString> system_includes;
    QVector<QString> user_includes;
    
    Compiler::Options options;
    QVector<LLVMMemSharedPtr> sources_buffer;

  public:
    JITCompiler();
    ~JITCompiler();
    
    void add_user_include_path( QString code);
    void add_system_include_path(QString code);
    void add_source_from_string( QString code, QString name);
    //add_source_file(const QString& code);
    
    void compile( void );
    void run_function(QString name);
};



#endif