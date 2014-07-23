#ifndef __Assembler_hpp__
#define __Assembler_hpp__

#include <vector>
#include "BaseTypes.hpp"
#include "Code.hpp"

namespace FM
{

class Assembler
{
  std::vector<BasicBlock*> _postorder;
  CodeBlock *_code;
  std::vector<uint64_t> _vm_codes;
  void depthFirstSearch(BasicBlock* p);
  void computeJumpOffsets();
public:
  Assembler(CodeBlock *code);
  void run();
  void assemble();
  Object codeObject(BaseTypes *b);
};

}
#endif
