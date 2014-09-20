#include "PODType.hpp"
#include "BaseTypes.hpp"
#include "IntegerType.hpp"
#include "ListType.hpp"
#include "ThreadContext.hpp"

using namespace FM;

template <class T>
static void copyPage(T* op, dim_t &outputOffset, const T* ip, dim_t &input_offset, dim_t pagesize)
{
  for (dim_t j=0;j<pagesize;j++)
    op[outputOffset+j] = ip[input_offset+j];
  outputOffset += pagesize;
  input_offset += pagesize;
}

template <class T>
static void copyPagePromote(Complex<T>* op, dim_t outputOffset, const T* ip, dim_t &input_offset, dim_t pagesize)
{
  for (dim_t j=0;j<pagesize;j++)
    op[outputOffset+j] = Complex<T>(ip[input_offset+j]);
  outputOffset += pagesize;
  input_offset += pagesize;
}


/**
 * The move algorithm.  The original data samples are at:
 *  old(i,j,k) = i + j*ax + k*ax*ay
 * And data samples are now at
 *  new(i,j,k) = i + j*nx + k*nx*ny
 * where nx >= ax, ny >= ay, etc.
 * The delta is then:
 *  delta(i,j,k) = new(i,j,k) - old(i,j,k)
 *               = j*(nx-ax) + k(nx*ny - ax*ay)
 * This is why the current algorithm doesn't work.
 */
template <class T>
static void moveLoop(T* ap, const Tuple &outdims, const Tuple &adims)
{
  int ndims = std::max<int>(adims.dimensions(),outdims.dimensions());
  dim_t adimraw[MAXDIMS];
  for (int i=0;i<ndims;i++) adimraw[i] = adims.dimension(i);
  // Check for a no-op - if only the last dimension increases, 
  // it's a no-op
  bool allsame = true;
  for (int i=0;i<ndims-1;i++)
    allsame = allsame && (adimraw[i] == outdims.dimension(i));
  if (allsame) return;
  dim_t iterations = 1;
  for (int i=1;i<ndims;i++)
    iterations *= adimraw[i];
  ndx_t ndx[MAXDIMS];
  dim_t strides[MAXDIMS];
  strides[0] = 1;
  ndx[0] = adimraw[0]-1;
  for (int i=1;i<ndims;i++)
    {
      strides[i] = strides[i-1]*outdims.dimension(i-1);
      ndx[i] = adimraw[i]-1;
    }
  dim_t arows = adims.rows();
  dim_t orows = outdims.rows();
  dim_t aoffset = adims.elementCount()-1;
  for (dim_t iter=0;iter<iterations;iter++) {
    // Compute the start address
    dim_t start = 0;
    for (int i=0;i<ndims;i++)
      start += ndx[i]*strides[i];
    assert(start >= aoffset);
    if (aoffset != start)
      for (dim_t row=0;row<arows;row++)
      {
	ap[start-row] = ap[aoffset-row];
	ap[aoffset-row] = T();
      }
    aoffset -= arows;
    for (dim_t row=0;row<orows-arows;row++)
	ap[start+1+row] = T();
    ndx[1]--;
    for (int i=1;i<ndims;i++)
      if (ndx[i] < 0)
	{
	  ndx[i] = adimraw[i] - 1;
	  ndx[i+1]--;
	}
  }
}

template <class T>
static void copyLoop(T* op, const T* ip, const Tuple & outdims, const Tuple & adims)
{
  // Compute the number of iterations required by the RESIZE
  // Excludes the first dimension. 
  int ndims = std::max<int>(adims.dimensions(),outdims.dimensions());
  dim_t adimraw[MAXDIMS];
  for (int i=0;i<ndims;i++) adimraw[i] = adims.dimension(i);
  dim_t iterations = 1;
  for (int i=1;i<ndims;i++)
    iterations *= adimraw[i];
  ndx_t ndx[MAXDIMS];
  dim_t strides[MAXDIMS];
  strides[0] = 1;
  for (int i=1;i<ndims;i++)
    {
      strides[i] = strides[i-1]*outdims.dimension(i-1);
      ndx[i] = 0;
    }
  // Loop over the iterations
  dim_t arows = adims.rows();
  for (dim_t iter=0;iter<iterations;iter++) {
    // Compute the start address
    dim_t start = 0;
    for (int i=1;i<ndims;i++)
      start += ndx[i]*strides[i];
    for (dim_t row=0;row<arows;row++)
      op[row+start] = ip[row];
    ip += arows;
    ndx[1]++;
    for (int i=1;i<ndims;i++)
      if (ndx[i] >= adimraw[i])
	{
	  ndx[i] = 0;
	  ndx[i+1]++;
	}
  }
}

// This algorithm is responsible for the set -- it is a bit tricky (for performance reasons), so here are the details.
//
// op - points to the beginning of the output array (resized to be large enough already)
// argdim - length of argument arrays, i.e., argdim[i] = numel(coords[i])
// adims - the dimensions of the array being modified
// ndims - the number of dimensions for the indexing array, i.e., numel(coords)
// bp - pointer to the beginning of the RHS array
// isScalar - flag that indicates if RHS is a scalar, and so replication is required.

template <class T>
static void setLoop(T* op, const dim_t (&argdim) [MAXDIMS], const Tuple & adims, int ndims, const ndx_t *(&coords)[MAXDIMS], const T* bp, bool isScalar)
{
  dim_t ndx[MAXDIMS];
  dim_t strides[MAXDIMS];
  ndx[0] = 0;
  strides[0] = 1;
  // Compute the number of iterations required by the set
  // Excludes the first dimension.  Also computes the strides
  // and initializes the index array to all zeros
  dim_t iterations = 1;
  for (int i=1;i<ndims;i++) {
    iterations *= argdim[i];
    ndx[i] = 0;
    strides[i] = strides[i-1]*adims.dimension(i-1);
  }
  // Loop over the iterations
  for (dim_t iter=0;iter<iterations;iter++) {
    // Compute the start address
    size_t start = 0;
    for (int i=1;i<ndims;i++) {
      start += coords[i][ndx[i]]*strides[i];
      ndx[i]++;
      for (int j=i;j<ndims;j++)
	if (ndx[j] >= argdim[j])
	  {
	    ndx[j] = 0;
	    ndx[j+1]++;
	  }
    }
    for (dim_t row=0;row<argdim[0];row++)
      {
	op[start + coords[0][row]] = *bp;
	if (!isScalar) bp++;
      }
  }
}

template <class T>
static void getRowLoop(T* op, const T* ip, const ndx_t *ndx, ndx_t count)
{
  for (int i=0;i<count;i++)
    op[i] = ip[ndx[i]];
}

template <class T>
static void getLoop(T* op, const T* ip, const dim_t (&outdim)[MAXDIMS], const Tuple & adims, int ndims, const ndx_t * (&coords)[MAXDIMS])
{
  dim_t ndx[MAXDIMS];
  dim_t strides[MAXDIMS];
  ndx[0] = 0;
  strides[0] = 1;
  // Compute the number of iterations required by the GET
  // Excludes the first dimension.  Also computes the strides
  // and initializes the index array to all zeros
  dim_t iterations = 1;
  for (int i=1;i<ndims;i++) {
    iterations *= outdim[i];
    ndx[i] = 0;
    strides[i] = strides[i-1]*adims.dimension(i-1);
  }
  // Loop over the iterations
  for (dim_t iter=0;iter<iterations;iter++) {
    // Compute the start address
    size_t start = 0;
    for (int i=1;i<ndims;i++) {
      start += coords[i][ndx[i]]*strides[i];
      ndx[i]++;
      for (int j=i;j<ndims;j++)
	if (ndx[j] >= outdim[j])
	  {
	    ndx[j] = 0;
	    ndx[j+1]++;
	  }
    }
    for (dim_t row=0;row<outdim[0];row++)
      op[row] = ip[start + coords[0][row]];
    op += outdim[0];
  }
}

// FIXME - Handle A(:)
template <class T, bool _objType>
Object PODType<T,_objType>::getParensRowMode(const Object &a, const Tuple &dims, const Object &b)
{
  if (_ctxt->_index->isColon(b))
    {
      Object output = a;
      output.dims() = Tuple(a.elementCount(),1);
      return output;
    }
  Object output = this->zeroArrayOfSize(dims,a.isComplex());
  if (a.isComplex())
    getRowLoop<Complex<T> >(this->readWriteDataComplex(output),this->readOnlyDataComplex(a),
			    _ctxt->_index->readOnlyData(b),b.elementCount());
  else
    getRowLoop<T>(this->readWriteData(output),this->readOnlyData(a),
		  _ctxt->_index->readOnlyData(b),b.elementCount());
  return output;
}

// TODO - handle (:,:,x) indexing!
static inline int isSliceIndexCase(ThreadContext *ctxt, Object (&c)[MAXDIMS], int cnt)
{
  int last_colon = 0;
  while (last_colon < cnt && ctxt->_index->isColon(c[last_colon])) last_colon++;
  if (last_colon == 0) return 0; // Need at least one colon
  for (int i=last_colon;i<cnt;i++)
    if (ctxt->_index->isColon(c[i]) || !c[i].isScalar()) return 0;
  return last_colon;
}

template <class T, bool _objType>
Object PODType<T,_objType>::getSliceMode(const Object &a, Object (&c)[MAXDIMS], int cnt, int last_colon)
{
  // Compute the index of the first element of the array
  dim_t offset = 0;
  const Tuple& adims = a.dims();
  dim_t stride = adims.dimension(0);
  Tuple slicedims;
  slicedims.setRows(adims.dimension(0));
  for (int i=1;i<cnt;i++)
    {
      if (i >= last_colon)
	{
	  offset += stride*_ctxt->_index->scalarValue(c[i]);
	  slicedims.set(i,1);
	}
      else
	slicedims.set(i,adims.dimension(i));
      stride *= adims.dimension(i);
    }
  if (cnt <= 2)
    slicedims.set(1,1);
  ObjectBase *q = new ObjectBase(a.d->data,
				 a.d->type,
				 offset,
				 slicedims,
				 a.d->flags,
				 slicedims.elementCount());
  q->refcnt = 0;
  return Object(q);
}

template <class T, bool _objType>
Object PODType<T,_objType>::getParens(const Object &a, const Object &b) {
  // FIXME - b is empty?
  if (b.elementCount() == 1) 
    {
      Object ndx = _ctxt->_list->first(b);
      return getParensRowMode(a,ndx.dims(),ndx.asIndex(a.elementCount()));
    }
  const Tuple & adims(a.dims());
  // TODO: Trim trailing singular dimensions.  Make sure b.size() < a.numDims.
  // TODO: Special case all-scalar indexing case
  Object c[MAXDIMS];
  int bsize = b.elementCount();
  const Object *bp = _ctxt->_list->readOnlyData(b);
  for (int i=0;i<bsize;i++)
    c[i] = bp[i].asIndex(adims.dimension(i));
  // TODO: Add slice test here (c.f. isSliceIndexCase)
  int last_colon = isSliceIndexCase(_ctxt,c,bsize);
  if (last_colon > 0) {
    return getSliceMode(a,c,bsize,last_colon);
  }
  // Expand colons, and do bounds check
  if (bsize > MAXDIMS) 
    throw Exception(FMString("FreeMat is compiled for maximum ") 
		    + Stringify(MAXDIMS) + FMString(" dimensions.  Indexing expression ")
		    + FMString(" requires too many dimensions."));
  dim_t outdim[MAXDIMS];
  const ndx_t *coords[MAXDIMS];
  for (int i=0;i<bsize;i++)
    {
      IndexType *ip = static_cast<IndexType*>(c[i].type());
      if (ip->isColon(c[i]))
	c[i] = ip->expandColons(adims.dimension(i));
      outdim[i] = c[i].elementCount();
      coords[i] = ip->readOnlyData(c[i]);
    }
    Object output = this->zeroArrayOfSize(Tuple::RawTuple(outdim,bsize),a.isComplex());
  if (a.isComplex())
    getLoop<Complex<T> >(this->readWriteDataComplex(output),
			 this->readOnlyDataComplex(a),
			 outdim,adims,bsize,coords);
  else
    getLoop<T>(this->readWriteData(output),this->readOnlyData(a),outdim,adims,bsize,coords);
  return output;
}

template <class T, bool _objType>
void PODType<T,_objType>::resize(Object &a, const Tuple &newsize) {
  if (a.elementCount() == 0)
    {
      if (a.d->capacity > newsize.elementCount())
	{
	  a.dims() = newsize;
	  return;
	}
      a = this->zeroArrayOfSize(newsize,a.isComplex());
      return;
    }
  // Determine if this is a move or a copy
  if (a.d->capacity > newsize.elementCount()) { 
    if (a.isComplex())
      moveLoop<Complex<T> >(this->readWriteDataComplex(a),
			    newsize,a.dims());
    else
      moveLoop<T>(this->readWriteData(a),newsize,a.dims());
  } else {
    // Copy
    Object na = this->zeroArrayOfSize(newsize,a.isComplex());
    if (a.isComplex())
      copyLoop<Complex<T> >(this->readWriteDataComplex(na),
			    this->readOnlyDataComplex(a),
			    newsize,a.dims());
    else
      copyLoop<T>(this->readWriteData(na),this->readOnlyData(a),newsize,a.dims());
    a = na;
  }
  a.dims() = newsize;
}

template <class T>
void setLoopRowMode(T* a, const ndx_t *addr, dim_t count, const T* b, bool bScalar)
{
  for (dim_t i=0;i<count;i++)
    {
      a[addr[i]] = *b;
      if (!bScalar) ++b;
    }
}

template <class T, bool _objType>
void PODType<T,_objType>::setParensRowIndexMode(Object &a, const Object &ndx, const Object &b) {
  // First check for resize
  dim_t maxndx = _ctxt->_index->maxValue(ndx)+1;
  if (maxndx > a.elementCount())
    {
      Tuple newdims;
      // This logic is taken from BasicArray::resize(index_t) in FM4
      if (a.isScalar() || a.isEmpty())
	newdims = Tuple(1,maxndx);
      else if (a.isVector()) {
	if (a.rows() != 1)
	  newdims = Tuple(maxndx,1);
	else
	  newdims = Tuple(1,maxndx);
      } else {
	newdims = Tuple(1,maxndx);
      }
      a.type()->resize(a,newdims);
    }
  const ndx_t *addr = _ctxt->_index->readOnlyData(ndx);
  // TODO - handle b is complex, a is real
  if (!a.isComplex() && b.isComplex())
    this->promoteComplex(a);
  if (a.isComplex())
    setLoopRowMode<Complex<T> >(this->readWriteDataComplex(a),
				addr,ndx.elementCount(),
				this->readOnlyDataComplex(b),
				b.isScalar());
  else
    setLoopRowMode<T>(this->readWriteData(a),
		      addr,ndx.elementCount(),
		      this->readOnlyData(b),
		      b.isScalar());
}

template <class T, bool _objType>
void PODType<T,_objType>::promoteComplex(Object &a) {
  if (a.isComplex()) return;
  Object o = zeroArrayOfSize(a.dims(),true);
  Complex<T> *op = this->readWriteDataComplex(o);
  const T *ip = this->readOnlyData(a);
  for (dim_t i=0;i<a.elementCount();i++)
    op[i] = Complex<T>(ip[i]);
  a = o;
}

template <class T, bool _objType>
void PODType<T,_objType>::setParens(Object &a, const Object &args, const Object &b) {
  if (args.elementCount() == 1)
    {
      Object ndx = _ctxt->_list->first(args);
      return setParensRowIndexMode(a,ndx.asIndexNoBoundsCheck(),b);
    }
  Tuple & adims(a.dims());
  // TODO: Trim trailing singular dimensions.  Make sure b.size() < a.numDims.
  // TODO: Special case all-scalar indexing case
  Object c[MAXDIMS];
  int argsize = args.elementCount();
  const Object *argsp = args.asType<ListType>()->readOnlyData(args);
  for (int i=0;i<argsize;i++)
    c[i] = argsp[i].asIndexNoBoundsCheck();
  // TODO: Add slice test here (c.f. isSliceIndexCase)
  // Expand colons, and do bounds check
  if (argsize > MAXDIMS) 
    throw Exception(FMString("FreeMat is compiled for maximum ") 
		    + Stringify(MAXDIMS) + FMString(" dimensions.  Indexing expression ")
		    + FMString(" requires too many dimensions."));
  dim_t argdim[MAXDIMS];
  dim_t outdim[MAXDIMS];
  const ndx_t *coords[MAXDIMS];
  bool resize_required = false;
  dim_t outcount = 1;
  for (int i=0;i<argsize;i++)
    {
      IndexType *ip = static_cast<IndexType*>(c[i].type());
      if (ip->isColon(c[i]))
	c[i] = ip->expandColons(adims.dimension(i));
      argdim[i] = c[i].elementCount();
      coords[i] = ip->readOnlyData(c[i]);
      outdim[i] = std::max<dim_t>(adims.dimension(i),ip->maxValue(c[i])+1);
      outcount *= argdim[i];
      resize_required |= (outdim[i] > adims.dimension(i));
    }
  // Check if the output requires resizing of the array
  if (resize_required)
    {
      // StdIOTermIF io;
      //      _ctxt->_io->output("Before resize:\n");
      //      a.type()->print(a);
      assert(a.isValid());
      a.type()->resize(a,Tuple::RawTuple(outdim,argsize));
      //      _ctxt->_io->output("After resize:\n");
      //      a.type()->print(a);
      adims = a.dims();
    }      
  // Check for validity of b
  if (!(b.isScalar() || b.elementCount() == outcount))
    throw Exception("Assignment A(...) = b requires b either be a scalar or have the correct number of elements");
  if (!a.isComplex() && b.isComplex())
    this->promoteComplex(a);
  if (a.isComplex())
    setLoop<Complex<T> >(this->readWriteDataComplex(a),
			 argdim,adims,argsize,coords,
			 this->readOnlyDataComplex(b),b.isScalar());
  else
    setLoop<T>(this->readWriteData(a),
	       argdim,adims,argsize,coords,
	       this->readOnlyData(b),b.isScalar());
}

template <class T, bool _objType>
void PODType<T,_objType>::printSheet(const Object &a, const ArrayFormatInfo &format, ndx_t offset) {
  ndx_t columns = a.cols();
  ndx_t rows = a.rows();
  int colsPerPage = (int) floor((_ctxt->_io->getTerminalWidth()-1)/((double) format.width + 2));
  colsPerPage = (colsPerPage < 1) ? 1 : colsPerPage;
  int pageCount = (int) ceil(columns/((double)colsPerPage));
  for (int k=0;k<pageCount;k++) {
    int colsInThisPage;
    colsInThisPage = columns - colsPerPage*k;
    colsInThisPage = (colsInThisPage > colsPerPage) ? 
      colsPerPage : colsInThisPage;
    if ((rows*columns > 1) && (pageCount > 1))
      _ctxt->_io->output("\n Columns %d to %d\n\n",k*colsPerPage+1,k*colsPerPage+colsInThisPage);
    for (int i=0;i<rows;i++) {
      for (int j=0;j<colsInThisPage;j++)
	this->printElement(a,format,i+(k*colsPerPage+j)*rows+offset);
      _ctxt->_io->output("\n");
    }
  }
  _ctxt->_io->output("\n");
}

template <class T, bool _objType>
void PODType<T,_objType>::print(const Object &a)
{
  if (a.isComplex())
    _ctxt->_io->output("A is complex ");
  if (a.isEmpty())
  {
    if (a.dims() == Tuple(0,0)) {
      _ctxt->_io->output("  []\n");
    } else {
      _ctxt->_io->output("  Empty array ");
      _ctxt->_io->output(a.dims().toString());
      _ctxt->_io->output("\n");
    }
  }
  ArrayFormatInfo format;
  this->computeArrayFormatInfo(_ctxt->_io->getFormatMode(),a,format);
  if (!a.isScalar() && (format.scalefact != 1))
    _ctxt->_io->output("\n   %.1e * \n",format.scalefact);
  if (a.isScalar() && !format.floatasint) {
    if (fabs(log10(format.scalefact)) > 3) {
      format.expformat = true;
    } else {
      format.scalefact = 1;
    }
  }
  if (a.is2D())
    this->printSheet(a,format,0);
  else {
    /**
     * For N-ary arrays, data slice  -  start with 
     * [1,1,1,...,1].  We keep doing the matrix
     * print, incrementing from the highest dimension,
     * and rolling downwards.
     */
    _ctxt->_io->output("\n");
    dim_t offset = 0;
    dim_t page = a.rows()*a.cols();
    dim_t page_count = a.elementCount()/page;
    for (dim_t p=0;p<page_count;p++)
      {
	Tuple w = offset % a.dims();
	_ctxt->_io->output("(:,:");
	for (int m=2;m<w.dimensions();m++) 
	  _ctxt->_io->output(FMString(",") + Stringify(w.dimension(m)));
	_ctxt->_io->output(") = \n");
	this->printSheet(a,format,offset);
	offset += page;
      }
  }
}


// Preconditions - all objects must be the same size
// Zero objects are removed
// Mixed real/complex objects are all promoted to 
// Complex case needs to be considered!
template <class T, bool _objType>
Object PODType<T,_objType>::NCat(const Object& p, int dim)
{
  int objectCount = p.elementCount();
  const Object *dp = _ctxt->_list->readOnlyData(p);
  if (objectCount == 0) return this->empty();
  // Compute the size of the output
  Tuple outputSize = dp[0].dims();
  // Sum the components along the given dimension
  dim_t aggregated_size = 0;
  for (int i=0;i<objectCount;i++)
    aggregated_size += dp[i].dims().dimension(dim);
  outputSize.set(dim,aggregated_size);
  std::cout << "Aggregated size: " << aggregated_size << "  " << outputSize.toString() << "\n";
  // Cache the pointers to the objects
  std::vector<const T*> pointers(objectCount);
  // Cache the current indices into each object
  std::vector<dim_t> offsets(objectCount);
  // Cache the page size for each object
  std::vector<dim_t> pagesize(objectCount);
  // Cache the promotion flag for each object
  std::vector<bool> promotionRequired(objectCount);
  // Determine if promotion to complex is required
  // Required if any of the objects is complex, but
  // not all of them.
  bool anyComplex = false;
  bool allComplex = true;
  for (int i=0;i<objectCount;i++)
    {
      anyComplex |= dp[i].isComplex();
      allComplex &= dp[i].isComplex();
    }
  for (int i=0;i<objectCount;i++)
    {
      pointers[i] = this->readOnlyData(dp[i]);
      offsets[i] = 0;
      dim_t pagesze = 1;
      for (int j=0;j<=dim;j++)
	pagesze *= dp[i].dims().dimension(j);
      pagesize[i] = pagesze;
      promotionRequired[i] = anyComplex && (!allComplex) && (!dp[i].isComplex());
    }
  // Allocate the output object
  Object retObject = this->zeroArrayOfSize(outputSize,anyComplex);
  dim_t outputOffset = 0;
  dim_t outputCount = outputSize.elementCount();
  int k = 0;
  T* op = this->readWriteData(retObject);
  while (outputOffset < outputCount)
    {
      if (!anyComplex)
	copyPage<T>(op,outputOffset,pointers[k],offsets[k],pagesize[k]);
      else if (anyComplex && !promotionRequired[k])
	copyPage<Complex<T> >(reinterpret_cast<Complex<T>*>(op),outputOffset,
			      reinterpret_cast<const Complex<T>*>(pointers[k]),offsets[k],pagesize[k]);
      else
	copyPagePromote<T>(reinterpret_cast<Complex<T>*>(op),outputOffset,
			   pointers[k],offsets[k],pagesize[k]);
      k = (k + 1) % objectCount;
    }
  return retObject;
}


template class PODType<char,false>;
template class PODType<float,false>;
template class PODType<double,false>;
template class PODType<Object,true>;
template class PODType<int32_t,false>;
template class PODType<uint32_t,false>;
template class PODType<int64_t,false>;
template class PODType<uint64_t,false>;
template class PODType<bool,false>;
//template class PODType<ndx_t,false>;

