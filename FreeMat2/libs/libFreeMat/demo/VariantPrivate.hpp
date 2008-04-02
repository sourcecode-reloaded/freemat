#ifndef __VariantPrivate_hpp__
#define __VariantPrivate_hpp__

template <>
inline double& Variant::realScalar()  {
  if (m_type != DoubleScalar) {
    if (m_type == DoubleArray)
      return real<double>()[1];
    throw Exception("type mismatch");
  }
  return m_real.d;
}

template <>
inline float& Variant::realScalar()  {
  if (m_type != FloatScalar) {
    if (m_type == FloatArray)
      return real<float>()[1];
    throw Exception("type mismatch");
  }
  return m_real.f;
}

template <>
inline uint64& Variant::realScalar()  {
  if (m_type != UInt64Scalar) {
    if (m_type == UInt64Array)
      return real<uint64>()[1];
    throw Exception("type mismatch");
  }
  return m_real.u64;
}

template <>
inline int64& Variant::realScalar()  {
  if (m_type != Int64Scalar) {
    if (m_type == Int64Array)
      return real<int64>()[1];
    throw Exception("type mismatch");
  }
  return m_real.i64;
}

template <>
inline uint32& Variant::realScalar()  {
  if (m_type != UInt32Scalar) {
    if (m_type == UInt32Array)
      return real<uint32>()[1];
    throw Exception("type mismatch");
  }
  return m_real.u32;
}

template <>
inline int32& Variant::realScalar()  {
  if (m_type != Int32Scalar) {
    if (m_type == Int32Array)
      return real<int32>()[1];
    throw Exception("type mismatch");
  }
  return m_real.i32;
}

template <>
inline uint16& Variant::realScalar()  {
  if (m_type != UInt16Scalar) {
    if (m_type == UInt16Array)
      return real<uint16>()[1];
    throw Exception("type mismatch");
  }
  return m_real.u16;
}

template <>
inline int16& Variant::realScalar()  {
  if (m_type != Int16Scalar) {
    if (m_type == Int16Array)
      return real<int16>()[1];
    throw Exception("type mismatch");
  }
  return m_real.i16;
}

template <>
inline uint8& Variant::realScalar()  {
  if (m_type != UInt8Scalar) {
    if (m_type == UInt8Array)
      return real<uint8>()[1];
    throw Exception("type mismatch");
  }
  return m_real.u8;
}

template <>
inline int8& Variant::realScalar()  {
  if (m_type != Int8Scalar) {
    if (m_type == Int8Array)
      return real<int8>()[1];
    throw Exception("type mismatch");
  }
  return m_real.i8;
}

template <>
inline logical& Variant::realScalar()  {
  if (m_type != BoolScalar) {
    if (m_type == BoolArray)
      return real<logical>()[1];
    throw Exception("type mismatch");
  }
  return m_real.l;
}

template <>
inline double& Variant::imagScalar()  {
  m_complex = true;
  if (m_type != DoubleScalar) {
    if (m_type == DoubleArray)
      return imag<double>()[1];
    throw Exception("type mismatch");
  }
  return m_imag.d;
}

template <>
inline float& Variant::imagScalar()  {
  m_complex = true;
  if (m_type != FloatScalar) {
    if (m_type == FloatArray)
      return imag<float>()[1];
    throw Exception("type mismatch");
  }
  return m_imag.f;
}

template <>
inline uint64& Variant::imagScalar()  {
  m_complex = true;
  if (m_type != UInt64Scalar) {
    if (m_type == UInt64Array)
      return imag<uint64>()[1];
    throw Exception("type mismatch");
  }
  return m_imag.u64;
}

template <>
inline int64& Variant::imagScalar()  {
  m_complex = true;
  if (m_type != Int64Scalar) {
    if (m_type == Int64Array)
      return imag<int64>()[1];
    throw Exception("type mismatch");
  }
  return m_imag.i64;
}

template <>
inline uint32& Variant::imagScalar()  {
  m_complex = true;
  if (m_type != UInt32Scalar) {
    if (m_type == UInt32Array)
      return imag<uint32>()[1];
    throw Exception("type mismatch");
  }
  return m_imag.u32;
}

template <>
inline int32& Variant::imagScalar()  {
  m_complex = true;
  if (m_type != Int32Scalar) {
    if (m_type == Int32Array)
      return imag<int32>()[1];
    throw Exception("type mismatch");
  }
  return m_imag.i32;
}

template <>
inline uint16& Variant::imagScalar()  {
  m_complex = true;
  if (m_type != UInt16Scalar) {
    if (m_type == UInt16Array)
      return imag<uint16>()[1];
    throw Exception("type mismatch");
  }
  return m_imag.u16;
}

template <>
inline int16& Variant::imagScalar()  {
  m_complex = true;
  if (m_type != Int16Scalar) {
    if (m_type == Int16Array)
      return imag<int16>()[1];
    throw Exception("type mismatch");
  }
  return m_imag.i16;
}

template <>
inline uint8& Variant::imagScalar()  {
  m_complex = true;
  if (m_type != UInt8Scalar) {
    if (m_type == UInt8Array)
      return imag<uint8>()[1];
    throw Exception("type mismatch");
  }
  return m_imag.u8;
}

template <>
inline int8& Variant::imagScalar()  {
  m_complex = true;
  if (m_type != Int8Scalar) {
    if (m_type == Int8Array)
      return imag<int8>()[1];
    throw Exception("type mismatch");
  }
  return m_imag.i8;
}

template <>
inline logical& Variant::imagScalar()  {
  throw Exception("imaginary part of logical variables is undefined");
}

template <>
inline double Variant::constRealScalar() const {
  if (m_type != DoubleScalar) {
    if (m_type == DoubleArray)
      return constReal<double>()[1];
    throw Exception("type mismatch");
  }
  return m_real.d;
}

template <>
inline float Variant::constRealScalar() const {
  if (m_type != FloatScalar) {
    if (m_type == FloatArray)
      return constReal<float>()[1];
    throw Exception("type mismatch");
  }
  return m_real.f;
}

template <>
inline uint64 Variant::constRealScalar() const {
  if (m_type != UInt64Scalar) {
    if (m_type == UInt64Array)
      return constReal<uint64>()[1];
    throw Exception("type mismatch");
  }
  return m_real.u64;
}

template <>
inline int64 Variant::constRealScalar() const {
  if (m_type != Int64Scalar) {
    if (m_type == Int64Array)
      return constReal<int64>()[1];
    throw Exception("type mismatch");
  }
  return m_real.i64;
}

template <>
inline uint32 Variant::constRealScalar() const {
  if (m_type != UInt32Scalar) {
    if (m_type == UInt32Array)
      return constReal<uint32>()[1];
    throw Exception("type mismatch");
  }
  return m_real.u32;
}

template <>
inline int32 Variant::constRealScalar() const {
  if (m_type != Int32Scalar) {
    if (m_type == Int32Array)
      return constReal<int32>()[1];
    throw Exception("type mismatch");
  }
  return m_real.i32;
}

template <>
inline uint16 Variant::constRealScalar() const {
  if (m_type != UInt16Scalar) {
    if (m_type == UInt16Array)
      return constReal<uint16>()[1];
    throw Exception("type mismatch");
  }
  return m_real.u16;
}

template <>
inline int16 Variant::constRealScalar() const {
  if (m_type != Int16Scalar) {
    if (m_type == Int16Array)
      return constReal<int16>()[1];
    throw Exception("type mismatch");
  }
  return m_real.i16;
}

template <>
inline uint8 Variant::constRealScalar() const {
  if (m_type != UInt8Scalar) {
    if (m_type == UInt8Array)
      return constReal<uint8>()[1];
    throw Exception("type mismatch");
  }
  return m_real.u8;
}

template <>
inline int8 Variant::constRealScalar() const {
  if (m_type != Int8Scalar) {
    if (m_type == Int8Array)
      return constReal<int8>()[1];
    throw Exception("type mismatch");
  }
  return m_real.i8;
}

template <>
inline logical Variant::constRealScalar() const {
  if (m_type != BoolScalar) {
    if (m_type == BoolArray)
      return constReal<logical>()[1];
    throw Exception("type mismatch");
  }
  return m_real.l;
}

template <>
inline double Variant::constImagScalar() const {
  if (!m_complex) return 0;
  if (m_type != DoubleScalar) {
    if (m_type == DoubleArray)
      return constImag<double>()[1];
    throw Exception("type mismatch");
  }
  return m_imag.d;
}

template <>
inline float Variant::constImagScalar() const {
  if (!m_complex) return 0;
  if (m_type != FloatScalar) {
    if (m_type == FloatArray)
      return constImag<float>()[1];
    throw Exception("type mismatch");
  }
  return m_imag.f;
}

template <>
inline uint64 Variant::constImagScalar() const {
  if (!m_complex) return 0;
  if (m_type != UInt64Scalar) {
    if (m_type == UInt64Array)
      return constImag<uint64>()[1];
    throw Exception("type mismatch");
  }
  return m_imag.u64;
}

template <>
inline int64 Variant::constImagScalar() const {
  if (!m_complex) return 0;
  if (m_type != Int64Scalar) {
    if (m_type == Int64Array)
      return constImag<int64>()[1];
    throw Exception("type mismatch");
  }
  return m_imag.i64;
}

template <>
inline uint32 Variant::constImagScalar() const {
  if (!m_complex) return 0;
  if (m_type != UInt32Scalar) {
    if (m_type == UInt32Array)
      return constImag<uint32>()[1];
    throw Exception("type mismatch");
  }
  return m_imag.u32;
}

template <>
inline int32 Variant::constImagScalar() const {
  if (!m_complex) return 0;
  if (m_type != Int32Scalar) {
    if (m_type == Int32Array)
      return constImag<int32>()[1];
    throw Exception("type mismatch");
  }
  return m_imag.i32;
}

template <>
inline uint16 Variant::constImagScalar() const {
  if (!m_complex) return 0;
  if (m_type != UInt16Scalar) {
    if (m_type == UInt16Array)
      return constImag<uint16>()[1];
    throw Exception("type mismatch");
  }
  return m_imag.u16;
}

template <>
inline int16 Variant::constImagScalar() const {
  if (!m_complex) return 0;
  if (m_type != Int16Scalar) {
    if (m_type == Int16Array)
      return constImag<int16>()[1];
    throw Exception("type mismatch");
  }
  return m_imag.i16;
}

template <>
inline uint8 Variant::constImagScalar() const {
  if (!m_complex) return 0;
  if (m_type != UInt8Scalar) {
    if (m_type == UInt8Array)
      return constImag<uint8>()[1];
    throw Exception("type mismatch");
  }
  return m_imag.u8;
}

template <>
inline int8 Variant::constImagScalar() const {
  if (!m_complex) return 0;
  if (m_type != Int8Scalar) {
    if (m_type == Int8Array)
      return constImag<int8>()[1];
    throw Exception("type mismatch");
  }
  return m_imag.i8;
}

template <>
inline logical Variant::constImagScalar() const {
  throw Exception("imaginary part of logical variables is undefined");
}

template <>
inline Variant::Variant(double real, double imag) {
  m_real.d = real; m_imag.d = imag; 
  m_type = DoubleScalar; m_complex = true;
}

template <>
inline Variant::Variant(float real, float imag) {
  m_real.f = real; m_imag.f = imag; 
  m_type = FloatScalar; m_complex = true;
}

template <>
inline Variant::Variant(int64 real, int64 imag) {
  m_real.i64 = real; m_imag.i64 = imag; 
  m_type = Int64Scalar; m_complex = true;
}

template <>
inline Variant::Variant(int32 real, int32 imag) {
  m_real.i32 = real; m_imag.i32 = imag; 
  m_type = Int32Scalar; m_complex = true;
}

template <>
inline Variant::Variant(int16 real, int16 imag) {
  m_real.i16 = real; m_imag.i16 = imag; 
  m_type = Int16Scalar; m_complex = true;
}

template <>
inline Variant::Variant(int8 real, int8 imag) {
  m_real.i8 = real; m_imag.i8 = imag; 
  m_type = Int8Scalar; m_complex = true;
}

template <>
inline Variant::Variant(uint64 real, uint64 imag) {
  m_real.u64 = real; m_imag.u64 = imag; 
  m_type = UInt64Scalar; m_complex = true;
}

template <>
inline Variant::Variant(uint32 real, uint32 imag) {
  m_real.u32 = real; m_imag.u32 = imag; 
  m_type = UInt32Scalar; m_complex = true;
}

template <>
inline Variant::Variant(uint16 real, uint16 imag) {
  m_real.u16 = real; m_imag.u16 = imag; 
  m_type = UInt16Scalar; m_complex = true;
}

template <>
inline Variant::Variant(uint8 real, uint8 imag) {
  m_real.u8 = real; m_imag.u8 = imag; 
  m_type = UInt8Scalar; m_complex = true;
}

template <>
inline Variant::Variant(logical, logical) {
  throw Exception("Complex logical values not allowed");
}

template <>
inline Variant::Variant(double real) {
  m_real.d = real; m_type = DoubleScalar; m_complex = false;
}

template <>
inline Variant::Variant(float real) {
  m_real.f = real; m_type = FloatScalar; m_complex = false;
}

template <>
inline Variant::Variant(int64 real) {
  m_real.i64 = real; m_type = Int64Scalar; m_complex = false;
}

template <>
inline Variant::Variant(int32 real) {
  m_real.i32 = real; m_type = Int32Scalar; m_complex = false;
}

template <>
inline Variant::Variant(int16 real) {
  m_real.i16 = real; m_type = Int16Scalar; m_complex = false;
}

template <>
inline Variant::Variant(int8 real) {
  m_real.i8 = real; m_type = Int8Scalar; m_complex = false;
}

template <>
inline Variant::Variant(uint64 real) {
  m_real.u64 = real; m_type = UInt64Scalar; m_complex = false;
}

template <>
inline Variant::Variant(uint32 real) {
  m_real.u32 = real; m_type = UInt32Scalar; m_complex = false;
}

template <>
inline Variant::Variant(uint16 real) {
  m_real.u16 = real; m_type = UInt16Scalar; m_complex = false;
}

template <>
inline Variant::Variant(uint8 real) {
  m_real.u8 = real; m_type = UInt8Scalar; m_complex = false;
}

template <>
inline Variant::Variant(logical real) {
  m_real.l = real; m_type = BoolScalar; m_complex = false;
}


#endif
