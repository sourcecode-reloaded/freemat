#ifndef __Register_hpp__
#define __Register_hpp__

#include <stdint.h>
#include <set>
#include <boost/shared_ptr.hpp>

namespace FM 
{

  class RegisterBlock;
  class Compiler;

  class Register
  {
    RegisterBlock *_block;
    int _num;
  public:
    Register(int num, RegisterBlock *block);
    ~Register();
    int num() const {return _num;}
  };

  typedef boost::shared_ptr<Register> reg_t;

  class RegisterBlock
  {
    std::set<int> _pool;
    Compiler *_compiler;
  public:
    RegisterBlock(int poolsize, Compiler *compiler);
    reg_t getRegister();
    void returnRegister(int num);
  };

}

#endif
