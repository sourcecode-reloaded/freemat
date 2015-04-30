#ifndef __SparseType_hpp__
#define __SparseType_hpp__

#include "FMLib.hpp"
#include "Type.hpp"
#include "AggregateType.hpp"
#include "Complex.hpp"

namespace FM
{

  struct ThreadContext;
  
  template <class T>
  using SparseSlice = std::map<ndx_t, T>;

  template <class T>
  using SparseMatrixData = std::map<ndx_t, SparseSlice<T> >;
  
  template <class T>
  class SparseData {
  public:
    SparseMatrixData<T> m_data;
    int m_nnz;
    SparseData(ThreadContext *) : m_nnz(0) {}
  };

  
  template <class T>
  class SparseType : public AggregateType<SparseData<T>,ValueSemantics> {
    FMString _name;
    FMString _classname;
    void updateNNZ(SparseData<T> *dat) {
      dat->m_nnz = 0;
      for (auto col : dat->m_data)
	dat->m_nnz += col.second.size();
    }
  public:
    SparseType<T>(ThreadContext *ctxt, FMString name) :
    AggregateType<SparseData<T>,ValueSemantics>(ctxt), _classname(name) {
      _name = name + " (sparse)";
    }
    virtual void visitContainedObjects(const ObjectBase *p, ObjectVisitor &visitor) const {}
    virtual Object import(const Object &foreign) {return foreign;}
    const FMString& name() const {return _name;}
    FMString describe(const Object &a);
    FMString brief(const Object &a);
    Object asFullMatrix(const Object &a);
    Object asSparseMatrix(const Object &a);
    virtual Object makeScalar() {
      Object ret = AggregateType<SparseData<T>,ValueSemantics>::makeScalar();
      SparseData<T> *sd = this->rw(ret);
      sd->m_nnz = 0;
      return ret;
    }      
    virtual Object makeScalar(T val) {
      Object ret = AggregateType<SparseData<T>,ValueSemantics>::makeScalar();
      SparseData<T> *sd = this->rw(ret);
      sd->m_nnz = 1;
      sd->m_data[0][0] = val;
      return ret;
    }
    Object convert(const Object &a);
    virtual Object Add(const Object &a, const Object &b);
    T scalarValue(const Object &a) {
      if (!a.isScalar()) throw Exception("Illegal request for scalar contents of non-scalar sparse matrix");
      Object acopy(a);
      SparseData<T> *sd = this->rw(acopy);
      return sd->m_data[0][0];
    }
  };

  template <class T>
  class ConstSparseIterator {
    const SparseData<T> *m_ptr;
    Tuple m_dims;
    typename SparseMatrixData<T>::const_iterator m_col;
    typename SparseSlice<T>::const_iterator m_row;
  public:
    ConstSparseIterator(const Object &a, SparseType<T> *t) {
      m_ptr = t->ro(a);
      m_col = m_ptr->m_data.begin();
      if (m_col != m_ptr->m_data.end())
	m_row = m_col->second.begin();
      m_dims = a.dims();
    }
    inline ndx_t rows() const {
      return m_dims.rows();
    }
    inline bool valid() const {
      return (m_col != m_ptr->m_data.end());
    }
    inline explicit operator bool() const {
      return this->valid();
    }
    inline ConstSparseIterator& operator++() {
      if (m_col == m_ptr->m_data.end())
	throw Exception("Attempt to iterate past end of matrix");
      ++m_row;
      if (m_row == m_col->second.end()) {
	++m_col;
	if (m_col != m_ptr->m_data.end())
	  m_row = m_col->second.begin();
      }
      return *this;
    }
    const T&  operator*() {
      return m_row->second;
    }
    ndx_t row() const {
      if (!this->valid()) return 0;
      return m_row->first;
    }
    ndx_t col() const {
      if (!this->valid()) return m_dims.cols();
      return m_col->first;
    }
    const Tuple pos() const {
      return Tuple(row(),col());
    }
    ndx_t ndx() const {
      return row() + col()*m_dims.rows();
    }
    bool operator==(const ConstSparseIterator<T> &other) const {
      return ((this->valid() && other.valid()) &&
	      (this->row() == other.row()) &&
	      (this->col() == other.col()));
    }
    bool operator!=(const ConstSparseIterator<T> &other) const {
      return (this->ndx() == other.ndx());
    }
    bool operator<(const ConstSparseIterator<T> &other) const {
      return (this->ndx() < other.ndx());
    }
    bool operator>(const ConstSparseIterator<T> &other) const {
      return (this->ndx() > other.ndx());
    }
  };


  class SparseLogicalType : public SparseType<bool> {
  public:
    SparseLogicalType(ThreadContext *ctxt) : SparseType<bool>(ctxt,"logical") {}
    DataCode code() const {return TypeSparseLogical;}
  };

  class SparseDoubleType : public SparseType<double> {
  public:
    SparseDoubleType(ThreadContext *ctxt) : SparseType<double>(ctxt,"double") {}
    DataCode code() const {return TypeSparseDouble;}
  };

  class SparseComplexType : public SparseType<Complex<double> > {
  public:
    SparseComplexType(ThreadContext *ctxt) : SparseType<Complex<double> >(ctxt,"double complex") {}
    DataCode code() const {return TypeSparseComplex;}
  };
  
};


#endif
