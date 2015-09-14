#include "SparseType.hpp"
#include "DoubleType.hpp"

namespace FM {

  
  static const int SPARSE_OP_NORMAL = 0;
  static const int SPARSE_OP_IS_BOOL_VALUED = 1;
  static const int SPARSE_OP_PRESERVES_SPARSITY = 2;

  template <class T>
  FMString SparseType<T>::describe(const Object &a) {
    const SparseData<T> *csd = this->ro(a);
    return "Sparse matrix of size "+Stringify(a.rows()) + " x " + Stringify(a.cols()) + " class " + this->_classname + " with "
      + Stringify(csd->m_nnz) + " nonzero entries";
  }

  template <class T>
  FMString SparseType<T>::brief(const Object &a) {
    return "Sparse " + Stringify(a.rows()) + "x" + Stringify(a.cols());  
  }

  template <class T>
  static void convertFullMatrixToSparse(Object &dest, const T* src, const Tuple &dims, SparseType<T> *t) {
    dest.setDims(dims);
    ndx_t M = dims.rows();
    SparseData<T> *csd = t->rw(dest);
    for (ndx_t col = 0;col<dims.cols();col++)
      for (ndx_t row = 0;row<M;row++) 
	if (src[col*M+row] != T()) {
	  csd->m_data[col][row] = src[col*M+row];
	  csd->m_nnz++;
	}
  }

  template <class T, class S>
  static Object convertSparseMatrixToSparseMatrix(const Object &a, SparseType<T> *t, SparseType<S> *s) {
    Object ret(s->empty());
    ret.setDims(a.dims());
    const SparseData<T> *asd = t->ro(a);
    SparseData<S> *rsd = s->rw(ret);
    for (auto col : asd->m_data)
      for (auto row : col.second)
	OpRangeConvert<S,T>::rangeConvert(rsd->m_data[col.first][row.first],row.second);
    return ret;
  }

  template <>
  Object SparseType<double>::convert(const Object &a) {
    if (a.isSparse()) {
      switch (a.typeCode())
	{
	case TypeSparseLogical:
	  return convertSparseMatrixToSparseMatrix(a,_ctxt->_splogical,this);
	case TypeSparseDouble:
	  return a;
	case TypeSparseComplex:
	  this->context()->_io->warning("Complex part of matrix will be ignored");
	  return convertSparseMatrixToSparseMatrix(a,_ctxt->_spcomplex,this);
	default:
	  throw Exception("Unhandled sparse matrix conversion case");
	}
    }
    Object aconv = _dense->convert(a);
    Object ret(this->empty());
    convertFullMatrixToSparse(ret,_dense->ro(aconv),a.dims(),this);
    return ret;
  }

  template <>
  Object SparseType<bool>::convert(const Object &a) {
    if (a.isSparse()) {
      switch (a.typeCode())
	{
	case TypeSparseLogical:
	  return a;
	case TypeSparseDouble:
	  return convertSparseMatrixToSparseMatrix(a,_ctxt->_spdouble,this);
	case TypeSparseComplex:
	  return convertSparseMatrixToSparseMatrix(a,_ctxt->_spcomplex,this);
	default:
	  throw Exception("Unhandled sparse matrix conversion case");
	}
    }
    Object aconv = _dense->convert(a);
    Object ret(this->empty());
    convertFullMatrixToSparse(ret,_dense->ro(aconv),a.dims(),this);
    return ret;
  }

  template <>
  Object SparseType<Complex<double> >::convert(const Object &a) {
    if (a.isSparse()) {
      switch (a.typeCode())
	{
	case TypeSparseLogical:
	  return convertSparseMatrixToSparseMatrix(a,_ctxt->_splogical,this);
	case TypeSparseDouble:
	  return convertSparseMatrixToSparseMatrix(a,_ctxt->_spdouble,this);
	case TypeSparseComplex:
	  return a;
	default:
	  throw Exception("Unhandled sparse matrix conversion case");
	}
    }
    Object aconv = _dense->convert(a);
    Object ret(this->empty());
    convertFullMatrixToSparse(ret,_dense->ro(aconv),a.dims(),this);
    return ret;
  }

  template <class T>
  static void convertSparseMatrixToFull(T* destp, const Object &src, SparseType<T> *t) {
    const SparseData<T> *ad = t->ro(src);
    for (auto column : ad->m_data)
      for (auto row : column.second)
	destp[row.first + column.first*src.rows()] = row.second;
  }

  template <class T>
  Object SparseType<T>::asFullMatrix(const Object &a) {
    Object af = _dense->makeMatrix(a.rows(),a.cols());
    convertSparseMatrixToFull(_dense->rw(af),a,this);
    return af;
  }

  class ColumnIterator
  {
  public:
    virtual Object extractColumn(ndx_t column) = 0;
  };

  class NullIterator : public ColumnIterator
  {
    Object _obj;
  public:
    NullIterator(Object obj) : _obj(obj) {}
    Object extractColumn(ndx_t) {
      return _obj;
    }
  };

  class DenseColumnIterator : public ColumnIterator
  {
    const Object &_obj;
  public:
    DenseColumnIterator(const Object &obj) : _obj(obj) {}
    Object extractColumn(ndx_t column) {
      return _obj.type()->sliceColumn(_obj,column);
    }
  };

  template <class T>
  class SparseColumnIterator : public ColumnIterator
  {
    const Object &_obj;
    Object _buffer;
    SparseType<T> *_type;
  public:
    SparseColumnIterator(SparseType<T> *type, const Object &obj) :
      _obj(obj), _buffer(type->context()), _type(type) {
      _buffer = type->dense()->makeMatrix(_obj.rows(),1);
    }
    Object extractColumn(ndx_t column) {
      _type->extractDenseColumn(_obj,column,_buffer);
      return _buffer;
    }
  };

  template <class T>
  static std::unique_ptr<ColumnIterator> makeSparseColumnIterator(SparseType<T> *type, const Object &obj) {
    if (obj.isScalar())
      return (std::unique_ptr<ColumnIterator>)(new NullIterator(type->scalarObject(obj)));
    else
      return (std::unique_ptr<ColumnIterator>)(new SparseColumnIterator<T>(type,obj));
  }

  static std::unique_ptr<ColumnIterator> makeColumnIterator(ThreadContext *ctxt, const Object &obj) {
    if (obj.isSparse()) {
      switch (obj.typeCode()) {
      case TypeSparseLogical:
	return makeSparseColumnIterator(ctxt->_splogical,obj);
      case TypeSparseDouble:
	return makeSparseColumnIterator(ctxt->_spdouble,obj);
      case TypeSparseComplex:
	return makeSparseColumnIterator(ctxt->_spcomplex,obj);
      default:
	throw Exception("Unhandled sparse type");
      }
    } else {
      if (obj.isScalar())
	return (std::unique_ptr<ColumnIterator>)(new NullIterator(obj));
      else
	return (std::unique_ptr<ColumnIterator>)(new DenseColumnIterator(obj));
    }
  }

  //TODO - remove ThreadContext arg - it is redundant
  typedef Object (Type::*TypeMemFn)(const Object &x, const Object &y);
  
  template <class T>
  static Object SparseSparseDotOperation(const Object &a, const Object &b, ThreadContext *ctxt,
					 SparseType<T> *return_type, TypeMemFn func) {
    Object ret(return_type->empty());
    ret.setDims(Tuple::computeDotOpSize(a.dims(),b.dims()));
    auto aiter(makeColumnIterator(ctxt,a));
    auto biter(makeColumnIterator(ctxt,b));
    for (ndx_t col=0;col<ret.cols();col++) {
      Object acol = aiter->extractColumn(col);
      Object bcol = biter->extractColumn(col);
      Object ccol = (*(return_type->dense()).*func)(acol,bcol);
      return_type->sparseifyColumn(ret,col,ccol);
    }
    return ret;
  }

  template <class T>
  static Object SparseFullDotOperation(const Object &a, const Object &b, ThreadContext *ctxt,
				       PODType<T> *return_type, TypeMemFn func) {
    Tuple rsize(Tuple::computeDotOpSize(a.dims(),b.dims()));
    Object ret(return_type->makeMatrix(rsize.rows(),rsize.cols()));
    auto aiter(makeColumnIterator(ctxt,a));
    auto biter(makeColumnIterator(ctxt,b));
    for (ndx_t col=0;col<ret.cols();col++) {
      Object acol = aiter->extractColumn(col);
      Object bcol = biter->extractColumn(col);
      Object ccol = (*(return_type).*func)(acol,bcol);
      return_type->setColumn(ret,col,ccol);
    }
    return ret;
  }

  template <class T>
  static Object convertSparseType(const Object &a, SparseType<T> *typ) {
    if (a.isSparse()) return typ->convert(a);
    return typ->dense()->convert(a);
  }

  template <class via, class result>
  static Object DispatchSparseSparse(const Object &a, const Object &b, SparseType<via> *typ,
				     SparseType<result> *res, ThreadContext *ctxt,
				     TypeMemFn func, int sparseFlags) {
    Object ca(convertSparseType(a,typ));
    Object cb(convertSparseType(b,typ));
    if ((ca.isSparse() && cb.isSparse()) || ((sparseFlags & SPARSE_OP_PRESERVES_SPARSITY) != 0))
      return SparseSparseDotOperation(ca,cb,ctxt,res,func);
    return SparseFullDotOperation(ca,cb,ctxt,res->dense(),func);
  }

  // TODO - handle Complex+Real, mixed sparse, dense cases
  // TODO handle SPARSE_OP_IS_SPARSITY_PRESERVING flag

  static Object DispatchSparseSparseOperation(const Object &a, const Object &b, ThreadContext *ctxt,
					      TypeMemFn func, int sparseFlags)
  {
    if (sparseFlags & SPARSE_OP_IS_BOOL_VALUED) {
      if (a.isComplex() || b.isComplex()) 
	return DispatchSparseSparse(a,b,ctxt->_spcomplex,ctxt->_splogical,ctxt,func,sparseFlags);
      return DispatchSparseSparse(a,b,ctxt->_spdouble,ctxt->_splogical,ctxt,func,sparseFlags);
    }
    if (a.isComplex() || b.isComplex()) 
      return DispatchSparseSparse(a,b,ctxt->_spcomplex,ctxt->_spcomplex,ctxt,func,sparseFlags);
    return DispatchSparseSparse(a,b,ctxt->_spdouble,ctxt->_spdouble,ctxt,func,sparseFlags);
  }

  template <>
  Object SparseType<Complex<double> >::makeScalar() {
    Object ret = AggregateType<SparseData<Complex<double> >,ValueSemantics>::makeScalar();
    SparseData<Complex<double> > *sd = this->rw(ret);
    sd->m_nnz = 0;
    //    ret.d->flags = OBJECT_COMPLEX_FLAG;
    return ret;
  }

  // template <>
  // Object SparseType<Complex<double> >::empty() {
  //   Object ret = AggregateType<SparseData<Complex<double> >,ValueSemantics>::empty();
  // }
  
  template <class T>
  Object SparseType<T>::LessEquals(const Object &a, const Object &b) {
    return DispatchSparseSparseOperation(a,b,Type::context(),&Type::LessEquals,SPARSE_OP_IS_BOOL_VALUED);
  }

  template <class T>
  Object SparseType<T>::LessThan(const Object &a, const Object &b) {
    return DispatchSparseSparseOperation(a,b,Type::context(),&Type::LessThan,SPARSE_OP_IS_BOOL_VALUED);
  }

  template <class T>
  Object SparseType<T>::GreaterThan(const Object &a, const Object &b) {
    return DispatchSparseSparseOperation(a,b,Type::context(),&Type::GreaterThan,SPARSE_OP_IS_BOOL_VALUED);
  }

  template <class T>
  Object SparseType<T>::GreaterEquals(const Object &a, const Object &b) {
    return DispatchSparseSparseOperation(a,b,Type::context(),&Type::GreaterEquals,SPARSE_OP_IS_BOOL_VALUED);
  }

  template <class T>
  Object SparseType<T>::NotEquals(const Object &a, const Object &b) {
    return DispatchSparseSparseOperation(a,b,Type::context(),&Type::NotEquals,SPARSE_OP_IS_BOOL_VALUED);
  }

  template <class T>
  Object SparseType<T>::Equals(const Object &a, const Object &b) {
    return DispatchSparseSparseOperation(a,b,Type::context(),&Type::Equals,SPARSE_OP_IS_BOOL_VALUED);
  }

  template <class T>
  Object SparseType<T>::Add(const Object &a, const Object &b) {
    return DispatchSparseSparseOperation(a,b,Type::context(),&Type::Add,0);
  }

  template <class T>
  Object SparseType<T>::Subtract(const Object &a, const Object &b) {
    return DispatchSparseSparseOperation(a,b,Type::context(),&Type::Subtract,0);
  }

  template <class T>
  Object SparseType<T>::DotLeftDivide(const Object &a, const Object &b) {
    return DispatchSparseSparseOperation(a,b,Type::context(),&Type::DotLeftDivide,0);
  }

  template <class T>
  Object SparseType<T>::DotRightDivide(const Object &a, const Object &b) {
    return DispatchSparseSparseOperation(a,b,Type::context(),&Type::DotRightDivide,0);
  }

  template <class T>
  Object SparseType<T>::DotMultiply(const Object &a, const Object &b) {
    return DispatchSparseSparseOperation(a,b,Type::context(),&Type::DotMultiply,SPARSE_OP_PRESERVES_SPARSITY);
  }

  template <class T>
  Object SparseType<T>::DotPower(const Object &a, const Object &b) {
    return DispatchSparseSparseOperation(a,b,Type::context(),&Type::DotPower,0);
  }

  template <class T>
  Object SparseType<T>::Or(const Object &a, const Object &b) {
    return DispatchSparseSparse(a,b,Type::context()->_splogical,Type::context()->_splogical,Type::context(),&Type::Or,0);
  }

  template <class T>
  Object SparseType<T>::And(const Object &a, const Object &b) {
    return DispatchSparseSparse(a,b,Type::context()->_splogical,Type::context()->_splogical,Type::context(),&Type::And,SPARSE_OP_PRESERVES_SPARSITY);
  }


  template <class T>
  static Object SparseMatrixMultiplyInner(const Object &a, ndx_t brows,
					  std::unique_ptr<ColumnIterator> &biter, ndx_t col,
					  SparseType<T> *typ) {
    ThreadContext *ctxt = typ->context();
    Object bcol = biter->extractColumn(col);
    T* bptr = typ->dense()->rw(bcol);
    auto aiter(makeColumnIterator(ctxt,a));
    Object scratch = typ->dense()->makeMatrix(a.rows(),1);
    T* sptr = typ->dense()->rw(scratch);
    for (ndx_t mid=0;mid<brows;mid++) {
      Object acol = aiter->extractColumn(mid);
      T* aptr = typ->dense()->rw(acol);
      for (ndx_t row=0;row<a.rows();row++)
	sptr[mid] = sptr[mid] + bptr[mid]*aptr[row];
    }
    return scratch;
  }
  
  template <class T>
  static Object SparseSparseMatrixMultiply(const Object &a, const Object &b, SparseType<T> *typ) {
    Object ret(typ->empty());
    ret.setDims(Tuple::Tuple(a.rows(),b.cols()));
    ThreadContext *ctxt = typ->context();
    auto biter(makeColumnIterator(ctxt,b));
    for (ndx_t col=0;col<b.cols();col++)
      typ->sparseifyColumn(ret,col,SparseMatrixMultiplyInner<T>(a,b.rows(),biter,col,typ));
    return ret;
  }

  template <class T>
  static Object SparseFullMatrixMultiply(const Object &a, const Object &b, SparseType<T> *typ) {
    Object ret(typ->dense()->makeMatrix(a.rows(),b.cols()));
    ThreadContext *ctxt = typ->context();
    auto biter(makeColumnIterator(ctxt,b));
    for (ndx_t col=0;col<b.cols();col++) 
      typ->dense()->setColumn(ret,col,SparseMatrixMultiplyInner<T>(a,b.rows(),biter,col,typ));
    return ret;
  }
  
  template <class T>
  static Object DispatchSparseMatrixMultiply(const Object &a, const Object &b, SparseType<T> *typ) {
    if (a.cols() != b.rows())
      throw Exception("Sparse matrix multiplication requires arguments to be conformant");
    Object ca(convertSparseType(a,typ));
    Object cb(convertSparseType(b,typ));
    if (ca.isSparse() && cb.isSparse())
      return SparseSparseMatrixMultiply(ca,cb,typ);
    return SparseFullMatrixMultiply(ca,cb,typ);
  }

  
  template <class T>
  Object SparseType<T>::Multiply(const Object &a, const Object &b) {
    if (a.isScalar() || b.isScalar()) return DotMultiply(a,b);
    if (a.isComplex() || b.isComplex())
      return DispatchSparseMatrixMultiply(a,b,Type::context()->_spcomplex);
    return DispatchSparseMatrixMultiply(a,b,Type::context()->_spdouble);
  }
  
  template class SparseType<double>;
  template class SparseType<bool>;
  template class SparseType<Complex<double> >;

}
