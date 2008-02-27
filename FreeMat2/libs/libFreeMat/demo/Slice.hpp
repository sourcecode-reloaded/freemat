#ifndef __Slice_hpp__
#define __Slice_hpp__

#include "NumericArray.hpp"

class Slice {
  Array &m_ref;
  const RangeSet &m_slices;
  NTuple m_dims;
public:
  Slice(Array &ref, const RangeSet& slice) :
    m_ref(ref), m_slices(slice), m_dims(slices.dims()) { }
  void set(const Array &data) {
    if (m_ref.type() != data.type())
      throw Exception("type mismatch");
    switch (m_ref.type()) {
    default:
    case Cell:
    case Struct:
    case String:
    case BoolArray:
    case Int8Array:
    case UInt8Array:
    case Int16Array:
    case UInt16Array:
    case Int32Array:
    case UInt32Array:
    case Int64Array:
    case UInt64Array:
    case FloatArray:
      throw Exception("Unhandled");
    case DoubleArray:
      set(m_ref.asDoubleArray(),data.asConstDoubleArray());
    }
  }
  template <typename T>
  void setSingularData(NumericArray<T>*dst, 
		       const NumericArray<T>*src){
    
  }
  template <typename T>
  void set(NumericArray<T>*dst, const NumericArray<T>*src) {
    if (src->isScalar()) {
      setSingularData(dst,src);
      return;
    }
    index_t t_count = m_dims.count();
    NTuple pos(1,1);
    for (int i=0;i<t_count;i++) {
      NTuple tpos;
      m_slices.remap(pos,tpos);
      m_ref.real()[tpos] = 
    }
  }
};

#endif
