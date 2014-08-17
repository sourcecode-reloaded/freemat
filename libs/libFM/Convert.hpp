#ifndef __Convert_hpp__
#define __Convert_hpp__

namespace FM
{
  template <class T, class S>
  void convertLoop(const S* ip, T* op, dim_t len)
  {
    for (dim_t i=0;i<len;i++)
      op[i] = T(ip[i]);
  }
}

#endif
