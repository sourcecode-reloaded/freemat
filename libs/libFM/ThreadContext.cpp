#include "ThreadContext.hpp"
#include "AllTypes.hpp"
#include "VM.hpp"
#include "Compiler.hpp"
#include "Assembler.hpp"
#include "HashMap.hpp"
#include "Type.hpp"

using namespace FM;


Type* FM::GetTypeForCode(ThreadContext *ctxt, DataCode code)
{
  switch (code) {
  case TypeInvalid: throw Exception("GetTypeForCode called with invalid type!");
  case TypeCellArray: return ctxt->_cell;
  case TypeStruct: return ctxt->_struct;
  case TypeString: return ctxt->_string;
  case TypeBool: return ctxt->_bool;
  case TypeInt8: return ctxt->_int8;
  case TypeUInt8: return ctxt->_uint8;
  case TypeInt16: return ctxt->_int16;
  case TypeUInt16: return ctxt->_uint16;
  case TypeInt32: return ctxt->_int32;
  case TypeUInt32: return ctxt->_uint32;
  case TypeInt64: return ctxt->_int64;
  case TypeUInt64: return ctxt->_uint64;
  case TypeSingle: return ctxt->_single;
  case TypeDouble: return ctxt->_double;
  case TypeIndex: return ctxt->_index;
  case TypeListArray: return ctxt->_list;
  case TypeMeta: return ctxt->_meta;
  case TypeClass: return ctxt->_class;
  case TypeCode: return ctxt->_code;
  case TypeModule: return ctxt->_module;
  case TypeCaptured: return ctxt->_captured;
  case TypeFunction: return ctxt->_function;
  case TypeBuiltIn: return ctxt->_builtin;
  case TypeBoundFunction: return ctxt->_bound;
  case TypeFunctionHandle: return ctxt->_functionHandle;
  case TypeAnonymous: return ctxt->_anonymous;
  case TypeBreakpoint: return ctxt->_breakpoint;
  default:
    throw Exception("Unknown typecode found");
  }
}

ThreadContext* FM::BuildNewThreadContext(TermIF *io)
{
  ThreadContext *ctxt = new ThreadContext;
  ctxt->_bool = new BoolType(ctxt);
  ctxt->_double = new DoubleType(ctxt);
  ctxt->_empty = ctxt->_double->emptyBase();
  ctxt->_single = new SingleType(ctxt);
  ctxt->_string = new StringType(ctxt);
  ctxt->_index = new IndexType(ctxt);
  ctxt->_int8 = new Int8Type(ctxt);
  ctxt->_int16 = new Int16Type(ctxt);
  ctxt->_int32 = new Int32Type(ctxt);
  ctxt->_int64 = new Int64Type(ctxt);
  ctxt->_uint8 = new UInt8Type(ctxt);
  ctxt->_uint16 = new UInt16Type(ctxt);
  ctxt->_uint32 = new UInt32Type(ctxt);
  ctxt->_uint64 = new UInt64Type(ctxt);
  ctxt->_struct = new StructType(ctxt);
  ctxt->_cell = new CellType(ctxt);
  ctxt->_list = new ListType(ctxt);
  ctxt->_code = new CodeType(ctxt);
  ctxt->_module = new ModuleType(ctxt);
  ctxt->_function = new FunctionType(ctxt);
  ctxt->_functionHandle = new FunctionHandleType(ctxt);
  ctxt->_anonymous = new AnonymousType(ctxt);
  ctxt->_builtin = new BuiltInType(ctxt);
  ctxt->_vm = new VM(ctxt);
  ctxt->_compiler = new Compiler(ctxt);
  ctxt->_asm = new Assembler(ctxt);
  ctxt->_meta = new ClassMetaType(ctxt);
  ctxt->_class = new ClassType(ctxt);
  ctxt->_captured = new CapturedType(ctxt);
  ctxt->_bound = new BoundFunctionType(ctxt);
  ctxt->_breakpoint = new BreakpointType(ctxt);
  ctxt->_io = io;
  return ctxt;
}
