#ifndef __JITCOMPILER_H_
#define __JITCOMPILER_H_

#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QSharedPointer>
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/ExecutionEngine/JIT.h"

typedef QSharedPointer<llvm::MemoryBuffer> LLVMMemSharedPtr;
class JITConsumer;
class JITCompiler {
private:
    QVector<QString> system_includes;
    QVector<QString> user_includes;

    QVector< llvm::MemoryBuffer* > sources_buffer;
    QVector< QString > bc_files;
    
    JITConsumer* jit;
    llvm::LLVMContext llvmContext;

    llvm::Module* TheModule;
    llvm::ExecutionEngine* ee;
    
public:
    JITCompiler();
    ~JITCompiler();

    void add_user_include_path( QString path);
    void add_system_include_path(QString path);
    void add_source_from_string( QString code, QString name);
    void add_source_file(const QString& code);
    void add_bc_file( const QString& code);

    void add_builtins( void );
    void compile( void );
    void run_function(QString name);
    
    llvm::Module* getModule( void ){ return TheModule; }
    void SaveBitcode( QString path );
};



#endif