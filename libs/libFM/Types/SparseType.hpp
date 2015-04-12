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
  using SparseSlice = FMMap<ndx_t, T>;

  template <class T>
  using SparseMatrixData = FMMap<ndx_t, SparseSlice<T> >;
  
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
