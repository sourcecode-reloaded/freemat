#ifndef __OpAdd_hpp__
#define __OpAdd_hpp__

struct OpAdd
{
  template <class ctype, class atype, class btype, class vtype>
  inline static void func(ctype *c, const atype *a, const btype *b)
  {
    *c = static_cast<vtype>(*a) + static_cast<vtype>(*b);
  }
};


#endif
