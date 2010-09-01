#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS
#include <stdio.h>
#include <iostream>

#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QTextStream>

#include "JITCompiler.h"


//
int main( void )
{
  
  JITCompiler * jit = new JITCompiler();

    QFile f("header_paths.txt");
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text)){
        printf("Error opening: header_paths\n");
    }
    QTextStream in(&f);
    QString path;
    while (!in.atEnd()) {
        path = in.readLine();
	jit->add_system_include_path(path);
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
            continue;
        }
        QTextStream insource(&fsource);
        QString source_code = insource.readAll()+'\0';
	if( !source_code.isEmpty() )
	  jit->add_source_from_string(source_code, path);
    }

    jit->compile();    
    jit->run_function("f_t");
    return 0;
}

