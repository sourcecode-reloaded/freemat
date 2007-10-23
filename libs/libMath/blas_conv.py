#!/usr/bin/env python

import os
import re

class Fn:
    def __init__(self, function_name, parameter_string, return_type, call_params ):
        self.function_name = function_name
        self.parameter_string = parameter_string
        self.return_type = return_type
        self.call_params = call_params

f=file('blas_header','r');
lines=f.readlines()
f.close()

fns=[]

for line in lines:
    function_name=re.search('\w+(?=\()',line).group().lower()
    parameter_string=re.search('\(.+\)',line).group()
    return_type = re.search('(?<=extern\s)\S+',line).group()
    call_params = re.findall('(?<=[*,\s])\w+(?=[\,,\)])',line)
    call_params = '( ' + ", ".join(call_params) + ' )'
    fns.append( Fn( function_name, parameter_string, return_type, call_params ) )

fout=file('blas_wrap.cpp','w')

preamble= \
"""//This is a generated file. Do not edit!
#include "blas_wrap.h"
#include <iostream>
#include <vector>
#include <QString>
#include <QLibrary> 

BlasWrapper wrapper;

#ifdef __cplusplus
extern "C" { 
#endif /* __cplusplus */ 
"""        
fout.write(preamble)


# Put function typedefs
for fn in fns:
    fdef = 'typedef ' + fn.return_type + ' (* p_' + fn.function_name + ' )' + fn.parameter_string + ';\n'
    fout.write(fdef)
fout.write('\n\n')

# put wrapper function definitions
i = 0
for fn in fns:
    fdecl = fn.return_type + ' ' + fn.function_name + fn.parameter_string + '\n{\n'
    fdecl = fdecl + '\tif( !wrapper.fn['+ str(i) + '] )\n'
    fdecl = fdecl + '\t\twrapper.fn['+ str(i) + '] = wrapper.Resolve(\"' + fn.function_name + '\");\n'
    if fn.return_type != 'void':
       fdecl = fdecl+ '\treturn '
    else:
       fdecl = fdecl+ '\t'
    fdecl = fdecl + \
    '(* reinterpret_cast<p_'+fn.function_name+'>(wrapper.fn[' + str(i) + ']))' + fn.call_params +';\n'
    fout.write(fdecl+'\n}\n\n')
    i=i+1
nblasfns = i    
fout.write('\n\n')


postamble=\
"""#ifdef __cplusplus 
}
#endif /* __cplusplus */
"""
fout.write( postamble )

class_impl="""
void BlasWrapper::DiscoverBlasLibrary( void )
{
    fileName = QString("libacml_dll.dll");
    blasLib->setFileName( fileName );
    if( ! blasLib->load() ){
	std::cout << blasLib->errorString().toStdString() << "\\n";
    }
}

void* BlasWrapper::Resolve( const char* function_name )
{
    void *p;
    QString fname( function_name );
    if( capitalized )
	fname = fname.toUpper();
    //TODO: handle prefix, suffix.

    if( !(p = blasLib->resolve( fname.toAscii() ))){
	std::cout << blasLib->errorString().toStdString() 
	    << " while loading function: " << fname.toStdString() << "\\n";
    }
    return p;
}

"""
fout.write(class_impl)

#resolve function names
fout.write("""
void BlasWrapper::InitFunctions( void )
{
""")

fout.write("\tfn.resize( %d );\n" % nblasfns)
fout.write("}\n")
fout.close()

class_header="""
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
"""
file('blas_wrap.h','w').write(class_header)


fout=file('blas.h','w')
preamble= \
"""//This is a generated file. Do not edit!
#ifndef __BLAS_H
#define __BLAS_H

#ifdef __cplusplus
extern "C" { 
#endif /* __cplusplus */

"""        
fout.write(preamble)

# Put function typedefs
for fn in fns:
    fdef = fn.return_type + ' ' + fn.function_name + fn.parameter_string + ';\n'
    fout.write(fdef)
fout.write('\n\n')

postamble=\
"""#ifdef __cplusplus 
}
#endif /* __cplusplus */

#endif /*_BLAS_WRAP_H*/
"""
fout.write( postamble )
