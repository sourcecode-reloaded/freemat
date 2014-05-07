#ifndef __DenseArray_hpp__
#define __DenseArray_hpp__

// Do we _have_ to separate the real and imaginary parts?  Why can't we do something like:


// DataClass
//   -type
//   -

template <typename T>
struct FMComplex
{
  T m_real;
  T m_imag;
};

template <typename T>
class DenseArray
{
  T *m_data;
  NTuple m_dims;
  size_t m_length;
  size_t m_offset;
  size_t m_capacity;
public:
  DenseArray() : m_data(0), m_dims(), m_length(0), m_offset(0) {}
  DenseArray(const NTuple & dim)
  {
    m_dims = dim;
    m_length = dim.count();
    m_capacity = m_length;
    m_offset = 0;
    m_data = 0;
    if (m_length) m_data = new T[m_length];
  }
  explicit DenseArray(T val) 
  {
    m_dims = NTuple(1,1);
    m_length = 1;
    m_capacity = 1;
    m_offset = 0;
    m_data = new T[1];
    m_data[0] = val;
  }
  ~DenseArray()
  {
    if (m_data) delete [] m_data;
  }
  inline index_t offset() const {return m_offset;}
  inline const NTuple dimensions() const {return m_dims;}
  inline index_t rows() const {return m_dims.rows();}
  inline index_t columns() const {return m_dims.cols();}
  inline index_t cols() const {return m_dims.cols();}
  inline index_t length() const {return m_length;}
  inline index_t isScalar() const {return length() == 1;}
  inline bool is2D() const {return m_dims.is2D();}
  inline bool isEmpty() const {return (length() == 0);}
  inline bool isColumnVector() const {return m_dims.isColumnVector();}
  inline bool isRowVector() const {return m_dims.isRowVector();}
  inline bool isVector() const {return isColumnVector() || isRowVector();}
  inline const T & operator[](const NTuple & pos) const {return get(pos);}
  inline const T & operator[](index_t pos) const {return get(pos);}
  inline T & operator[](const NTuple & pos) {return get(pos);}
  inline T & operator[](index_t pos) {return get(pos);}
  inline const T & get(const NTuple& pos) const {
    if (m_dims.validate(pos))
      return m_data[(size_t)(m_dims.map(pos)+m_offset-1)];
    throw Exception("out of range");
  }
  inline const T & get(index_t pos) const {
    if ((pos > 0) && (pos <= length()))
      return m_data[(size_t)(pos+m_offset-1)];
    throw Exception("out of range");
  }
  inline const T & getNoBoundsCheck(index_t pos) const {
    return m_data[(size_t)(pos+m_offset-1)];
  }
  inline T & get(const NTuple& pos) {
    if (m_dims.validate(pos))
      return m_data[(size_t)(m_dims.map(pos)+m_offset-1)];
    throw Exception("out of range");
  }
  inline T & get(index_t pos) {
    if ((pos > 0) && (pos <= length()))
      return m_data[(size_t)(pos+m_offset-1)];
    throw Exception("out of range");
  }
  inline const T* constData() const {
    if (length() > 0)
      return m_real + size_t(m_offset);
    return NULL;
  }
  inline T* data() {
    if (length() > 0)
      return m_real + size_t(m_offset);
    return NULL;
  }
  void fill(T val) {
    for (index_t i=1;i<=length();i++)
      m_data[(size_t)(i+m_offset-1)] = val;
  }
  bool operator==(const BasicArray<T>& b) const {
    for (index_t i=1;i<=b.length();i++) 
      if (get(i) != b.get(i)) return false;
    return true;
  }
  inline void set(const NTuple& pos, const T& val) {
    if (!(pos <= dimensions())) resize(max(dimensions(),pos));
    m_data[(size_t)(m_dims.map(pos)+m_offset-1)] = val;
  }
  inline void set(index_t pos, const T& val) {
    if (m_count < pos) resize(pos);
    m_data[(size_t)(pos+m_offset-1)] = val;
  }
  inline void setNoBoundsCheck(index_t pos, const T& val) {
    m_data.data()[(size_t)(pos+m_offset-1)] = val;
  }

  
};


#endif
