#ifndef __Exception_hpp__
#define __Exception_hpp__

#include "FMLib.hpp"

namespace FM
{
  class Exception {
    FMString m_msg;
  public:
    Exception() : m_msg("") {}
    Exception(const FMString &msg_in) : m_msg(msg_in) {}
    inline const FMString & msg() const {return m_msg;}
  };
}

#endif
