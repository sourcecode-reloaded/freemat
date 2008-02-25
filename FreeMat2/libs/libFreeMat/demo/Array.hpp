#ifndef __Array_hpp__
#define __Array_hpp__

#include "Types.hpp"
#include "NDimArray.hpp"
#include "NumericArray.hpp"
class StructArray;
class CellArray;
class StringArray;

class Array {
public:
  enum Type {
    Invalid = 0,
    Cell = 1,
    Struct = 2,
    String = 3,
    BoolArray = 4,
    Int8Array = 5,
    UInt8Array = 6,
    Int16Array = 7,
    UInt16Array = 8,
    Int32Array = 9,
    UInt32Array = 10,
    Int64Array = 11,
    UInt64Array = 12,
    FloatArray = 13,
    DoubleArray = 14,
  };
  inline Array() : m_ptr(NULL), m_type(Invalid) {}
  inline Array(CellArray *p) : m_ptr(p), m_type(Cell) {}
  inline Array(StructArray *p) : m_ptr(p), m_type(Struct) {}
  inline Array(NumericArray<logical> *p) : m_ptr(p), m_type(BoolArray) {}
  inline Array(NumericArray<int8> *p) : m_ptr(p), m_type(Int8Array) {}
  inline Array(NumericArray<uint8> *p) : m_ptr(p), m_type(UInt8Array) {}
  inline Array(NumericArray<int16> *p) : m_ptr(p), m_type(Int16Array) {}
  inline Array(NumericArray<uint16> *p) : m_ptr(p), m_type(UInt16Array) {}
  inline Array(NumericArray<int32> *p) : m_ptr(p), m_type(Int32Array) {}
  inline Array(NumericArray<uint32> *p) : m_ptr(p), m_type(UInt32Array) {}
  inline Array(NumericArray<int64> *p) : m_ptr(p), m_type(Int64Array) {}
  inline Array(NumericArray<uint64> *p) : m_ptr(p), m_type(UInt64Array) {}
  inline Array(NumericArray<float> *p) : m_ptr(p), m_type(FloatArray) {}
  inline Array(NumericArray<double> *p) : m_ptr(p), m_type(DoubleArray) {}
  //
  inline const CellArray* asConstCellArray() const {
    if (m_type != Cell) throw Exception("Not a cell array");
    return reinterpret_cast<const CellArray*>(m_ptr);
  }
  inline CellArray* asCellArray() {
    if (m_type != Cell) throw Exception("Not a cell array");
    return reinterpret_cast<CellArray*>(m_ptr);
  }
  inline const StructArray* asConstStructArray() const {
    if (m_type != Struct) throw Exception("Not a struct array");
    return reinterpret_cast<const StructArray*>(m_ptr);
  }
  inline StructArray* asStructArray() {
    if (m_type != Struct) throw Exception("Not a struct array");
    return reinterpret_cast<StructArray*>(m_ptr);
  }
  inline const NumericArray<logical>* asConstLogicalArray() const {
    if (m_type != BoolArray) throw Exception("Not a bool array");
    return reinterpret_cast<const NumericArray<logical>*>(m_ptr);
  }
  inline NumericArray<logical>* asLogicalArray() {
    if (m_type != BoolArray) throw Exception("Not a bool array");
    return reinterpret_cast<NumericArray<logical>*>(m_ptr);
  }
  inline const NumericArray<double>* asConstDoubleArray() const {
    if (m_type != DoubleArray) throw Exception("Not a bool array");
    return reinterpret_cast<const NumericArray<double>*>(m_ptr);
  }
  inline NumericArray<double>* asDoubleArray() {
    if (m_type != DoubleArray) throw Exception("Not a bool array");
    return reinterpret_cast<NumericArray<double>*>(m_ptr);
  }
  Array(const Array &ref);
private:
  void *m_ptr;
  Type m_type;
};

#endif
