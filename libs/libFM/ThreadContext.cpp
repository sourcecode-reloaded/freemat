#include "ThreadContext.hpp"
#include "AllTypes.hpp"
#include "VM.hpp"
#include "Compiler.hpp"
#include "Assembler.hpp"

using namespace FM;


ThreadContext* FM::BuildNewThreadContext(TermIF *io)
{
  ThreadContext *ctxt = new ThreadContext;
  ctxt->_bool = new BoolType(ctxt);
  ctxt->_double = new DoubleType(ctxt);
  ctxt->_empty = ctxt->_double->emptyBase();
  ctxt->_single = new SingleType(ctxt);
  ctxt->_string = new StringType(ctxt);
  ctxt->_index = new IndexType(ctxt);
  ctxt->_int32 = new Int32Type(ctxt);
  ctxt->_int64 = new Int64Type(ctxt);
  ctxt->_uint32 = new UInt32Type(ctxt);
  ctxt->_uint64 = new UInt64Type(ctxt);
  ctxt->_struct = new StructType(ctxt);
  ctxt->_cell = new CellType(ctxt);
  ctxt->_list = new ListType(ctxt);
  ctxt->_code = new CodeType(ctxt);
  ctxt->_vm = new VM(ctxt);
  ctxt->_compiler = new Compiler(ctxt);
  ctxt->_asm = new Assembler(ctxt);
  ctxt->_io = io;
  return ctxt;
}
