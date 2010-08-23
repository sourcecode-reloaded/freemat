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
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&f);
    QString path;
    while (!in.atEnd()) {
        path = in.readLine();
        c->options.system_includes.push_back(path);
    }
    f.close();


    f.setFileName("sources.txt");
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    in.setDevice(&f);
    while (!in.atEnd()) {
        path = in.readLine();
        QFile fsource(path);
        f.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream insource(&f);
        QString source_code = insource.read(1000000);
        c->add_source(source_code);
    }
    f.close();

    c->compile();
    ClangCompiler::JIT* jit;

    jit = c->jit();

    //c->link();


    delete c;
    return 0;
}
