#ifndef __ThreadContext_hpp__
#define __ThreadContext_hpp__

#include "FMLib.hpp"
#include "Breakpoint.hpp"

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
#include <mutex>

namespace FM
{
  class BoolType;
  class DoubleType;
  class SingleType;
  class StringType;
  class IndexType;
  class Int8Type;
  class Int16Type;
  class Int32Type;
  class Int64Type;
  class UInt8Type;
  class UInt16Type;
  class UInt32Type;
  class UInt64Type;
  class StructType;
  class CellType;
  class ListType;
  class ClassMetaType;
  class ClassType;
  class ModuleType;
  class CapturedType;
  class CodeType;
  class VM;
  class Compiler;
  class Assembler;
  class TermIF;
  class ObjectBase;
  class Object;
  class FunctionType;
  class FunctionHandleType;
  class BuiltInType;
  class BoundFunctionType;
  class AnonymousType;
  struct Breakpoint;
  
  struct ThreadContext
  {
    // Type pool
    BoolType *_bool;
    DoubleType *_double;
    SingleType *_single;
    StringType *_string;
    IndexType *_index;
    Int8Type *_int8;
    Int16Type *_int16;
    Int32Type *_int32;
    Int64Type *_int64;
    UInt8Type *_uint8;
    UInt16Type *_uint16;
    UInt32Type *_uint32;
    UInt64Type *_uint64;
    StructType *_struct;
    CellType *_cell;
    ListType *_list;
    CodeType *_code;
    BuiltInType *_builtin;
    ClassMetaType *_meta;
    ClassType *_class;
    FunctionType *_function;
    FunctionHandleType *_functionHandle;
    ModuleType *_module;
    CapturedType *_captured;
    BoundFunctionType *_bound;
    AnonymousType *_anonymous;
    // Tools
    VM *_vm;
    Compiler *_compiler;
    Assembler *_asm;
    TermIF *_io;
    ObjectBase *_empty;
    // Global functions
    std::mutex *_lock;
    std::map<FMString,Object> *_globals;
    std::set<ObjectBase*> _handles;
    BPSet *_bps;
  };

  ThreadContext* BuildNewThreadContext(TermIF *io);
  
};


#endif
