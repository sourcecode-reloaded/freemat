#ifndef __VariantPrivate_hpp__
#define __VariantPrivate_hpp__

template <>
inline double Variant::realScalar() const {
  if (m_type != DoubleScalar) throw Exception("type mismatch");
  return m_real.d;
}

template <>
inline double Variant::imagScalar() const {
  if (m_type != DoubleScalar) throw Exception("type mismatch");
  return m_imag.d;
}

template <>
inline float Variant::realScalar() const {
  if (m_type != FloatScalar) throw Exception("type mismatch");
  return m_real.f;
}

template <>
inline float Variant::imagScalar() const {
  if (m_type != FloatScalar) throw Exception("type mismatch");
  return m_imag.f;
}

template <>
inline uint64 Variant::realScalar() const {
  if (m_type != UInt64Scalar) throw Exception("type mismatch");
  return m_real.u64;
}

template <>
inline uint64 Variant::imagScalar() const {
  if (m_type != UInt64Scalar) throw Exception("type mismatch");
  return m_imag.u64;
}

template <>
inline int64 Variant::realScalar() const {
  if (m_type != Int64Scalar) throw Exception("type mismatch");
  return m_real.i64;
}

template <>
inline int64 Variant::imagScalar() const {
  if (m_type != Int64Scalar) throw Exception("type mismatch");
  return m_imag.i64;
}

template <>
inline uint32 Variant::realScalar() const {
  if (m_type != UInt32Scalar) throw Exception("type mismatch");
  return m_real.u32;
}

template <>
inline uint32 Variant::imagScalar() const {
  if (m_type != UInt32Scalar) throw Exception("type mismatch");
  return m_imag.u32;
}

template <>
inline int32 Variant::realScalar() const {
  if (m_type != Int32Scalar) throw Exception("type mismatch");
  return m_real.i32;
}

template <>
inline int32 Variant::imagScalar() const {
  if (m_type != Int32Scalar) throw Exception("type mismatch");
  return m_imag.i32;
}

template <>
inline uint16 Variant::realScalar() const {
  if (m_type != UInt16Scalar) throw Exception("type mismatch");
  return m_real.u16;
}

template <>
inline uint16 Variant::imagScalar() const {
  if (m_type != UInt16Scalar) throw Exception("type mismatch");
  return m_imag.u16;
}

template <>
inline int16 Variant::realScalar() const {
  if (m_type != Int16Scalar) throw Exception("type mismatch");
  return m_real.i16;
}

template <>
inline int16 Variant::imagScalar() const {
  if (m_type != Int16Scalar) throw Exception("type mismatch");
  return m_imag.i16;
}

template <>
inline uint8 Variant::realScalar() const {
  if (m_type != UInt8Scalar) throw Exception("type mismatch");
  return m_real.u8;
}

template <>
inline uint8 Variant::imagScalar() const {
  if (m_type != UInt8Scalar) throw Exception("type mismatch");
  return m_imag.u8;
}

template <>
inline int8 Variant::realScalar() const {
  if (m_type != Int8Scalar) throw Exception("type mismatch");
  return m_real.i8;
}

template <>
inline int8 Variant::imagScalar() const {
  if (m_type != Int8Scalar) throw Exception("type mismatch");
  return m_imag.i8;
}

template <>
inline logical Variant::realScalar() const {
  if (m_type != Int8Scalar) throw Exception("type mismatch");
  return m_real.l;
}

template <>
inline Variant::Variant(double real, double imag) {
  m_real.d = real; m_imag.d = imag; m_type = DoubleScalar;
}

template <>
inline Variant::Variant(float real, float imag) {
  m_real.f = real; m_imag.f = imag; m_type = FloatScalar;
}

template <>
inline Variant::Variant(int64 real, int64 imag) {
  m_real.i64 = real; m_imag.i64 = imag; m_type = Int64Scalar;
}

template <>
inline Variant::Variant(int32 real, int32 imag) {
  m_real.i32 = real; m_imag.i32 = imag; m_type = Int32Scalar;
}

template <>
inline Variant::Variant(int16 real, int16 imag) {
  m_real.i16 = real; m_imag.i16 = imag; m_type = Int16Scalar;
}

template <>
inline Variant::Variant(int8 real, int8 imag) {
  m_real.i8 = real; m_imag.i8 = imag; m_type = Int8Scalar;
}

template <>
inline Variant::Variant(uint64 real, uint64 imag) {
  m_real.u64 = real; m_imag.u64 = imag; m_type = UInt64Scalar;
}

template <>
inline Variant::Variant(uint32 real, uint32 imag) {
  m_real.u32 = real; m_imag.u32 = imag; m_type = UInt32Scalar;
}

template <>
inline Variant::Variant(uint16 real, uint16 imag) {
  m_real.u16 = real; m_imag.u16 = imag; m_type = UInt16Scalar;
}

template <>
inline Variant::Variant(uint8 real, uint8 imag) {
  m_real.u8 = real; m_imag.u8 = imag; m_type = UInt8Scalar;
}


#endif
