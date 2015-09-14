#ifndef __Operators_hpp__
#define __Operators_hpp__

#include "Convert.hpp"
#include "PowFunc.hpp"
#include "Complex.hpp"

namespace FM
{
  struct OpAdd
  {
    template <class ctype, class atype, class btype, class vtype>
    inline static void func(ctype *c, const atype &a, const btype &b)
    {
      OpRangeConvert<ctype,vtype>::rangeConvert(*c,static_cast<vtype>(a) + static_cast<vtype>(b));
    }
  };
  
  struct OpSubtract
  {
    template <class ctype, class atype, class btype, class vtype>
    inline static void func(ctype *c, const atype &a, const btype &b)
    {
      OpRangeConvert<ctype,vtype>::rangeConvert(*c,static_cast<vtype>(a) - static_cast<vtype>(b));
    }
  };
  
  struct OpDotPower
  {
    template <class ctype, class atype, class btype, class vtype>
    inline static void func(ctype *c, const atype &a, const btype &b)
    {
      OpRangeConvert<ctype,vtype>::rangeConvert(*c,powfunc(static_cast<vtype>(a),static_cast<vtype>(b)));
    }
  };
  
  struct OpDotMultiply
  {
    template <class ctype, class atype, class btype, class vtype>
    inline static void func(ctype *c, const atype &a, const btype &b)
    {
      OpRangeConvert<ctype,vtype>::rangeConvert(*c,static_cast<vtype>(a) * static_cast<vtype>(b));
    }
  };
  
  struct OpDotRightDivide
  {
    template <class ctype, class atype, class btype, class vtype>
    inline static void func(ctype *c, const atype &a, const btype &b)
    {
      OpRangeConvert<ctype,vtype>::rangeConvert(*c,static_cast<vtype>(a) / static_cast<vtype>(b));
    }
  };
  
  struct OpDotLeftDivide
  {
    template <class ctype, class atype, class btype, class vtype>
    inline static void func(ctype *c, const atype &a, const btype &b)
    {
      OpRangeConvert<ctype,vtype>::rangeConvert(*c,static_cast<vtype>(b) / static_cast<vtype>(a));
    }
  };
  
  struct OpLE
  {
    template <class ctype, class atype, class btype, class vtype>
    inline static void func(ctype *c, const atype &a, const btype &b)
    {
      *c = static_cast<vtype>(a) <= static_cast<vtype>(b);
    }
  };
  
  struct OpLT
  {
    template <class ctype, class atype, class btype, class vtype>
    inline static void func(ctype *c, const atype &a, const btype &b)
    {
      *c = static_cast<vtype>(a) < static_cast<vtype>(b);
    }
  };
  
  struct OpGE
  {
    template <class ctype, class atype, class btype, class vtype>
    inline static void func(ctype *c, const atype &a, const btype &b)
    {
      *c = static_cast<vtype>(a) >= static_cast<vtype>(b);
    }
  };
  
  struct OpGT
  {
    template <class ctype, class atype, class btype, class vtype>
    inline static void func(ctype *c, const atype &a, const btype &b)
    {
      *c = static_cast<vtype>(a) > static_cast<vtype>(b);
    }
  };
  
  struct OpEQ
  {
    template <class ctype, class atype, class btype, class vtype>
    inline static void func(ctype *c, const atype &a, const btype &b)
    {
      *c = static_cast<vtype>(a) == static_cast<vtype>(b);
    }
  };
  
  struct OpNE
  {
    template <class ctype, class atype, class btype, class vtype>
    inline static void func(ctype *c, const atype &a, const btype &b)
    {
      *c = static_cast<vtype>(a) != static_cast<vtype>(b);
    }
  };
  
  struct OpOr
  {
    template <class ctype, class atype, class btype, class vtype>
    inline static void func(ctype *c, const atype &a, const btype &b)
    {
      *c = static_cast<vtype>(a) || static_cast<vtype>(b);
    }
  };
  
  struct OpAnd
  {
    template <class ctype, class atype, class btype, class vtype>
    inline static void func(ctype *c, const atype &a, const btype &b)
    {
      *c = static_cast<vtype>(a) && static_cast<vtype>(b);
    }
  };
  
  struct OpNeg
  {
    template <class T>
    inline static void func(T* c, const T* a)
    {
      *c = -*a;
    }
  };


  // Provide wrappers for operators that handle mixed complex/non-complex types
  template <class Op, class ctype, class atype, class btype, class vtype>
  class OpComplexWrapper
  {
  public:
    inline static void func(ctype *c, const atype &a, const btype &b)
    {
      Op::template func<ctype,atype,btype,vtype>(c,a,b);
    }
  };

  template <class Op, class atype, class btype, class vtype>
  class OpComplexWrapper<Op,bool,Complex<atype>,Complex<btype>,vtype>
  {
  public:
    inline static void func(bool *c, const Complex<atype> &a, const Complex<btype> &b)
    {
      Op::template func<bool,Complex<atype>,Complex<btype>,Complex<vtype> >(c,a,b);
    }
  };
  
  template <class Op, class atype, class btype, class vtype>
  class OpComplexWrapper<Op,bool,Complex<atype>,btype,vtype>
  {
  public:
    inline static void func(bool *c, const Complex<atype> &a, const btype &b)
    {
      Op::template func<bool,Complex<atype>,Complex<btype>,Complex<vtype> >(c,a,Complex<btype>(b,0));
    }
  };
  
  template <class Op, class ctype, class atype, class btype, class vtype>
  class OpComplexWrapper<Op,Complex<ctype>,Complex<atype>,Complex<btype>,vtype>
  {
  public:
    inline static void func(Complex<ctype> *c, const Complex<atype> &a, const Complex<btype> &b)
    {
      Op::template func<Complex<ctype>,Complex<atype>,Complex<btype>,Complex<vtype> >(c,a,b);
    }
  };
  
  template <class Op, class ctype, class atype, class btype, class vtype>
  class OpComplexWrapper<Op,Complex<ctype>,Complex<atype>,btype,vtype>
  {
  public:
    inline static void func(Complex<ctype> *c, const Complex<atype> &a, const btype &b)
    {
      Op::template func<Complex<ctype>,Complex<atype>,Complex<btype>,Complex<vtype> >(c,a,Complex<btype>(b,0));
    }
  };
  
  
  template <class Op, class ctype, class atype, class btype, class vtype>
  class OpComplexWrapper<Op,Complex<ctype>,atype,Complex<btype>,vtype>
  {
  public:
    inline static void func(Complex<ctype> *c, const atype &a, const Complex<btype> &b)
    {
      Op::template func<Complex<ctype>,Complex<atype>,Complex<btype>,Complex<vtype> >(c,Complex<atype>(a,0),b);
    }
  };

  struct OpSum
  {
    template <class Input, class Output>
    inline static void func(Output *op, const Input *ip, ndx_t reductionlen, ndx_t stride)
    {
      Output accum = Output();
      for (ndx_t vec=0;vec<reductionlen;vec++)
	accum += ip[vec*stride]; //page*stride*reductionlen + row + 
      *op = accum;
    }
};
  

}
#endif
