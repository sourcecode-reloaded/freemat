
#ifndef __BLAS_WRAP_H
#define __BLAS_WRAP_H

#include <QString>
#include <QLibrary> 
#include <vector>
typedef long int integer;
typedef char *address;
typedef short int shortint;
typedef float real;
typedef double doublereal;
typedef struct { real r, i; } complex;
typedef struct { doublereal r, i; } doublecomplex;
typedef long int logical;
typedef short int shortlogical;
typedef char logical1;
typedef char integer1;

class BlasWrapper{
    QString fileName;
    QString prefix;
    QString suffix;
    bool capitalized;
    QLibrary* blasLib;
public:
    BlasWrapper() : capitalized( false ) {  blasLib = new QLibrary();};
    ~BlasWrapper() { delete blasLib; };
    void DiscoverBlasLibrary( void );
    void* Resolve( const char* function_name );
    void InitFunctions( void );

public:
    std::vector<void*> fn;
};
#endif /*_BLAS_WRAP_H*/
