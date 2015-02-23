#ifndef __Assembler_hpp__
#define __Assembler_hpp__

#include <vector>
#include "Code.hpp"

namespace FM
{

  struct ThreadContext;

  class Assembler
  {
    ThreadContext *_ctxt;

    std::vector<BasicBlock*> _postorder;
    CodeBlock *_code;
    std::vector<uint64_t> _vm_codes;
    std::vector<uint16_t> _line_nos;
    std::vector<Object> _nested_codes;
    void depthFirstSearch(BasicBlock* p);
    void computeJumpOffsets();
    Object codeObject(const FMString &name);
    void assemble();
    Object run(CodeBlock* code, const FMString &name);
  public:
    Assembler(ThreadContext *ctxt) : _ctxt(ctxt) {};
    Object run(Module* mod);
  };
}
#endif
