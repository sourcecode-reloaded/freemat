#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS
#include <stdio.h>
#include <iostream>

#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QTextStream>

#include "JITCompiler.h"

#include "llvm/Support/Timer.h"

//
int main( void )
{

    JITCompiler * jit = new JITCompiler();
    llvm::TimerGroup tg("Source Compile");
    llvm::Timer timer_setup("Source Setup", tg);
    timer_setup.startTimer();
    
    QFile f("header_paths.txt");
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
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
    if ( !f.open(QIODevice::ReadOnly | QIODevice::Text) ) {
        printf("Error opening: sources\n");
        return 2;
    }
    in.setDevice(&f);
    while (!in.atEnd()) {
        path = in.readLine();
	try{
	  jit->add_source_file( path );
	}
	catch(...){
	  continue;
	}
/*        QFile fsource(path);
        if ( !fsource.open(QIODevice::ReadOnly | QIODevice::Text) ) {
            printf("Error opening: %s\n", path.toStdString().c_str());
            continue;
        }
        QTextStream insource(&fsource);
        QString source_code = insource.readAll()+'\0';
	if( !source_code.isEmpty() )
	  jit->add_source_from_string(source_code, path);*/
    }

    timer_setup.stopTimer();
    llvm::Timer timer_compile("Compile Time",tg);
    timer_compile.startTimer();
    jit->compile();
    timer_compile.stopTimer();
    
    llvm::Timer timer_run("Run time",tg);
    timer_run.startTimer();
    jit->run_function("f_t");
    timer_run.stopTimer();
    return 0;
}

// kate: indent-mode cstyle; space-indent on; indent-width 0; 
