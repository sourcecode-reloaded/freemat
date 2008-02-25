#include "Cell.hpp"
#include "StructArray.hpp"

Array::Array(const Array &ref) {
  m_type = ref.m_type;
  switch (m_type) {
  case Invalid:
    return;
  case Cell:
    m_ptr = new CellArray(*(reinterpret_cast<CellArray*>(ref.m_ptr)));
    return;
  case Struct:
    m_ptr = new StructArray(*(reinterpret_cast<StructArray*>(ref.m_ptr)));
    return;
  case String:
    return;
  case BoolArray:
    m_ptr = new NumericArray<logical>(*(reinterpret_cast<NumericArray<logical>*>(ref.m_ptr)));
    return;
  case Int8Array:
    m_ptr = new NumericArray<int8>(*(reinterpret_cast<NumericArray<int8>*>(ref.m_ptr)));
    return;
  case UInt8Array:
    m_ptr = new NumericArray<uint8>(*(reinterpret_cast<NumericArray<uint8>*>(ref.m_ptr)));
    return;
  case Int16Array:
    m_ptr = new NumericArray<int16>(*(reinterpret_cast<NumericArray<int16>*>(ref.m_ptr)));
    return;
  case UInt16Array:
    m_ptr = new NumericArray<uint16>(*(reinterpret_cast<NumericArray<uint16>*>(ref.m_ptr)));
    return;
  case Int32Array:
    m_ptr = new NumericArray<int32>(*(reinterpret_cast<NumericArray<int32>*>(ref.m_ptr)));
    return;
  case UInt32Array:
    m_ptr = new NumericArray<uint32>(*(reinterpret_cast<NumericArray<uint32>*>(ref.m_ptr)));
    return;
  case Int64Array:
    m_ptr = new NumericArray<int64>(*(reinterpret_cast<NumericArray<int64>*>(ref.m_ptr)));
    return;
  case UInt64Array:
    m_ptr = new NumericArray<uint64>(*(reinterpret_cast<NumericArray<uint64>*>(ref.m_ptr)));
    return;
  case FloatArray:
    m_ptr = new NumericArray<float>(*(reinterpret_cast<NumericArray<float>*>(ref.m_ptr)));
    return;
  case DoubleArray:
    m_ptr = new NumericArray<double>(*(reinterpret_cast<NumericArray<double>*>(ref.m_ptr)));
    return;
  }
}
