#ifndef __ArrayPrivate_hpp__
#define __ArrayPrivate_hpp__

#define MacroExpandCasesNoBool(func)		\
  func(double,Double);				\
  func(float,Float);				\
  func(uint64,UInt64);				\
  func(int64,Int64);				\
  func(uint32,UInt32);				\
  func(int32,Int32);				\
  func(uint16,UInt16);				\
  func(int16,Int16);				\
  func(uint8,UInt8);				\
  func(int8,Int8);				

#define MacroExpandCases(func)			\
  MacroExpandCasesNoBool(func);			\
  func(bool,Bool);				

#define MacroExpandCasesSimple(func)		\
  MacroExpandCases(func);			\
  func(uint16,StringArray);

#define MacroExpandCasesAll(func)		\
  MacroExpandCasesSimple(func);			\
  func(Array,CellArray);


#define MacroGetDataClass(ctype,cls)		\
  template <>					\
  static inline DataClass GetDataClass(ctype) { \
    return cls;					\
  } 


#define MacroArrayRealScalar(ctype,cls)			\
  template <>						\
  inline ctype & Array::realScalar() {			\
    if (m_type.Class != cls)				\
      throw Exception("type mismatch");			\
    if (m_type.Scalar == 1)				\
      return m_real.cls;				\
    else if (m_type.Sparse == 1)			\
      return realSparse<ctype>()[1];			\
    else						\
      return real<ctype>()[1];				\
  }

#define MacroArrayImagScalar(ctype,cls)					\
  template <>								\
  inline ctype & Array::imagScalar() {					\
    if (cls == Bool)							\
      throw Exception("imaginary part not defined for logical types");	\
    if (m_type.Class != cls)						\
      throw Exception("type mismatch");					\
    m_type.Complex = 1;							\
    if (m_type.Scalar == 1)						\
      return m_imag.cls;						\
    else if (m_type.Sparse == 1)					\
      return imagSparse<ctype>()[1];					\
    else								\
      return imag<ctype>()[1];						\
  }

#define MacroArrayConstRealScalar(ctype,cls)	\
  template <>					\
  inline ctype Array::constRealScalar() const {	\
    if (m_type.Class != cls)			\
      throw Exception("type mismatch");		\
    if (m_type.Scalar == 1)			\
      return m_real.cls;			\
    else if (m_type.Sparse == 1)		\
      return constRealSparse<ctype>()[1];	\
    else					\
      return constReal<ctype>()[1];		\
  }

#define MacroArrayConstImagScalar(ctype,cls)				\
  template <>								\
  inline ctype Array::constImagScalar() const {				\
    if (cls == Bool)							\
      throw Exception("imaginary part not defined for logical types");	\
    if (m_type.Class != cls)						\
      throw Exception("type mismatch");					\
    if (m_type.Scalar == 1)						\
      return m_imag.cls;						\
    else if (m_type.Sparse == 1)					\
      return constImagSparse<ctype>()[1];				\
    else								\
      return constImag<ctype>()[1];					\
  }

#define MacroArrayComplexConstructor(ctype,cls)		     \
  template <>						     \
  inline Array::Array(ctype real, ctype imag) {		     \
    if (cls == Bool)					     \
      throw Exception("Complex logical values not allowed"); \
    m_real.cls = real; m_imag.cls = imag;		     \
    m_type.Class = cls;	 m_type.Complex = 1;		     \
    m_type.Sparse = 0; m_type.Scalar = 1;		     \
  };

#define MacroArrayRealConstructor(ctype,cls)		     \
  template <>						     \
  inline Array::Array(ctype real) {			     \
    m_real.cls = real; m_type.Class = cls;		     \
    m_type.Complex = 0; m_type.Sparse = 0;		     \
    m_type.Scalar = 1;					     \
  };

MacroExpandCases(MacroArrayRealScalar);
MacroExpandCases(MacroArrayImagScalar);
MacroExpandCases(MacroArrayConstRealScalar);
MacroExpandCases(MacroArrayConstImagScalar);
MacroExpandCases(MacroArrayComplexConstructor);
MacroExpandCases(MacroArrayRealConstructor);

MacroExpandCases(MacroGetDataClass);
MacroGetDataClass(Array,CellArray);

#undef MacroArrayRealScalar
#undef MacroArrayImagScalar
#undef MacroArrayConstRealScalar
#undef MacroArrayConstImagScalar
#undef MacroArrayComplexConstructor
#undef MacroArrayRealConstructor
#undef MacroGetDataClass

#endif
