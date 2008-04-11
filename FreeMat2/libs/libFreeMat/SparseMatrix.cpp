#include "SparseMatrix.hpp"
#include "BasicArray.hpp"

SparseMatrix::SparseMatrix(const BasicArray<double> &A) {
  ConstBasicIterator<double> source(&A,0);
  index_t col = 1;
  while (source.isValid()) {
    SparseSlice this_col;
    for (index_t row=1;row<=source.size();row++) {
      if (source.get() != 0)
	this_col[row] = source.get();
      source.next();
    }
    if (this_col.count() > 0)
      m_data[col] = this_col;
    ++col;
    source.nextSlice();
  }
  m_dims = A.dimensions();
}

SparseMatrix SparseMatrix::getSlice(const IndexArrayVector& index) const {
  SparseMatrix ret;
  ret.m_dims = NTuple(m_dims[0],1);
  ret.m_data[1] = m_data[index[1].get((index_t)1)];
  return ret;
}

void SparseMatrix::printMe(std::ostream& o) const {
  ConstSparseIterator source(this);
  while (source.isValid()) {
    NTuple pos(source.pos());
    o << pos[1] << "," << pos[0] << "," << source.value() << "\n";
    source.next();
  }
}

bool SparseMatrix::operator==(const SparseMatrix &other) const {
  ConstSparseIterator source(this);
  ConstSparseIterator dest(&other);
  while (source.isValid() && dest.isValid()) {
    if (!(source.pos() == dest.pos())) return false;
    if (source.value() != dest.value()) return false;
    source.next();
    dest.next();
  }
  return true;
}

void SparseMatrix::resize(const NTuple& pos) {
  SparseMatrix ret(pos);
  ConstSparseIterator source(this);
  while (source.isValid()) {
    ret.set(dimensions().map(source.pos()),source.value());
    source.next();
  }
  *this = ret;
}

void SparseMatrix::resize(index_t len) {
  if (len > length()) {
    NTuple newDim;
    if (isEmpty() || isScalar()) {
      newDim = NTuple(1,len);
    } else if (isVector()) {
      if (rows() != 1)
	newDim = NTuple(len,1);
      else 
	newDim = NTuple(1,len);
    } else {
      m_dims = NTuple(1,length());
      newDim = NTuple(1,len);
    }
    resize(newDim);
  }
}

void SparseMatrix::reshape(const NTuple& pos) {
  if (length() != pos.count()) 
    throw Exception("Illegal reshape");
  resize(pos);
}

void SparseMatrix::del(const IndexArray& index) {
  if (IsColonOp(index)) {
    *this = SparseMatrix();
    return;
  }
  QSet<uint64> delete_set;
  for (index_t i=1;i<=index.length();i++)
    delete_set.insert(uint64(index.get(i)));
  index_t newSize = length() - delete_set.count();
  if (newSize == length()) return;
  NTuple newDim;
  if (isRowVector())
    newDim = NTuple(1,newSize);
  else
    newDim = NTuple(newSize,1);
  SparseMatrix ret(newDim);
  ConstSparseIterator source(this);
  index_t deleted_count = 0;
  index_t dp = 1;
  while (source.isValid()) {
    index_t source_pos = m_dims.map(source.pos());
    if (!delete_set.contains(uint64(source_pos))) {
      // This element was not deleted.  
      while (dp < source_pos) {
	if (delete_set.contains(uint64(dp))) ++deleted_count;
	++dp;
      }
      ret.set(source_pos-deleted_count,source.value());
    }
    source.next();
  }
  *this = ret;
}

void SparseMatrix::del(const IndexArrayVector& index) {
  if (IsColonOp(index[0]) && IsColonOp(index[1])) {
    *this = SparseMatrix();
    return;
  }
  if (IsColonOp(index[0]))
    deleteColumns(index[1]);
  else if (IsColonOp(index[1]))
    deleteRows(index[0]);
  else
    throw Exception("Unsupported deletion for sparse matrices.");
}

void SparseMatrix::deleteColumns(const IndexArray& index) {
  QSet<uint64> delete_set;
  for (index_t i=1;i<=index.length();i++)
    delete_set.insert(uint64(index.get(i)));
  SparseData copy;
  index_t deleted_count = 0;
  for (index_t i=1;i<=cols();i++) {
    if (delete_set.contains(uint64(i))) deleted_count++;
    if (m_data.contains(i))
      copy[i-deleted_count] = m_data[i];
  }
  m_dims = NTuple(m_dims[0],m_dims[1] - delete_set.count());
  m_data = copy;
}

void SparseMatrix::deleteRows(const IndexArray& index) {
  QSet<uint64> delete_set;
  for (index_t i=1;i<=index.length();++i)
    delete_set.insert(uint64(index.get(i)));
  for (SparseData::iterator i=m_data.begin();i!=m_data.end();++i) {
    index_t deleted_count = 0;
    SparseSlice copy;
    index_t dp = 1;
    for (SparseSlice::iterator j=i.value().begin();j!=i.value().end();++j) {
      if (!delete_set.contains(uint64(j.key()))) {
	while (dp < j.key()) {
	  if (delete_set.contains(uint64(dp))) ++deleted_count;
	  ++dp;
	}
	copy[j.key()-deleted_count] = j.value();
      }
    }
    i.value() = copy;
  }
  m_dims = NTuple(m_dims[0] - delete_set.count(),m_dims[1]);
}

ConstSparseIterator::ConstSparseIterator(const SparseMatrix *ptr) {
  m_ptr = ptr;
  m_col = m_ptr->constData().constBegin();
  m_row = m_col.value().constBegin();
}


void ConstSparseIterator::next() {
  ++m_row;
  if (m_row == m_col.value().constEnd()) {
    ++m_col;
    if (m_col != m_ptr->constData().constEnd())
      m_row = m_col.value().constBegin();
  }
}

bool ConstSparseIterator::isValid() const {
  return (m_col != m_ptr->constData().constEnd());
}

double ConstSparseIterator::value() {
  return m_row.value();
}

const NTuple ConstSparseIterator::pos() const {
  return NTuple(m_row.key(),m_col.key());
}
