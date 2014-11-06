#include "ArrayType.hpp"
#include "BaseTypes.hpp"
#include "IntegerType.hpp"
#include "BoolType.hpp"
#include "ListType.hpp"
#include "ThreadContext.hpp"

using namespace FM;

static inline void IncrementColumnsAndRipple(ndx_t (&ndx) [MAXDIMS], const dim_t (&limits) [MAXDIMS], int ndims)
{
  ndx[1]++;
  for (int i=1;i<(ndims-1);i++)
    if (ndx[i] >= limits[i])
      {
	ndx[i] = 0;
	ndx[i+1]++;
      }
}

static inline void DecrementColumnsAndRipple(ndx_t (&ndx) [MAXDIMS], const dim_t (&limits) [MAXDIMS], int ndims)
{
  ndx[1]--;
  for (int i=1;i<(ndims-1);i++)
    if (ndx[i] < 0)
      {
	ndx[i] = limits[i]-1;
	ndx[i+1]--;
      }
}

static inline Tuple ComputeRowResizedTuple(const Object &a, dim_t maxndx)
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
static inline void copyPage(T* op, dim_t &outputOffset, const T* ip, dim_t &input_offset, dim_t pagesize, bool isComplex)
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
static inline void copyPagePromote(T* op, dim_t outputOffset, const T* ip, dim_t &input_offset, dim_t pagesize, const T& zero)
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
static inline void moveLoop(T* ap, const Tuple &outdims, const Tuple &adims, const T &zero, bool isComplex)
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
    DecrementColumnsAndRipple(ndx,adimraw,ndims);
  }
}

template <class T>
static inline void copyLoop(T* op, const T* ip, const Tuple & outdims, const Tuple & adims, bool isComplex)
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
static inline void setLoop(T* op, const dim_t (&argdim) [MAXDIMS], const Tuple & adims, int ndims, const ndx_t *(&coords)[MAXDIMS], const T* bp, bool isScalar,
		    bool isComplex)
{
  ndx_t ndx[MAXDIMS];
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
    for (int i=1;i<ndims;i++) 
      start += coords[i][ndx[i]]*strides[i];
    IncrementColumnsAndRipple(ndx,argdim,ndims);
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
static inline void getRowLoop(T* op, const T* ip, const ndx_t *ndx, ndx_t count, bool isComplex)
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
static inline void getLoop(T* op, const T* ip, const dim_t (&outdim)[MAXDIMS], const Tuple & adims, int ndims, const ndx_t * (&coords)[MAXDIMS], bool isComplex)
{
  ndx_t ndx[MAXDIMS];
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
    for (int i=1;i<ndims;i++) 
      start += coords[i][ndx[i]]*strides[i];
    IncrementColumnsAndRipple(ndx,outdim,ndims);
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
  getRowLoop<T>(this->rw(output),this->ro(a),
		_ctxt->_index->ro(b),b.elementCount(),a.isComplex());
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
  Object *c = _ctxt->_list->rw(carray);
  int bsize = b.elementCount();
  const Object *bp = _ctxt->_list->ro(b);
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
      coords[i] = ip->ro(c[i]);
    }
    Object output = this->zeroArrayOfSize(Tuple::RawTuple(outdim,bsize),a.isComplex());
    getLoop<T>(this->rw(output),this->ro(a),outdim,adims,bsize,coords,a.isComplex());
  return output;
}

template <class T>
void ArrayType<T>::resizeSlow(Object &a, const Tuple &newsize) {
  // Determine if this is a move or a copy
  T zero(this->zeroElement());
  if (a.d->capacity > newsize.elementCount()) { 
    moveLoop<T>(this->rw(a),newsize,a.dims(),zero,a.isComplex());
  } else {
    // Copy
    Object na = this->zeroArrayOfSize(newsize,a.isComplex());
    copyLoop<T>(this->rw(na),this->ro(a),newsize,a.dims(),a.isComplex());
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
      a.type()->resize(a,ComputeRowResizedTuple(a,maxndx));
    }
  const ndx_t *addr = _ctxt->_index->ro(ndx);
  // TODO - handle b is complex, a is real
  if (!a.isComplex() && b.isComplex())
    this->promoteComplex(a);
  setLoopRowMode<T>(this->rw(a),
		    addr,ndx.elementCount(),
		    this->ro(b),
		    b.isScalar(),
		    a.isComplex());
}

template <class T>
void ArrayType<T>::promoteComplex(Object &a) {
  if (a.isComplex()) return;
  Object o = zeroArrayOfSize(a.dims(),true);
  T *op = this->rw(o);
  const T *ip = this->ro(a);
  for (dim_t i=0;i<a.elementCount();i++)
    op[2*i] = ip[i];
  a = o;
}

template <class T>
Object ArrayType<T>::convertArgumentsToIndexingExpressions(const Object &args) {
  Object carray = _ctxt->_list->makeMatrix(MAXDIMS,1);
  Object *c = _ctxt->_list->rw(carray);
  int argsize = args.elementCount();
  const Object *argsp = args.asType<ListType>()->ro(args);
  for (int i=0;i<argsize;i++)
    c[i] = argsp[i].asIndexNoBoundsCheck();
  // TODO: Add slice test here (c.f. isSliceIndexCase)
  // Expand colons, and do bounds check
  if (argsize > MAXDIMS) 
    throw Exception(FMString("FreeMat is compiled for maximum ") 
		    + Stringify(MAXDIMS) + FMString(" dimensions.  Indexing expression ")
		    + FMString(" requires too many dimensions."));
  return carray;
}

// Computes the deletion map from a set of index objects
static inline Object ComputeDeletionMap(ThreadContext *_ctxt, dim_t length_in_deletion_dim, const Object &deletion_index) {
  // A deletion of the planar type means we should be able to implement
  // the deletion with a move.
  // Create a bit mask for the given dimension
  Object mask(_ctxt->_bool->zeroArrayOfSize(Tuple(length_in_deletion_dim,1),false));
  bool *mp = _ctxt->_bool->rw(mask);
  // Loop over the non-colon dimension and fill in the columns to be deleted
  const ndx_t *dp = _ctxt->_index->ro(deletion_index);
  dim_t dp_len = deletion_index.elementCount();
  for (dim_t i=0;i<dp_len;i++)
    {
      if (dp[i] > length_in_deletion_dim) throw Exception("Out of range - in deletion A(..,x,..) = [] x exceeds size of array A");
      mp[dp[i]] = true;
    }
  return mask;
}

template <class T>
static inline void deleteRowsLoop(T* ap, const bool *mp, dim_t len, dim_t srcptr, dim_t dstptr, bool isComplex)
{
  if (!isComplex)
    for (dim_t i=0;i<len;i++)
      {
	if (!mp[i]) 
	  {
	    ap[dstptr] = ap[srcptr];
	    dstptr++;
	  }
	srcptr++;
      }
  else
    for (dim_t i=0;i<len;i++)
      {
	if (!mp[i]) 
	  {
	    ap[2*dstptr] = ap[2*srcptr];
	    ap[2*dstptr+1] = ap[2*srcptr+1];
	    dstptr++;
	  }
	srcptr++;
      }    
}

template <class T>
void ArrayType<T>::erasePlanes(Object &a, const Object &mask, const Tuple &outdims, int non_colon_index)
{
  // Compute the page size - product of dimensions up to (but not including
  // non-colon-index).
  dim_t page_size = 1;
  for (int i=0;i<non_colon_index;i++)
    page_size *= outdims.dimension(i);
  // Compute the iteration count - the number of times we have to apply 
  // the deletion
  dim_t iteration_count = 1;
  for (int i=non_colon_index+1;i<outdims.dimensions();i++) 
    iteration_count *= outdims.dimension(i);
  dim_t scan_dim = a.dims().dimension(non_colon_index);
  const bool *mp = _ctxt->_bool->ro(mask);
  T *ap = this->rw(a);
  dim_t srcptr = 0;
  dim_t dstptr = 0;
  for (dim_t iteration = 0;iteration < iteration_count;iteration++)
    {
      for (dim_t scan = 0;scan < scan_dim;scan++)
	if (!mp[scan])
	  copyPage(ap,dstptr,ap,srcptr,page_size,a.isComplex());
	else
	  srcptr += page_size;
    }
}

// Deletion of the form A(b) = []
template <class T>
void ArrayType<T>::eraseRowIndexMode(Object &a, const Object &ndx)
{
  // First compute the deletion map
  Object deletion_map(ComputeDeletionMap(_ctxt,a.elementCount(),ndx));
  const bool *bp = _ctxt->_bool->ro(deletion_map);
  // Determine the output dim
  dim_t outputLength = _ctxt->_bool->countZero(deletion_map);
  T* ap = this->rw(a);
  dim_t adst = 0;
  if (!a.isComplex())
    {
      for (dim_t i=0;i<a.elementCount();i++)
	if (!bp[i]) ap[adst++] = ap[i];
    } 
  else
    {
      for (dim_t i=0;i<a.elementCount();i++)
	if (!bp[i]) {
	  ap[2*adst] = ap[2*i];
	  ap[2*adst+1] = ap[2*i+1];
	  adst++;
	}
    }
  a.dims() = ComputeRowResizedTuple(a,outputLength);
}


// Deletion is in the row dimension
template <class T>
void ArrayType<T>::eraseRows(Object &a, const Object &mask, const Tuple &outdims)
{
  const bool *mp = _ctxt->_bool->ro(mask);
  // Calculate the number of iterations
  const Tuple &adims = a.dims();
  dim_t iterations = 1;
  for (int i=1;i<adims.dimensions();i++)
    iterations *= adims.dimension(i);
  dim_t srcptr = 0;
  dim_t dstptr = 0;
  for (dim_t iter=0;iter < iterations;iter++)
    {
      deleteRowsLoop(this->rw(a),mp,adims.rows(),srcptr,dstptr,a.isComplex());
      srcptr += adims.rows();
      dstptr += outdims.rows();
    }
}


template <class T>
void ArrayType<T>::erase(Object &a, const Object &args) {
  Object carray(this->convertArgumentsToIndexingExpressions(args));
  Object *c = _ctxt->_list->rw(carray);
  // Check to see if all but one dimension are covered with colons
  dim_t num_colons = 0;
  int non_colon_index = -1;
  for (int i=0;i<args.elementCount();i++)
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
  a.dims() = outdims;
  return;  
}

template <class T>
void ArrayType<T>::setParens(Object &a, const Object &args, const Object &b) {
  if (args.elementCount() == 1)
    {
      Object ndx = _ctxt->_list->first(args);
      if (b.isEmpty())
	return eraseRowIndexMode(a,ndx.asIndexNoBoundsCheck());
      return setParensRowIndexMode(a,ndx.asIndexNoBoundsCheck(),b);
    }
  if (b.isEmpty()) return erase(a,args);
  Tuple & adims(a.dims());
  // TODO: Trim trailing singular dimensions.  Make sure b.size() < a.numDims.
  // TODO: Special case all-scalar indexing case
  Object carray(this->convertArgumentsToIndexingExpressions(args));
  int argsize = args.elementCount();
  Object *c = _ctxt->_list->rw(carray);
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
      coords[i] = ip->ro(c[i]);
      outdim[i] = std::max<dim_t>(adims.dimension(i),ip->maxValue(c[i])+1);
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
  if (!(b.isScalar() || b.elementCount() == outcount))
    throw Exception("Assignment A(...) = b requires b either be a scalar or have the correct number of elements");
  if (!a.isComplex() && b.isComplex())
    this->promoteComplex(a);
  setLoop<T>(this->rw(a),
	     argdim,adims,argsize,coords,
	     this->ro(b),b.isScalar(),a.isComplex());
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
Object ArrayType<T>::NCat(const Object& p, int dim)
{
  int objectCount = p.elementCount();
  const Object *dp = _ctxt->_list->ro(p);
  if (objectCount == 0) return this->empty();
  // Compute the size of the output
  Tuple outputSize = dp[0].dims();
  // Sum the components along the given dimension
  dim_t aggregated_size = 0;
  for (int i=0;i<objectCount;i++)
    aggregated_size += dp[i].dims().dimension(dim);
  outputSize.set(dim,aggregated_size);
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
      pointers[i] = this->ro(dp[i]);
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
  T* op = this->rw(retObject);
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

