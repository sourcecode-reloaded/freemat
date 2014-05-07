#!/usr/bin/python

def iscomplextype(atype):
  return atype.find('Complex') >= 0

def isintegertype(atype):
  return atype.find('Int') >= 0

def issingletype(atype):
  return atype.find('Single') >= 0

def computeTypes(atype,btype):
  outputcomplex = iscomplextype(atype) or iscomplextype(btype)
  singleprecisioncase = issingletype(atype) and issingletype(btype)
  if (singleprecisioncase):
    if (outputcomplex):
      viatype = 'ComplexSingle'
    else:
      viatype = 'Single'
  else:
    if (outputcomplex):
      viatype = 'ComplexDouble'
    else:
      viatype = 'Double'
  

typelist = ['Double','ComplexDouble','Single','ComplexSingle','UInt64','ComplexUInt64']
typemap = {}
for t in typelist:
  typemap[t] = str.lower(t)

f = open("DotOperator.hpp","w")
f.write('template <class Op>\n')
f.write('FMObject3 DotOperator(const FMObject3 &a, const FMObject3 &b) {\n')
f.write('  if (a.type() == b.type()) {\n')
f.write('    FMObject3 c = a.type()->zeroArrayOfSize(FMTuple::computeDotOperatorSize(a.dims(),b.dims()));\n')
f.write('    switch (a.type()->code())\n')
f.write('    {\n')
for t in typelist:
  inputType = t
  outputType = t
  f.write('        case ' + t + ':\n')
  f.write('          Op::template func_st<'+typemap[t]+'>(c.readWriteData(),a.readOnlyData(),b.readOnlyData(),a.isScalar(),b.isScalar(),c.elementCount());\n')
  f.write('          break;\n')
f.write('    }\n')
f.write('  }\n')
f.write('  if (a.type()->code() == Double) {\n')
f.write('    FMObject3 c = b.type()->zeroArrayOfSize(FMTuple::computeDotOperatorSize(a.dims(),b.dims()));\n')
f.write('    switch (a.type()->code())\n')
f.write('    {\n')
for t in typelist:
  inputType = t
  outputType = t
  f.write('        case ' + t + ':\n')
  f.write('          Op::template func_st<'+typemap[t]+'>(c.readWriteData(),a.readOnlyData(),b.readOnlyData(),a.isScalar(),b.isScalar(),c.elementCount());\n')
  f.write('          break;\n')
f.write('    }\n')
f.write('  }\n')

f.write('}\n')
