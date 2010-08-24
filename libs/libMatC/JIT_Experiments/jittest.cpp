#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS
#include <stdio.h>
#include <iostream>

#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QTextStream>

#include "Compiler.h"



int main( void )
{
    ClangCompiler::Compiler* c = new ClangCompiler::Compiler();


    QFile f("header_paths.txt");
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text)){
        printf("Error opening: header_paths\n");
        return 1;
    }
    QTextStream in(&f);
    QString path;
    while (!in.atEnd()) {
        path = in.readLine();
        c->options.system_includes.push_back(path);
    }
    f.close();


    f.setFileName("sources.txt");
    if( !f.open(QIODevice::ReadOnly | QIODevice::Text) ){
        printf("Error opening: sources\n");
        return 2;
    }
    in.setDevice(&f);
    while (!in.atEnd()) {
        path = in.readLine();
        QFile fsource(path);
        if( !fsource.open(QIODevice::ReadOnly | QIODevice::Text) ){
            printf("Error opening: %s\n", path.toStdString().c_str());
            return 3;
        }
        QTextStream insource(&fsource);
        QString source_code = insource.readAll();
        c->add_source(source_code);
    }
    f.close();

    c->compile();
    ClangCompiler::JIT* jit;

    jit = c->jit();

    c->dump("t.a");

    //c->link();

    delete c;
    return 0;
}
