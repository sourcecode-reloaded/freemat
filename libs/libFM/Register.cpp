#include "Register.hpp"

using namespace FM;

Register::Register(int num, RegisterBlock *block) : _num(num), _block(block)
{
}

Register::~Register()
{
  _block->returnRegister(_num);
}

RegisterBlock::RegisterBlock(int poolsize)
{
  for (int i=0;i<poolsize;i++)
    _pool.insert(i);
}

reg_t RegisterBlock::getRegister()
{
  int a = *(_pool.begin());
  _pool.erase(_pool.begin());
  return boost::shared_ptr<Register>(new Register(a,this));
}

void RegisterBlock::returnRegister(int num)
{
  _pool.insert(num);
}
