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
    std::vector<Object> _nested_codes;
    void depthFirstSearch(BasicBlock* p);
    void computeJumpOffsets();
    Object codeObject();
    void assemble();
  public:
    Assembler(ThreadContext *ctxt) : _ctxt(ctxt) {};
    Object run(CodeBlock* code);
    Object run(Module* mod);
  };
}
#endif
