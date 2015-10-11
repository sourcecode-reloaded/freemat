#include "ArrayType.hpp"
#include "IntegerType.hpp"
#include "BoolType.hpp"
#include "ListType.hpp"
#include "ThreadContext.hpp"
#include "Complex.hpp"

using namespace FM;

static inline void IncrementColumnsAndRipple(ndx_t (&ndx) [MAXDIMS], const ndx_t (&limits) [MAXDIMS], ndx_t ndims)
{
  ndx[1]++;
  for (auto i=1;i<(ndims-1);i++)
    if (ndx_t(ndx[i]) >= limits[i])
      {
	ndx[i] = 0;
	ndx[i+1]++;
      }
}

static inline void DecrementColumnsAndRipple(ndx_t (&ndx) [MAXDIMS], const ndx_t (&limits) [MAXDIMS], ndx_t ndims)
{
  ndx[1]--;
  for (auto i=1;i<(ndims-1);i++)
    if (ndx[i] < 0)
      {
	ndx[i] = ndx_t(limits[i])-1;
	ndx[i+1]--;
      }
}

static inline Tuple ComputeRowResizedTuple(const Object &a, ndx_t maxndx)
{
  // This logic is taken from BasicArray::resize(index_t) in FM4
  if (a.isScalar() || a.isEmpty())
    return Tuple(1,maxndx);
  else if (a.isVector()) {
    if (a.rows() != 1)
      return Tuple(maxndx,1);
    else
      return Tuple(1,maxndx);
  } else {
    return Tuple(1,maxndx);
  }
}

template <class T>
static inline void copyPage(T* op, ndx_t &outputOffset, const T* ip, ndx_t &input_offset, ndx_t pagesize)
{
  for (ndx_t j=0;j<pagesize;j++)
    op[outputOffset+j] = ip[input_offset+j];
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
static inline void moveLoop(T* ap, const Tuple &outdims, const Tuple &adims, const T &zero)
{
  auto ndims = std::max(adims.dimensions(),outdims.dimensions());
  ndx_t adimraw[MAXDIMS];
  for (auto i=0;i<ndims;i++) adimraw[i] = adims.dimension(i);
  // Check for a no-op - if only the last dimension increases, 
  // it's a no-op
  bool allsame = true;
  for (auto i=0;i<ndims-1;i++)
    allsame = allsame && (adimraw[i] == outdims.dimension(i));
  if (allsame) return;
  ndx_t iterations = 1;
  for (auto i=1;i<ndims;i++)
    iterations *= adimraw[i];
  ndx_t ndx[MAXDIMS];
  ndx_t strides[MAXDIMS];
  strides[0] = 1;
  ndx[0] = adimraw[0]-1;
  for (auto i=1;i<ndims;i++)
    {
      strides[i] = strides[i-1]*outdims.dimension(i-1);
      ndx[i] = adimraw[i]-1;
    }
  ndx_t arows = adims.rows();
  ndx_t orows = outdims.rows();
  ndx_t aoffset = adims.count()-1;
  for (ndx_t iter=0;iter<iterations;iter++) {
    // Compute the start address
    ndx_t start = 0;
    for (auto i=0;i<ndims;i++)
      start += ndx[i]*strides[i];
    assert(start >= aoffset);
    if (aoffset != start)
      {
	for (ndx_t row=0;row<arows;row++)
	  {
	    ap[start-row] = ap[aoffset-row];
	    ap[aoffset-row] = zero;
	  }
      }
    aoffset -= arows;
    for (ndx_t row=0;row<orows-arows;row++)
      ap[start+1+row] = zero;
    DecrementColumnsAndRipple(ndx,adimraw,ndims);
  }
}

template <class T>
static inline void copyLoop(T* op, const T* ip, const Tuple & outdims, const Tuple & adims)
{
  // Compute the number of iterations required by the RESIZE
  // Excludes the first dimension. 
  auto ndims = std::max(adims.dimensions(),outdims.dimensions());
  ndx_t adimraw[MAXDIMS];
  for (auto i=0;i<ndims;i++) adimraw[i] = adims.dimension(i);
  ndx_t iterations = 1;
  for (auto i=1;i<ndims;i++)
    iterations *= adimraw[i];
  ndx_t ndx[MAXDIMS];
  ndx_t strides[MAXDIMS];
  strides[0] = 1;
  for (auto i=1;i<ndims;i++)
    {
      strides[i] = strides[i-1]*outdims.dimension(i-1);
      ndx[i] = 0;
    }
  // Loop over the iterations
  ndx_t arows = adims.rows();
  for (ndx_t iter=0;iter<iterations;iter++) {
    // Compute the start address
    ndx_t start = 0;
    for (auto i=1;i<ndims;i++)
      start += ndx[i]*strides[i];
    for (ndx_t row=0;row<arows;row++)
      op[row+start] = ip[row];
    ip += arows;
    IncrementColumnsAndRipple(ndx,adimraw,ndims);
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
static inline void setLoop(T* op, const ndx_t (&argdim) [MAXDIMS], const Tuple & adims, ndx_t ndims, const ndx_t *(&coords)[MAXDIMS], const T* bp, bool isScalar)
{
  ndx_t ndx[MAXDIMS];
  ndx_t strides[MAXDIMS];
  ndx[0] = 0;
  strides[0] = 1;
  // Compute the number of iterations required by the set
  // Excludes the first dimension.  Also computes the strides
  // and initializes the index array to all zeros
  ndx_t iterations = 1;
  for (auto i=1;i<ndims;i++) {
    iterations *= argdim[i];
    ndx[i] = 0;
    strides[i] = strides[i-1]*adims.dimension(i-1);
  }
  // Loop over the iterations
  for (ndx_t iter=0;iter<iterations;iter++) {
    // Compute the start address
    ndx_t start = 0;
    for (auto i=1;i<ndims;i++) 
      start += coords[i][ndx[i]]*strides[i];
    IncrementColumnsAndRipple(ndx,argdim,ndims);
    for (ndx_t row=0;row<argdim[0];row++)
      {
	op[start + coords[0][row]] = *bp;
	if (!isScalar) bp++;
      }
  }
}

template <class T>
static inline void getRowLoop(T* op, const T* ip, const ndx_t *ndx, ndx_t count)
{
  for (auto i=0;i<count;i++)
    op[i] = ip[ndx[i]];
}

template <class T>
static inline void getLoop(T* op, const T* ip, const ndx_t (&outdim)[MAXDIMS], const Tuple & adims, ndx_t ndims, const ndx_t * (&coords)[MAXDIMS])
{
  ndx_t ndx[MAXDIMS];
  ndx_t strides[MAXDIMS];
  ndx[0] = 0;
  strides[0] = 1;
  // Compute the number of iterations required by the GET
  // Excludes the first dimension.  Also computes the strides
  // and initializes the index array to all zeros
  ndx_t iterations = 1;
  for (auto i=1;i<ndims;i++) {
    iterations *= outdim[i];
    ndx[i] = 0;
    strides[i] = strides[i-1]*adims.dimension(i-1);
  }
  // Loop over the iterations
  for (ndx_t iter=0;iter<iterations;iter++) {
    // Compute the start address
    ndx_t start = 0;
    for (auto i=1;i<ndims;i++) 
      start += coords[i][ndx[i]]*strides[i];
    IncrementColumnsAndRipple(ndx,outdim,ndims);
    for (ndx_t row=0;row<outdim[0];row++)
      op[row] = ip[start + coords[0][row]];
    op += outdim[0];
  }
}

template <class T>
Object ArrayType<T>::getParensRowMode(const Object &a, const Tuple &dims, const Object &b)
{
  if (_ctxt->_index->isColon(b))
    {
      Object output = a;
      output.setDims(Tuple(a.count(),1));
      return output;
    }
  Object output = this->zeroArrayOfSize(dims);
  getRowLoop<T>(this->rw(output),this->ro(a),
		_ctxt->_index->ro(b),b.count());
  return output;
}

static inline ndx_t isSliceIndexCase(ThreadContext *ctxt, Object *c, ndx_t cnt)
{
  ndx_t last_colon = 0;
  while (last_colon < cnt && ctxt->_index->isColon(c[last_colon])) last_colon++;
  if (last_colon == 0) return 0; // Need at least one colon
  for (auto i=last_colon;i<cnt;i++)
    if (ctxt->_index->isColon(c[i]) || !c[i].isScalar()) return 0;
  return last_colon;
}

template <class T>
Object ArrayType<T>::getSliceMode(const Object &a, Object *c, ndx_t cnt, ndx_t last_colon)
{
  // Compute the index of the first element of the array
  ndx_t offset = 0;
  const Tuple& adims = a.dims();
  ndx_t stride = adims.dimension(0);
  Tuple slicedims;
  slicedims.setRows(adims.dimension(0));
  for (auto i=1;i<cnt;i++)
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
				 slicedims.count());
  q->refcnt = 0;
  return Object(q);
}

template <class T>
Object ArrayType<T>::getParens(const Object &a, const Object &b) {
  if (b.count() == 1) 
    {
      Object ndx = _ctxt->_list->first(b);
      return getParensRowMode(a,ndx.dims(),ndx.asIndex(a.count()));
    }
  const Tuple & adims(a.dims());
  // TODO: Special case all-scalar indexing case
  Object carray(_ctxt->_list->makeMatrix(MAXDIMS,1));
  Object *c = _ctxt->_list->rw(carray);
  auto bsize = b.count();
  const Object *bp = _ctxt->_list->ro(b);
  for (auto i=0;i<bsize;i++)
    c[i] = bp[i].asIndex(adims.dimension(i));
  auto last_colon = isSliceIndexCase(_ctxt,c,bsize);
  if (last_colon > 0) {
    return getSliceMode(a,c,bsize,last_colon);
  }
  // Expand colons, and do bounds check
  if (bsize > MAXDIMS) 
    throw Exception(FMString("FreeMat is compiled for maximum ") 
		    + Stringify(MAXDIMS) + FMString(" dimensions.  Indexing expression ")
		    + FMString(" requires too many dimensions."));
  ndx_t outdim[MAXDIMS];
  const ndx_t *coords[MAXDIMS];
  for (auto i=0;i<bsize;i++)
    {
      IndexType *ip = static_cast<IndexType*>(c[i].type());
      if (ip->isColon(c[i]))
	c[i] = ip->expandColons(adims.dimension(i));
      outdim[i] = c[i].count();
      coords[i] = ip->ro(c[i]);
    }
    Object output = this->zeroArrayOfSize(Tuple::RawTuple(outdim,bsize));
    getLoop<T>(this->rw(output),this->ro(a),outdim,adims,bsize,coords);
  return output;
}

template <class T>
void ArrayType<T>::resizeSlow(Object &a, const Tuple &newsize) {
  // Determine if this is a move or a copy
  T zero(this->zeroElement());
  if (a.d->capacity > newsize.count()) { 
    moveLoop<T>(this->rw(a),newsize,a.dims(),zero);
  } else {
    // Copy
    Object na = this->zeroArrayOfSize(newsize);
    copyLoop<T>(this->rw(na),this->ro(a),newsize,a.dims());
    a = na;
  }
  a.setDims(newsize);
}

template <class T>
void setLoopRowMode(T* a, const ndx_t *addr, ndx_t count, const T* b, bool bScalar)
{
  for (ndx_t i=0;i<count;i++)
    {
      a[addr[i]] = *b;
      if (!bScalar) ++b;
    }
}

template <class T>
void ArrayType<T>::setParensRowIndexMode(Object &a, const Object &ndx, const Object &b) {
  // First check for resize
  ndx_t maxndx = _ctxt->_index->maxValue(ndx)+1;
  if (maxndx > a.count())
    {
      a.type()->resize(a,ComputeRowResizedTuple(a,maxndx));
    }
  const ndx_t *addr = _ctxt->_index->ro(ndx);
  // FIXME
  //  if (!a.isComplex() && b.isComplex())
  //    this->promoteComplex(a);
  setLoopRowMode<T>(this->rw(a),
		    addr,ndx.count(),
		    this->ro(b),
		    b.isScalar());
}

// FIXME
// template <class T>
// void ArrayType<T>::promoteComplex(Object &a) {
//   if (a.isComplex()) return;
//   Object o = zeroArrayOfSize(a.dims(),true);
//   T *op = this->rw(o);
//   const T *ip = this->ro(a);
//   for (ndx_t i=0;i<a.count();i++)
//     op[2*i] = ip[i];
//   a = o;
// }

template <class T>
Object ArrayType<T>::convertArgumentsToIndexingExpressions(const Object &args) {
  Object carray = _ctxt->_list->makeMatrix(MAXDIMS,1);
  Object *c = _ctxt->_list->rw(carray);
  ndx_t argsize = args.count();
  const Object *argsp = args.asType<ListType>()->ro(args);
  for (ndx_t i=0;i<argsize;i++)
    c[i] = argsp[i].asIndexNoBoundsCheck();
  if (argsize > MAXDIMS) 
    throw Exception(FMString("FreeMat is compiled for maximum ") 
		    + Stringify(MAXDIMS) + FMString(" dimensions.  Indexing expression ")
		    + FMString(" requires too many dimensions."));
  return carray;
}

// Computes the deletion map from a set of index objects
static inline Object ComputeDeletionMap(ThreadContext *_ctxt, ndx_t length_in_deletion_dim, const Object &deletion_index) {
  // A deletion of the planar type means we should be able to implement
  // the deletion with a move.
  // Create a bit mask for the given dimension
  Object mask(_ctxt->_bool->zeroArrayOfSize(Tuple(length_in_deletion_dim,1)));
  bool *mp = _ctxt->_bool->rw(mask);
  // Loop over the non-colon dimension and fill in the columns to be deleted
  const ndx_t *dp = _ctxt->_index->ro(deletion_index);
  ndx_t dp_len = deletion_index.count();
  for (ndx_t i=0;i<dp_len;i++)
    {
      if (dp[i] > ndx_t(length_in_deletion_dim)) throw Exception("Out of range - in deletion A(..,x,..) = [] x exceeds size of array A");
      mp[dp[i]] = true;
    }
  return mask;
}

template <class T>
static inline void deleteRowsLoop(T* ap, const bool *mp, ndx_t len, ndx_t srcptr, ndx_t dstptr)
{
  for (ndx_t i=0;i<len;i++)
    {
      if (!mp[i]) 
	{
	  ap[dstptr] = ap[srcptr];
	  dstptr++;
	}
      srcptr++;
    }
}

template <class T>
void ArrayType<T>::erasePlanes(Object &a, const Object &mask, const Tuple &outdims, ndx_t non_colon_index)
{
  // Compute the page size - product of dimensions up to (but not including
  // non-colon-index).
  ndx_t page_size = 1;
  for (auto i=0;i<non_colon_index;i++)
    page_size *= outdims.dimension(i);
  // Compute the iteration count - the number of times we have to apply 
  // the deletion
  ndx_t iteration_count = 1;
  for (auto i=non_colon_index+1;i<outdims.dimensions();i++) 
    iteration_count *= outdims.dimension(i);
  ndx_t scan_dim = a.dims().dimension(non_colon_index);
  const bool *mp = _ctxt->_bool->ro(mask);
  T *ap = this->rw(a);
  ndx_t srcptr = 0;
  ndx_t dstptr = 0;
  for (ndx_t iteration = 0;iteration < iteration_count;iteration++)
    {
      for (ndx_t scan = 0;scan < scan_dim;scan++)
	if (!mp[scan])
	  copyPage(ap,dstptr,ap,srcptr,page_size);
	else
	  srcptr += page_size;
    }
}

// Deletion of the form A(b) = []
template <class T>
void ArrayType<T>::eraseRowIndexMode(Object &a, const Object &ndx)
{
  // First compute the deletion map
  Object deletion_map(ComputeDeletionMap(_ctxt,a.count(),ndx));
  const bool *bp = _ctxt->_bool->ro(deletion_map);
  // Determine the output dim
  ndx_t outputLength = _ctxt->_bool->countZero(deletion_map);
  T* ap = this->rw(a);
  ndx_t adst = 0;
  for (ndx_t i=0;i<a.count();i++)
    if (!bp[i]) ap[adst++] = ap[i];
  a.setDims(ComputeRowResizedTuple(a,outputLength));
}


// Deletion is in the row dimension
template <class T>
void ArrayType<T>::eraseRows(Object &a, const Object &mask, const Tuple &outdims)
{
  const bool *mp = _ctxt->_bool->ro(mask);
  // Calculate the number of iterations
  const Tuple &adims = a.dims();
  ndx_t iterations = 1;
  for (auto i=1;i<adims.dimensions();i++)
    iterations *= adims.dimension(i);
  ndx_t srcptr = 0;
  ndx_t dstptr = 0;
  for (ndx_t iter=0;iter < iterations;iter++)
    {
      deleteRowsLoop(this->rw(a),mp,adims.rows(),srcptr,dstptr);
      srcptr += adims.rows();
      dstptr += outdims.rows();
    }
}


template <class T>
void ArrayType<T>::erase(Object &a, const Object &args) {
  Object carray(this->convertArgumentsToIndexingExpressions(args));
  Object *c = _ctxt->_list->rw(carray);
  // Check to see if all but one dimension are covered with colons
  ndx_t num_colons = 0;
  ndx_t non_colon_index = -1;
  for (auto i=0;i<args.count();i++)
    {
      if (_ctxt->_index->isColon(c[i])) 
	num_colons++;
      else
	non_colon_index = i;
    }
  if (num_colons < (a.dims().dimensions() - 1)) throw Exception("In expression A(:,...,:,x) = [], only one non-colon index is allowed");
  if ((num_colons >= a.dims().dimensions()) && (non_colon_index == -1))
    {
      a = this->empty();
      return;
    }
  Object deletion_mask = ComputeDeletionMap(_ctxt,a.dims().dimension(non_colon_index),c[non_colon_index]);
  Tuple outdims(a.dims());
  outdims.set(non_colon_index,_ctxt->_bool->countZero(deletion_mask));
  if (non_colon_index == 0)
    eraseRows(a,deletion_mask,outdims);
  else
    erasePlanes(a,deletion_mask,outdims,non_colon_index);
  a.setDims(outdims);
  return;  
}

template <class T>
void ArrayType<T>::setParens(Object &a, const Object &args, const Object &b) {
  if (args.count() == 1)
    {
      Object ndx = _ctxt->_list->first(args);
      if (b.isEmpty())
	return eraseRowIndexMode(a,ndx.asIndexNoBoundsCheck());
      return setParensRowIndexMode(a,ndx.asIndexNoBoundsCheck(),b);
    }
  if (b.isEmpty()) return erase(a,args);
  Tuple adims(a.dims());
  // TODO: Special case all-scalar indexing case
  Object carray(this->convertArgumentsToIndexingExpressions(args));
  auto argsize = args.count();
  Object *c = _ctxt->_list->rw(carray);
  ndx_t argdim[MAXDIMS];
  ndx_t outdim[MAXDIMS];
  const ndx_t *coords[MAXDIMS];
  bool resize_required = false;
  ndx_t outcount = 1;
  for (auto i=0;i<argsize;i++)
    {
      IndexType *ip = static_cast<IndexType*>(c[i].type());
      if (ip->isColon(c[i]))
	c[i] = ip->expandColons(adims.dimension(i));
      argdim[i] = c[i].count();
      coords[i] = ip->ro(c[i]);
      outdim[i] = std::max<ndx_t>(adims.dimension(i),ip->maxValue(c[i])+1);
      outcount *= argdim[i];
      resize_required |= (outdim[i] > adims.dimension(i));
    }
  // Check if the output requires resizing of the array
  if (resize_required)
    {
      a.type()->resize(a,Tuple::RawTuple(outdim,argsize));
      adims = a.dims();
    }      
  // Check for validity of b
  if (!(b.isScalar() || b.count() == outcount))
    throw Exception("Assignment A(...) = b requires b either be a scalar or have the correct number of elements");
  // FIXME
  // if (!a.isComplex() && b.isComplex())
  //   this->promoteComplex(a);
  setLoop<T>(this->rw(a),
	     argdim,adims,argsize,coords,
	     this->ro(b),b.isScalar());
}

const int print_pad = 2;

template <class T>
void ArrayType<T>::printSheet(const Object &a, const ArrayFormatInfo &format, ndx_t offset) {
  ndx_t columns = a.cols();
  ndx_t rows = a.rows();
  ndx_t elwidth = format.total_width;
  if (format.needs_space) elwidth += print_pad; 
  int colsPerPage = (int) floor((_ctxt->_io->getTerminalWidth()-1-print_pad)/((double) elwidth));
  colsPerPage = (colsPerPage < 1) ? 1 : colsPerPage;
  int pageCount = (int) ceil(columns/((double)colsPerPage));
  for (auto k=0;k<pageCount;k++) {
    ndx_t colsInThisPage;
    colsInThisPage = columns - colsPerPage*k;
    colsInThisPage = (colsInThisPage > colsPerPage) ? 
      colsPerPage : colsInThisPage;
    if ((rows*columns > 1) && (pageCount > 1))
      _ctxt->_io->output("\n Columns %d to %d\n\n",k*colsPerPage+1,k*colsPerPage+colsInThisPage);
    _ctxt->_io->output(FMString(print_pad,' '));
    for (auto i=0;i<rows;i++) {
      for (auto j=0;j<colsInThisPage;j++) {
	this->printElement(a,format,i+(k*colsPerPage+j)*rows+offset);
	if (format.needs_space && (j != (colsInThisPage-1))) _ctxt->_io->output(FMString(print_pad,' '));
      }
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
  _ctxt->_io->output("Format width = %d\n",format.total_width);
  _ctxt->_io->output("Format number_width = %d\n",format.number_width);
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
    ndx_t offset = 0;
    ndx_t page = a.rows()*a.cols();
    ndx_t page_count = a.count()/page;
    for (ndx_t p=0;p<page_count;p++)
      {
	Tuple w = offset % a.dims();
	_ctxt->_io->output("(:,:");
	for (auto m=2;m<w.dimensions();m++) 
	  _ctxt->_io->output(FMString(",") + Stringify(w.dimension(m)+1));
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
Object ArrayType<T>::NCat(const Object& p, ndx_t dim)
{
  ndx_t objectCount = p.count();
  const Object *dp = _ctxt->_list->ro(p);
  if (objectCount == 0) return this->empty();
  // Compute the size of the output
  Tuple outputSize = dp[0].dims();
  // Sum the components along the given dimension
  ndx_t aggregated_size = 0;
  for (auto i=0;i<objectCount;i++)
    aggregated_size += dp[i].dims().dimension(dim);
  outputSize.set(dim,aggregated_size);
  // Cache the pointers to the objects
  std::vector<const T*> pointers;
  pointers.resize(size_t(objectCount));
  // Cache the current indices into each object
  std::vector<ndx_t> offsets;
  offsets.resize(size_t(objectCount));
  // Cache the page size for each object
  std::vector<ndx_t> pagesize;
  pagesize.resize(size_t(objectCount));
  // FIXME - Promotion logic moves up a level
  /*
  // Cache the promotion flag for each object
  std::vector<bool> promotionRequired;
  promotionRequired.resize(size_t(objectCount));
  // Determine if promotion to complex is required
  // Required if any of the objects is complex, but
  // not all of them.
  bool anyComplex = false;
  bool allComplex = true;
  for (ndx_t i=0;i<objectCount;i++)
    {
      anyComplex |= dp[i].isComplex();
      allComplex &= dp[i].isComplex();
    }
  */
  for (ndx_t i=0;i<objectCount;i++)
    {
      pointers[unsigned(i)] = this->ro(dp[i]);
      offsets[unsigned(i)] = 0;
      ndx_t pagesze = 1;
      for (auto j=0;j<=dim;j++)
	pagesze *= dp[i].dims().dimension(j);
      pagesize[unsigned(i)] = pagesze;
      //      promotionRequired[unsigned(i)] = anyComplex && (!allComplex) && (!dp[i].isComplex());
    }
  // Allocate the output object
  Object retObject = this->zeroArrayOfSize(outputSize);
  ndx_t outputOffset = 0;
  ndx_t outputCount = outputSize.count();
  unsigned k = 0;
  T* op = this->rw(retObject);
  while (outputOffset < outputCount)
    {
      copyPage<T>(op,outputOffset,pointers[k],offsets[k],pagesize[k]);
      k = (k + 1) % objectCount;
    }
  return retObject;
}


template class FM::ArrayType<char>;
template class FM::ArrayType<float>;
template class FM::ArrayType<Complex<float> >;
template class FM::ArrayType<double>;
template class FM::ArrayType<Complex<double> >;
template class FM::ArrayType<Object>;
template class FM::ArrayType<int8_t>;
template class FM::ArrayType<Complex<int8_t> >;
template class FM::ArrayType<uint8_t>;
template class FM::ArrayType<Complex<uint8_t> >;
template class FM::ArrayType<int16_t>;
template class FM::ArrayType<Complex<int16_t> >;
template class FM::ArrayType<uint16_t>;
template class FM::ArrayType<Complex<uint16_t> >;
template class FM::ArrayType<int32_t>;
template class FM::ArrayType<Complex<int32_t> >;
template class FM::ArrayType<uint32_t>;
template class FM::ArrayType<Complex<uint32_t> >;
template class FM::ArrayType<int64_t>;
template class FM::ArrayType<Complex<int64_t> >;
template class FM::ArrayType<uint64_t>;
template class FM::ArrayType<Complex<uint64_t> >;
template class FM::ArrayType<bool>;

