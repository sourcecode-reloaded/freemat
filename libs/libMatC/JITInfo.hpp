#ifndef __JITInfo_hpp__
#define __JITInfo_hpp__

#ifdef HAVE_LLVM
#include <QSharedDataPointer>
#include "JITFunc.hpp"

#ifndef HAVE_LLVM
class JITFunc {};
#endif

class JITInfo {
public:
  typedef enum {
    UNTRIED,
    FAILED,
    SUCCEEDED
  } JITState_t;
private:
  JITState_t m_jitstate;
  JITFunc* m_jitfunc;
public:
  JITInfo() : m_jitstate(UNTRIED), m_jitfunc(NULL) {}
  inline JITState_t JITState() const {return m_jitstate;}
  inline void setJITState(JITState_t t) {m_jitstate = t;}
  inline JITFunc* JITFunction() const {return m_jitfunc;}
  inline void setJITFunction(JITFunc *t) {m_jitfunc = t;}
};

#endif

#endif
