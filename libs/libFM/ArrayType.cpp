#include "ArrayType.hpp"
#include "BaseTypes.hpp"
#include "IntegerType.hpp"
#include "ListType.hpp"
#include "ThreadContext.hpp"

using namespace FM;

template <class T>
static void copyPage(T* op, dim_t &outputOffset, const T* ip, dim_t &input_offset, dim_t pagesize, bool isComplex)
{
  if (!isComplex)
    {
      for (dim_t j=0;j<pagesize;j++)
	op[outputOffset+j] = ip[input_offset+j];
    }
  else
    {
      for (dim_t j=0;j<pagesize;j++)
	{
	  op[2*(outputOffset+j)] = ip[2*(input_offset+j)+1];
	  op[2*(outputOffset+j)+1] = ip[2*(input_offset+j)+1];
	}
    }
  outputOffset += pagesize;
  input_offset += pagesize;
}

template <class T>
static void copyPagePromote(T* op, dim_t outputOffset, const T* ip, dim_t &input_offset, dim_t pagesize, const T& zero)
{
  for (dim_t j=0;j<pagesize;j++)
    {
      op[2*(outputOffset+j)] = ip[input_offset+j];
      op[2*(outputOffset+j)+1] = zero;
    }
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
static void moveLoop(T* ap, const Tuple &outdims, const Tuple &adims, const T &zero, bool isComplex)
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
      {
      if (!isComplex)
	{
	  for (dim_t row=0;row<arows;row++)
	    {
	      ap[start-row] = ap[aoffset-row];
	      ap[aoffset-row] = zero;
	    }
	}
      else
	{
	  for (dim_t row=0;row<arows;row++)
	    {
	      ap[2*(start-row)] = ap[2*(aoffset-row)];
	      ap[2*(start-row)+1] = ap[2*(aoffset-row)+1];
	      ap[2*(aoffset-row)] = zero;
	      ap[2*(aoffset-row)+1] = zero;
	    }	  
	}
      }
    aoffset -= arows;
    if (!isComplex)
      {
	for (dim_t row=0;row<orows-arows;row++)
	  ap[start+1+row] = zero;
      }
    else
      {
	for (dim_t row=0;row<orows-arows;row++)
	  {
	    ap[2*(start+1+row)] = zero;
	    ap[2*(start+1+row)+1] = zero;
	  }
      }
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
static void copyLoop(T* op, const T* ip, const Tuple & outdims, const Tuple & adims, bool isComplex)
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
    if (!isComplex)
      {
	for (dim_t row=0;row<arows;row++)
	  op[row+start] = ip[row];
	ip += arows;
      }
    else
      {
	for (dim_t row=0;row<arows;row++)
	  {
	    op[2*(row+start)] = ip[2*row];
	    op[2*(row+start)+1] = ip[2*row+1];
	  }
	ip += 2*arows;
      }
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
static void setLoop(T* op, const dim_t (&argdim) [MAXDIMS], const Tuple & adims, int ndims, const ndx_t *(&coords)[MAXDIMS], const T* bp, bool isScalar,
		    bool isComplex)
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
    if (!isComplex)
      {
	for (dim_t row=0;row<argdim[0];row++)
	  {
	    op[start + coords[0][row]] = *bp;
	    if (!isScalar) bp++;
	  }
      }
    else
      {
	for (dim_t row=0;row<argdim[0];row++)
	  {
	    op[2*(start + coords[0][row])] = bp[0];
	    op[2*(start + coords[0][row])+1] = bp[1];
	    if (!isScalar) bp+=2;
	  }
      }
  }
}

template <class T>
static void getRowLoop(T* op, const T* ip, const ndx_t *ndx, ndx_t count, bool isComplex)
{
  if (!isComplex)
    for (int i=0;i<count;i++)
      op[i] = ip[ndx[i]];
  else
    for (int i=0;i<count;i++)
      {
	op[2*i] = ip[2*ndx[i]];
	op[2*i+1] = ip[2*ndx[i]+1];
      }
}

template <class T>
static void getLoop(T* op, const T* ip, const dim_t (&outdim)[MAXDIMS], const Tuple & adims, int ndims, const ndx_t * (&coords)[MAXDIMS], bool isComplex)
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
    if (!isComplex)
      {
	for (dim_t row=0;row<outdim[0];row++)
	  op[row] = ip[start + coords[0][row]];
	op += outdim[0];
      }
    else
      {
	for (dim_t row=0;row<outdim[0];row++)
	  {
	    op[2*row] = ip[2*(start + coords[0][row])];
	    op[2*row+1] = ip[2*(start + coords[0][row])+1];
	  }
	op += 2*outdim[0];
      }
  }
}

// FIXME - Handle A(:)
template <class T>
Object ArrayType<T>::getParensRowMode(const Object &a, const Tuple &dims, const Object &b)
{
  if (_ctxt->_index->isColon(b))
    {
      Object output = a;
      output.dims() = Tuple(a.elementCount(),1);
      return output;
    }
  Object output = this->zeroArrayOfSize(dims,a.isComplex());
  getRowLoop<T>(this->readWriteData(output),this->readOnlyData(a),
		_ctxt->_index->readOnlyData(b),b.elementCount(),a.isComplex());
  return output;
}

// TODO - handle (:,:,x) indexing!
static inline int isSliceIndexCase(ThreadContext *ctxt, Object *c, int cnt)
{
  int last_colon = 0;
  while (last_colon < cnt && ctxt->_index->isColon(c[last_colon])) last_colon++;
  if (last_colon == 0) return 0; // Need at least one colon
  for (int i=last_colon;i<cnt;i++)
    if (ctxt->_index->isColon(c[i]) || !c[i].isScalar()) return 0;
  return last_colon;
}

template <class T>
Object ArrayType<T>::getSliceMode(const Object &a, Object *c, int cnt, int last_colon)
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

template <class T>
Object ArrayType<T>::getParens(const Object &a, const Object &b) {
  // FIXME - b is empty?
  if (b.elementCount() == 1) 
    {
      Object ndx = _ctxt->_list->first(b);
      return getParensRowMode(a,ndx.dims(),ndx.asIndex(a.elementCount()));
    }
  const Tuple & adims(a.dims());
  // TODO: Trim trailing singular dimensions.  Make sure b.size() < a.numDims.
  // TODO: Special case all-scalar indexing case
  Object carray(_ctxt->_list->makeMatrix(MAXDIMS,1));
  Object *c = _ctxt->_list->readWriteData(carray);
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
    getLoop<T>(this->readWriteData(output),this->readOnlyData(a),outdim,adims,bsize,coords,a.isComplex());
  return output;
}

template <class T>
void ArrayType<T>::resize(Object &a, const Tuple &newsize) {
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
  T zero(this->zeroElement());
  if (a.d->capacity > newsize.elementCount()) { 
    moveLoop<T>(this->readWriteData(a),newsize,a.dims(),zero,a.isComplex());
  } else {
    // Copy
    Object na = this->zeroArrayOfSize(newsize,a.isComplex());
    copyLoop<T>(this->readWriteData(na),this->readOnlyData(a),newsize,a.dims(),a.isComplex());
    a = na;
  }
  a.dims() = newsize;
}

template <class T>
void setLoopRowMode(T* a, const ndx_t *addr, dim_t count, const T* b, bool bScalar, bool isComplex)
{
  if (!isComplex)
    {
      for (dim_t i=0;i<count;i++)
	{
	  a[addr[i]] = *b;
	  if (!bScalar) ++b;
	}
    }
  else
    {
      for (dim_t i=0;i<count;i++)
	{
	  a[2*addr[i]] = b[0];
	  a[2*addr[i]+1] = b[1];
	  if (!bScalar) b+=2;
	}
    }
}

template <class T>
void ArrayType<T>::setParensRowIndexMode(Object &a, const Object &ndx, const Object &b) {
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
  setLoopRowMode<T>(this->readWriteData(a),
		    addr,ndx.elementCount(),
		    this->readOnlyData(b),
		    b.isScalar(),
		    a.isComplex());
}

template <class T>
void ArrayType<T>::promoteComplex(Object &a) {
  if (a.isComplex()) return;
  Object o = zeroArrayOfSize(a.dims(),true);
  T *op = this->readWriteData(o);
  const T *ip = this->readOnlyData(a);
  for (dim_t i=0;i<a.elementCount();i++)
    op[2*i] = ip[i];
  a = o;
}

template <class T>
void ArrayType<T>::setParens(Object &a, const Object &args, const Object &b) {
  if (args.elementCount() == 1)
    {
      Object ndx = _ctxt->_list->first(args);
      return setParensRowIndexMode(a,ndx.asIndexNoBoundsCheck(),b);
    }
  Tuple & adims(a.dims());
  // TODO: Trim trailing singular dimensions.  Make sure b.size() < a.numDims.
  // TODO: Special case all-scalar indexing case
  Object carray = _ctxt->_list->makeMatrix(MAXDIMS,1);
  Object *c = _ctxt->_list->readWriteData(carray);
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
  setLoop<T>(this->readWriteData(a),
	     argdim,adims,argsize,coords,
	     this->readOnlyData(b),b.isScalar(),a.isComplex());
}

template <class T>
void ArrayType<T>::printSheet(const Object &a, const ArrayFormatInfo &format, ndx_t offset) {
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

template <class T>
void ArrayType<T>::print(const Object &a)
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
template <class T>
Object ArrayType<T>::NCat(const Object& p, int dim)
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
  T zero(this->zeroElement());
  while (outputOffset < outputCount)
    {
      if (!anyComplex)
	copyPage<T>(op,outputOffset,pointers[k],offsets[k],pagesize[k],false);
      else if (anyComplex && !promotionRequired[k])
	copyPage<T>(op,outputOffset,pointers[k],offsets[k],pagesize[k],true);
      else
	copyPagePromote<T>(op,outputOffset,pointers[k],offsets[k],pagesize[k],zero);
      k = (k + 1) % objectCount;
    }
  return retObject;
}


template class ArrayType<char>;
template class ArrayType<float>;
template class ArrayType<double>;
template class ArrayType<Object>;
template class ArrayType<int32_t>;
template class ArrayType<uint32_t>;
template class ArrayType<int64_t>;
template class ArrayType<uint64_t>;
template class ArrayType<bool>;

