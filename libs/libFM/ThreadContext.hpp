#ifndef __ThreadContext_hpp__
#define __ThreadContext_hpp__

#include "FMLib.hpp"

// The context struct is meant to contain (pointers) to all objects needed by the execution
// of the FM code.  Because of the new design of the array classes (and their lack of 
// thread safe reference counting), each VM which operates in a thread must have access
// to it's own context.  All object pools are then allocated with respect to the current
// context.  Sharing variables will be need to be addressed later.  The toolchain is also
// connected to the context, although it's not clear this is strictly necessary.  For sure
// different threads will need their own compilers and VM objects, and the ability to
// inject code at run time means that functions need access to a compiler (consider the
// eval statement).

#include <map>

namespace FM
{
  class BoolType;
  class DoubleType;
  class SingleType;
  class StringType;
  class IndexType;
  class Int32Type;
  class Int64Type;
  class UInt32Type;
  class UInt64Type;
  class StructType;
  class CellType;
  class ListType;
  class ClassMetaType;
  class ClassType;
  class CodeType;
  class VM;
  class Compiler;
  class Assembler;
  class TermIF;
  class ObjectBase;
  class Object;

  struct ThreadContext
  {
    // Type pool
    BoolType *_bool;
    DoubleType *_double;
    SingleType *_single;
    StringType *_string;
    IndexType *_index;
    Int32Type *_int32;
    Int64Type *_int64;
    UInt32Type *_uint32;
    UInt64Type *_uint64;
    StructType *_struct;
    CellType *_cell;
    ListType *_list;
    CodeType *_code;
    ClassMetaType *_meta;
    ClassType *_class;
    // Tools
    VM *_vm;
    Compiler *_compiler;
    Assembler *_asm;
    TermIF *_io;
    ObjectBase *_empty;
    // Global functions
    std::map<FMString,Object> *_globals;
  };

  ThreadContext* BuildNewThreadContext(TermIF *io);
};


#endif
