#include "ThreadContext.hpp"
#include "AllTypes.hpp"
#include "VM.hpp"
#include "Compiler.hpp"
#include "Assembler.hpp"
#include "HashMap.hpp"
#include "Type.hpp"

using namespace FM;


Type* FM::ThreadContext::GetTypeForCode(ThreadContext *ctxt, DataCode code)
{
  switch (code) {
  case TypeInvalid: throw Exception("GetTypeForCode called with invalid type!");
  case TypeCellArray: return ctxt->_cell;
  case TypeStruct: return ctxt->_struct;
  case TypeString: return ctxt->_string;
  case TypeBool: return ctxt->_bool;
  case TypeInt8: return ctxt->t_int8;
  case TypeUInt8: return ctxt->_uint8;
  case TypeInt16: return ctxt->t_int16;
  case TypeUInt16: return ctxt->_uint16;
  case TypeInt32: return ctxt->t_int32;
  case TypeUInt32: return ctxt->_uint32;
  case TypeInt64: return ctxt->t_int64;
  case TypeUInt64: return ctxt->_uint64;
  case TypeSingle: return ctxt->_single;
  case TypeDouble: return ctxt->_double;
  case TypeZInt8: return ctxt->t_zint8;
  case TypeZUInt8: return ctxt->_zuint8;
  case TypeZInt16: return ctxt->t_zint16;
  case TypeZUInt16: return ctxt->_zuint16;
  case TypeZInt32: return ctxt->t_zint32;
  case TypeZUInt32: return ctxt->_zuint32;
  case TypeZInt64: return ctxt->t_zint64;
  case TypeZUInt64: return ctxt->_zuint64;
  case TypeZSingle: return ctxt->_zsingle;
  case TypeZDouble: return ctxt->_zdouble;
  case TypeIndex: return ctxt->_index;
  case TypeListArray: return ctxt->_list;
  case TypeMeta: return ctxt->_meta;
  case TypeClass: return ctxt->_class;
  case TypeCode: return ctxt->_code;
  case TypeModule: return ctxt->_module;
  case TypeCaptured: return ctxt->_captured;
  case TypeFunction: return ctxt->_function;
  case TypeBoundFunction: return ctxt->_bound;
  case TypeFunctionHandle: return ctxt->_functionHandle;
  case TypeAnonymous: return ctxt->_anonymous;
  case TypeBreakpoint: return ctxt->_breakpoint;
  case TypeSparseLogical: return ctxt->_splogical;
  case TypeSparseDouble: return ctxt->_spdouble;
  case TypeSparseComplex: return ctxt->_spcomplex;
  default:
    throw Exception("Unknown typecode found");
  }
}

Type* FM::ThreadContext::GetComplexTypeForCode(ThreadContext *ctxt, DataCode code)
{
  switch (code) {
  case TypeInt8: return ctxt->t_zint8;
  case TypeUInt8: return ctxt->_zuint8;
  case TypeInt16: return ctxt->t_zint16;
  case TypeUInt16: return ctxt->_zuint16;
  case TypeInt32: return ctxt->t_zint32;
  case TypeUInt32: return ctxt->_zuint32;
  case TypeInt64: return ctxt->t_zint64;
  case TypeUInt64: return ctxt->_zuint64;
  case TypeSingle: return ctxt->_zsingle;
  case TypeDouble: return ctxt->_zdouble;
  case TypeZInt8: return ctxt->t_zint8;
  case TypeZUInt8: return ctxt->_zuint8;
  case TypeZInt16: return ctxt->t_zint16;
  case TypeZUInt16: return ctxt->_zuint16;
  case TypeZInt32: return ctxt->t_zint32;
  case TypeZUInt32: return ctxt->_zuint32;
  case TypeZInt64: return ctxt->t_zint64;
  case TypeZUInt64: return ctxt->_zuint64;
  case TypeZSingle: return ctxt->_zsingle;
  case TypeZDouble: return ctxt->_zdouble;
  case TypeSparseDouble: return ctxt->_spcomplex;
  case TypeSparseComplex: return ctxt->_spcomplex;
  default:
    throw Exception("GetComplexTypeForCode called with illegal type code");
  }
}



ThreadContext* FM::ThreadContext::BuildNewThreadContext(TermIF *io)
{
  ThreadContext *ctxt = new ThreadContext;
  ctxt->_bool = new BoolType(ctxt);
  ctxt->_double = new DoubleType(ctxt);
  ctxt->_zdouble = new ComplexDoubleType(ctxt);
  ctxt->_empty = ctxt->_double->emptyBase();
  ctxt->_single = new SingleType(ctxt);
  ctxt->_zsingle = new ComplexSingleType(ctxt);
  ctxt->_string = new StringType(ctxt);
  ctxt->_index = new IndexType(ctxt);
  ctxt->t_int8 = new Int8Type(ctxt);
  ctxt->t_int16 = new Int16Type(ctxt);
  ctxt->t_int32 = new Int32Type(ctxt);
  ctxt->t_int64 = new Int64Type(ctxt);
  ctxt->t_zint8 = new ComplexInt8Type(ctxt);
  ctxt->t_zint16 = new ComplexInt16Type(ctxt);
  ctxt->t_zint32 = new ComplexInt32Type(ctxt);
  ctxt->t_zint64 = new ComplexInt64Type(ctxt);
  ctxt->_uint8 = new UInt8Type(ctxt);
  ctxt->_uint16 = new UInt16Type(ctxt);
  ctxt->_uint32 = new UInt32Type(ctxt);
  ctxt->_uint64 = new UInt64Type(ctxt);
  ctxt->_zuint8 = new ComplexUInt8Type(ctxt);
  ctxt->_zuint16 = new ComplexUInt16Type(ctxt);
  ctxt->_zuint32 = new ComplexUInt32Type(ctxt);
  ctxt->_zuint64 = new ComplexUInt64Type(ctxt);
  ctxt->_struct = new StructType(ctxt);
  ctxt->_splogical = new SparseLogicalType(ctxt);
  ctxt->_spdouble = new SparseDoubleType(ctxt);
  ctxt->_spcomplex = new SparseComplexType(ctxt);
  ctxt->_cell = new CellType(ctxt);
  ctxt->_list = new ListType(ctxt);
  ctxt->_code = new CodeType(ctxt);
  ctxt->_module = new ModuleType(ctxt);
  ctxt->_function = new FunctionType(ctxt);
  ctxt->_functionHandle = new FunctionHandleType(ctxt);
  ctxt->_anonymous = new AnonymousType(ctxt);
  ctxt->_vm = new VM(ctxt);
  ctxt->_compiler = new Compiler(ctxt);
  ctxt->_assembler = new Assembler(ctxt);
  ctxt->_meta = new ClassMetaType(ctxt);
  ctxt->_class = new ClassType(ctxt);
  ctxt->_captured = new CapturedType(ctxt);
  ctxt->_bound = new BoundFunctionType(ctxt);
  ctxt->_breakpoint = new BreakpointType(ctxt);
  ctxt->_io = io;
  return ctxt;
}
