#ifndef __JITCOMPILER_H_
#define __JITCOMPILER_H_

#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS


#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QSharedPointer>
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/LLVMContext.h"

typedef QSharedPointer<llvm::MemoryBuffer> LLVMMemSharedPtr;
class JITConsumer;
class JITCompiler {
private:
    QVector<QString> system_includes;
    QVector<QString> user_includes;

    QVector< llvm::MemoryBuffer* > sources_buffer;
    QSharedPointer< JITConsumer > jit;
    llvm::LLVMContext llvmContext;

public:
    JITCompiler();
    ~JITCompiler();

    void add_user_include_path( QString path);
    void add_system_include_path(QString path);
    void add_source_from_string( QString code, QString name);
    //add_source_file(const QString& code);

    void compile( void );
    void run_function(QString name);
};



#endif