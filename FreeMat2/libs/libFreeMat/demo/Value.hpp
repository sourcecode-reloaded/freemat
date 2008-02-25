#ifndef __Value_hpp__
#define __Value_hpp__

class Value {
public:
  enum Type {
    Invalid = 0,
    Bool = 1,
    Int8 = 2,
    UInt8 = 3,
    Int16 = 4,
    UInt16 = 5,
    Int32 = 6,
    UInt32 = 7,
    Int64 = 8,
    UInt64 = 9,
    Float = 10,
    Double = 11
  };
  enum Class {
    Invalid = 0;
    Scalar = 1,
    Cell = 2,
    Struct = 3,
    Dense = 4,
    Sparse = 5,
    Slice = 6,
    Shape = 7,
    List = 8,
    Perm = 9,
    Object = 10
  };
  // Default constructor
  inline Value() : m_type(Invalid), m_class(Invalid) {}
  inline Value(double v) : m_type(Double), m_class(Scalar), m_r.d(v), m_i.d(0) 
  {}
    type = Double;
    d = v;
  }
  inline Value(Type t, const NTuple &dims);
  inline Value(const Value& ref) {
    type = ref.type;
    switch (type) {
    case Invalid:
      break;
    case Scalar:
      d = ref.d;
      break;
    case Cell:
      p = new CellArray(*(reinterpret_cast<CellArray*>(ref.p)));
      break;
    case Struct:
      p = new StructArray(*(reinterpret_cast<StructArray*>(ref.p)));
      break;
    case String:
      p = new StringArray(*(reinterpret_cast<StringArray*>(ref.p)));
      break;
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
    }
  }
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
  Type type;
};
  
};

#endif
