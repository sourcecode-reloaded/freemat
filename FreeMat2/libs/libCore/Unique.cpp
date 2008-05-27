#include "Array.hpp"
#include <QtCore>

//!
//@Module UNIQUE Unique
//@@Section ARRAY
//@@Usage
//Returns a vector containing the unique elements of an array.  The first
//form is simply
//@[
//   y = unique(x)
//@]
//where @|x| is either a numerical array or a cell-array of strings.  The 
//result is sorted in increasing order.  You can also retrieve two sets
//of index vectors
//@[
//   [y, m, n] = unique(x)
//@]
//such that @|y = x(m)| and @|x = y(n)|.  If the argument @|x| is a matrix,
//you can also indicate that FreeMat should look for unique rows in the
//matrix via
//@[
//   y = unique(x,'rows')
//@]
//and
//@[
//   [y, m, n] = unique(x,'rows')
//@]
//@@Example
//Here is an example in row mode
//@<
//A = randi(1,3*ones(15,3))
//unique(A,'rows')
//[b,m,n] = unique(A,'rows');
//b
//A(m,:)
//b(n,:)
//@>
//Here is an example in vector mode
//@<
//A = randi(1,5*ones(10,1))
//unique(A)
//[b,m,n] = unique(A,'rows');
//b
//A(m)
//b(n)
//@>
//For cell arrays of strings.
//@<
//A = {'hi','bye','good','tell','hi','bye'}
//unique(A)
//@>
//!
template <class T>
class UniqueEntryReal {
public:
  BasicArray<T> real;
};

template <class T>
bool operator<(const UniqueEntryReal<T> &A, const UniqueEntryReal<T> &B) {
  for (index_t i=1;i<=A.real.length();i++) {
    if (B.real[i] < A.real[i]) return false;
    if (A.real[i] < B.real[i]) return true;
  }
  return false;
}

template <>
bool operator<(const UniqueEntryReal<Array> &A, const UniqueEntryReal<Array> &B) {
  for (index_t i=1;i<=A.real.length();i++) {
    if (B.real[i].asString() < A.real[i].asString()) return false;
    if (A.real[i].asString() < B.real[i].asString()) return true;
  }
  return false;
}

template <class T>
void UniqueFunctionReal(const BasicArray<T> &data, BasicArray<T> &u_data, 
			BasicArray<index_t> &m_array, BasicArray<index_t> &n_array) {
  QMap<UniqueEntryReal,index_t> qSet;
  for (index_t i=1;i<=data.rows();i++) {
    UniqueEntryReal<T> tmp;
    tmp.real = BasicArray<T>(NTuple(1,data.cols()));
    for (index_t j=1;j<=data.cols();j++)
      tmp.real[j] = data.get(NTuple(i,j));
    qSet.insertMulti(tmp,i);
  }
  QList<UniqueEntryReal> qKeys(qSet.uniqueKeys());
  u_data = BasicArray<T>(NTuple(qKeys.size(),data.cols()));
  m_array = BasicArray<index_t>(NTuple(qKeys.size(),1));
  n_array = BasicArray<index_t>(NTuple(data.rows(),1));
  for (int i=0;i<qKeys.size();i++) {
    QList<index_t> qValues(qSet.values(qKeys[i]));
    for (index_t j=1;j<=data.cols();j++)
      u_data.set(NTuple(i+1,j),qKeys[i].real[j]);
    // To add support for "least" "most" modes of occurance,
    // need to control this line:
    m_array.set(i+1,qValues.front());
    for (int k=0;k<qValues.size();k++)
      n_array.set(qValues[k],i+1);
  }
}

template <class T>
class UniqueEntryComplex {
public:
  BasicArray<T> real;
  BasicArray<T> imag;
};

template <class T>
bool operator<(const UniqueEntryComplex<T> &A, const UniqueEntryReal<T> &B) {
  for (index_t i=1;i<=A.real.length();i++) {
    if (complex_lt(B.real[i],B.imag[i],A.real[i],A.imag[i])) return false;
    if (complex_lt(A.real[i],A.imag[i],B.real[i],B.imag[i])) return true;
  }
  return false;
}

template <class T>
void UniqueFunctionComplex(const BasicArray<T> &data_real, 
			   const BasicArray<T> &data_imag,
			   BasicArray<T> &u_data_real, 
			   BasicArray<T> &u_data_imag,
			   BasicArray<index_t> &m_array, 
			   BasicArray<index_t> &n_array) {
  QMap<UniqueEntryComplex,index_t> qSet;
  for (index_t i=1;i<=data_real.rows();i++) {
    UniqueEntryComplex<T> tmp;
    tmp.real = BasicArray<T>(NTuple(1,data_real.cols()));
    tmp.imag = BasicArray<T>(NTuple(1,data_real.cols()));
    for (index_t j=1;j<=data.cols();j++) {
      tmp.real[j] = data_real.get(NTuple(i,j));
      tmp.imag[j] = data_imag.get(NTuple(i,j));
    }
    qSet.insertMulti(tmp,i);
  }
  QList<UniqueEntryComplex> qKeys(qSet.uniqueKeys());
  u_data_real = BasicArray<T>(NTuple(qKeys.size(),data.cols()));
  u_data_imag = BasicArray<T>(NTuple(qKeys.size(),data.cols()));
  m_array = BasicArray<index_t>(NTuple(qKeys.size(),1));
  n_array = BasicArray<index_t>(NTuple(data.rows(),1));
  for (int i=0;i<qKeys.size();i++) {
    QList<index_t> qValues(qSet.values(qKeys[i]));
    for (index_t j=1;j<=data.cols();j++) {
      u_data_real.set(NTuple(i+1,j),qKeys[i].real[j]);
      u_data_imag.set(NTuple(i+1,j),qKeys[i].imag[j]);
    }
    // To add support for "least" "most" modes of occurance,
    // need to control this line:
    m_array.set(i+1,qValues.front());
    for (int k=0;k<qValues.size();k++)
      n_array.set(qValues[k],i+1);
  }
}

template <typename T>
static inline ArrayVector UniqueFunctionAux(const Array &arg) {
  if (arg.allReal()) {
    BasicArray<T> u_data;
    BasicArray<index_t> m_array;
    BasicArray<index_t> n_array;
    UniqueFunctionReal(arg.constReal<T>(),u_data,m_array,n_array);
    ArrayVector ret;
    ret.push_back(Array(u_data));
    ret.push_back(Array(m_array));
    ret.push_back(Array(n_array));
    return ret;
  } else {
    BasicArray<T> u_data_real;
    BasicArray<T> u_data_imag;
    BasicArray<index_t> m_array;
    BasicArray<index_t> n_array;
    UniqueFunctionReal(arg.constReal<T>(),
		       arg.constImag<T>(),
		       u_data,m_array,n_array);
    ArrayVector ret;
    ret.push_back(Array(u_data_real,u_data_imag));
    ret.push_back(Array(m_array));
    ret.push_back(Array(n_array));
    return ret;    
  }
}

#define MacroUnique(ctype,cls) \
  case cls: return UniqueFunctionAux<ctype>(arg);

static inline ArrayVector UniqueFunctionAux(const Array &arg) {
  if (arg.dataClass() == CellArray)
    if (!IsCellStringArray(arg)) 
      throw Exception("cell array arguments to 'unique' must be all strings");
  switch (arg.dataClass()) {
  default:
    throw Exception("Unhandled type for unique function");
    MacroExpandCasesAll(MacroUnique);
  }
}

#undef MacroUnique

ArrayVector UniqueFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("unique function requires at least one argument");
  if (arg[0].isEmpty()) {
    if (nargout <= 1)
      return ArrayVector(arg[0]);
    else {
      ArrayVector retval;
      retval.push_back(arg[0]);
      retval.push_back(Array(Double));
      retval.push_back(Array(Double));
      return retval;
    }
  }
  bool rowmode = false;
  if (arg.size() == 2) {
    if (arg[1].asString().toLower() == "rows")
      throw Exception("second argument to unique must be 'rows'");
    rowmode = true;
  }
  if (rowmode && !arg[0].is2D())
    throw Exception("'rows' mode only works for matrix (2D) arguments");
  if (!rowmode)
    return UniqueFunctionAux(Vectorize(arg[0]));
  else
    return UniqueFunctionAux(arg[0]);
}
