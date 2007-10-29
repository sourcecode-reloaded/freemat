#include "blas_wrap.h"
#include "blas_dyn_link.h"
#include <iostream>
#include <vector>
#include <QString>
#include <QLibrary> 

BlasWrapper::BlasWrapper() : capitalized( false ) 
{  
    blasLib = new QLibrary();
    InitFunctions();
    DiscoverBlasLibrary();
}

void BlasWrapper::DiscoverBlasLibrary( void )
{
    //fileName = QString("libacml_dll.dll");
    fileName = QString("blas_ref.dll");
    blasLib->setFileName( fileName );
    if( ! blasLib->load() ){
	std::cout << blasLib->errorString().toStdString() << "\n";
    }
}

void* BlasWrapper::Resolve( const char* function_name )
{
    void *p;
    QString fname( function_name );
    if( capitalized )
	fname = fname.toUpper();
    //TODO: handle prefix, suffix.
    fname = fname + "_";
    if( !(p = blasLib->resolve( fname.toAscii() ))){
	std::cout << blasLib->errorString().toStdString() 
	    << " while loading function: " << fname.toStdString() << "\n";
    }
    return p;
}


void BlasWrapper::InitFunctions( void )
{
    fn.resize( BLAS_NUM_FNS );
}
