#ifndef __SparseType_hpp__
#define __SparseType_hpp__

#include "FMLib.hpp"
#include "Type.hpp"
#include "AggregateType.hpp"
#include "Complex.hpp"
#include "BoolType.hpp"
#include "DoubleType.hpp"
#include "SingleType.hpp"

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
    PODType<T> *_dense;
    void updateNNZ(SparseData<T> *dat) {
      dat->m_nnz = 0;
      for (auto col : dat->m_data)
	dat->m_nnz += col.second.size();
    }
  public:
    SparseType<T>(ThreadContext *ctxt, FMString name, PODType<T> *dense) :
    AggregateType<SparseData<T>,ValueSemantics>(ctxt), _classname(name), _dense(dense) {
      _name = name + " (sparse)";
    }
    virtual void visitContainedObjects(const ObjectBase *, ObjectVisitor &) const {}
    virtual Object import(const Object &foreign) {return foreign;}
    const FMString& name() const {return _name;}
    PODType<T> *dense() const {return _dense;}
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
    virtual Object Multiply(const Object &a, const Object &b);
    virtual Object Subtract(const Object &a, const Object &b);
    virtual Object DotLeftDivide(const Object &a, const Object &b);
    virtual Object DotRightDivide(const Object &a, const Object &b);
    virtual Object DotMultiply(const Object &a, const Object &b);
    virtual Object DotPower(const Object &a, const Object &b);
    virtual Object LessEquals(const Object &a, const Object &b);
    virtual Object LessThan(const Object &a, const Object &b);
    virtual Object GreaterThan(const Object &a, const Object &b);
    virtual Object GreaterEquals(const Object &a, const Object &b);
    virtual Object NotEquals(const Object &a, const Object &b);
    virtual Object Equals(const Object &a, const Object &b);
    virtual Object Or(const Object &a, const Object &b);
    virtual Object And(const Object &a, const Object &b);
    // virtual Object Neg(const Object &a);
    // virtual Object Not(const Object &a);
    T scalarValue(const Object &a) {
      if (!a.isScalar()) throw Exception("Illegal request for scalar contents of non-scalar sparse matrix");
      Object acopy(a);
      SparseData<T> *sd = this->rw(acopy);
      return sd->m_data[0][0];
    }
    Object scalarObject(const Object &a) {
      if (!a.isScalar()) throw Exception("Illegal request for scalar object from non-scalar sparse matrix");
      Object acopy(a);
      SparseData<T> *sd = this->rw(acopy);
      return _dense->makeScalar(sd->m_data[0][0]);
    }
    void extractDenseColumn(const Object &a, ndx_t col, Object &buffer) {
      T* ptr = _dense->rw(buffer);
      memset(ptr,0,sizeof(T)*buffer.count());
      const SparseData<T> *csd = this->ro(a);
      if (csd->m_data.count(col)) {
	for (auto row : csd->m_data.at(col))
	  ptr[row.first] = row.second;
      }
    }
    void sparseifyColumn(Object &a, ndx_t col, const Object &buffer) {
      const T*ptr = _dense->ro(buffer);
      SparseData<T> *csd = this->rw(a);
      for (ndx_t i=0;i<a.count();i++) {
	if (ptr[i] != T()) {
	  csd->m_data[col][i] = ptr[i];
	  csd->m_nnz++;
	}
      }
    }
  };

  class SparseLogicalType : public SparseType<bool> {
  public:
    SparseLogicalType(ThreadContext *ctxt) : SparseType<bool>(ctxt,"logical",ctxt->_bool) {}
    DataCode code() const {return TypeSparseLogical;}
  };

  class SparseDoubleType : public SparseType<double> {
  public:
    SparseDoubleType(ThreadContext *ctxt) : SparseType<double>(ctxt,"double",ctxt->_double) {}
    DataCode code() const {return TypeSparseDouble;}
  };

  class SparseComplexType : public SparseType<Complex<double> > {
  public:
    SparseComplexType(ThreadContext *ctxt) : SparseType<Complex<double> >(ctxt,"double complex",ctxt->_zdouble) {}
    DataCode code() const {return TypeSparseComplex;}
  };
  
};


#endif
