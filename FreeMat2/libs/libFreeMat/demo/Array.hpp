#ifndef __Array_hpp__
#define __Array_hpp__

#include "BaseArray.hpp"
#include <QSharedData>

template <typename T>
class NumericArray;

class ArrayBase : public QSharedData {
};

class Array;

class StructArray;
class StringArray;

class Array {
public:
  enum Type {
    Invalid = 0,
    Scalar = 1,
    Cell = 2,
    Struct = 3,
    String = 4,
    Bool = 5,
    Int8 = 6,
    UInt8 = 7,
    Int16 = 8,
    UInt16 = 9,
    Int32 = 10,
    UInt32 = 11,
    Int64 = 12,
    UInt64 = 13,
    Float = 14,
    Double = 15
  };
  inline Array() {d = 0; type = Invalid;}
  inline Array(double v) {
    type = Double;
    d = v;
  }
  inline Array(Type t, const NTuple &dims);
  //   inline const BaseArray<Array>* asConstCellArray() const {
  //     if (type != Cell) throw Exception("not a cell array");
  //     return static_cast<const BaseArray<Array>*>(p.constData());
  //   }
  //   inline BaseArray<Array>* asCellArray() {
  //     if (type != Cell) throw Exception("not a cell array");
  //     return static_cast<BaseArray<Array>*>(p.data());
  //   }
  inline const NumericArray<float>* asConstFloatArray() const {
    if (type != Float) throw Exception("not a float array");
    return reinterpret_cast<const NumericArray<float>*>(p.constData());
  }
  inline NumericArray<float>* asFloatArray() {
    if (type != Float) throw Exception("not a float array");
    return reinterpret_cast<NumericArray<float>*>(p.data());
  }
private:
  union {
    double d;
    void *p;
  }
  QSharedDataPointer<ArrayBase> p;
  Type type;
};

class StructArray : public ArrayBase {
  QVector<QSharedDataPointer<BaseArray<Array> > > data;
  QVector<QString> fieldnames;
public:
  StructArray(const NTuple &dims) { }
};

template <typename T>
class NumericArray : public ArrayBase {
  QSharedDataPointer<BaseArray<T> > p_real;
  QSharedDataPointer<BaseArray<T> > p_imag;
public:
  NumericArray(const NTuple &dims) {
    p_real = new NDimArray<T>(dims);
  }
  const BaseArray<T>* constReal() const {return p_real.constData();}
  BaseArray<T>* real() {return p_real.data();}
};

class StringArray : public ArrayBase {
  QSharedDataPointer<BaseArray<quint16> > data;
public:
  StringArray(const NTuple &dims) {
    data = new NDimArray<quint16>(dims);
  }
};

Array::Array(Type t,const NTuple &dims) {
  type = t;
  switch (t) {
  case Invalid:
    throw Exception("Illegal array construction");
  case Scalar:
    throw Exception("Illegal array construction");
  case Cell:
    //     p = static_cast<ArrayBase*>(new NDimArray<Array>(dims));
    break;
  case Struct:
    p = static_cast<ArrayBase*>(new StructArray(dims));
    break;
  case String:
    p = static_cast<ArrayBase*>(new StringArray(dims));
    break;
  case Bool:
    p = static_cast<ArrayBase*>(new NumericArray<bool>(dims));
    break;
  case UInt8:
    p = static_cast<ArrayBase*>(new NumericArray<quint8>(dims));
    break;
  case Int8:
    p = static_cast<ArrayBase*>(new NumericArray<qint8>(dims));
    break;
  case UInt16:
    p = static_cast<ArrayBase*>(new NumericArray<quint16>(dims));
    break;
  case Int16:
    p = static_cast<ArrayBase*>(new NumericArray<qint16>(dims));
    break;
  case UInt32:
    p = static_cast<ArrayBase*>(new NumericArray<quint32>(dims));
    break;
  case Int32:
    p = static_cast<ArrayBase*>(new NumericArray<qint32>(dims));
    break;
  case UInt64:
    p = static_cast<ArrayBase*>(new NumericArray<quint64>(dims));
    break;
  case Int64:
    p = static_cast<ArrayBase*>(new NumericArray<qint64>(dims));
    break;
  case Float:
    p = static_cast<ArrayBase*>(new NumericArray<float>(dims));
    break;
  case Double:
    p = static_cast<ArrayBase*>(new NumericArray<double>(dims));
    break;
  }
}


#endif
