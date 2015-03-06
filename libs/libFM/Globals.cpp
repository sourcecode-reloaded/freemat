#include "Globals.hpp"
#include "ThreadContext.hpp"
#include "Object.hpp"

using namespace FM;

Globals::Globals(ThreadContext *ctxt) : _myctxt(ctxt) {}

void Globals::set(const FMString &name, const Object &value) {
  std::lock_guard<std::mutex> lock(this->_lock);
  auto p = _dict.find(name);
  if (p == _dict.end())
    _dict.insert(std::make_pair(name,value));
  else
    p->second = value;
}

bool Globals::isDefined(const FMString &name) {
  std::lock_guard<std::mutex> lock(this->_lock);
  return _dict.count(name) > 0;
}

Object Globals::get(const FMString &name, ThreadContext *ctxt) {
  std::lock_guard<std::mutex> lock(this->_lock);
  auto p = _dict.find(name);
  if (p == _dict.end()) throw Exception("Attempt to get undefined variable from Global");
  return p->second.exportTo(ctxt);
}
