#ifndef __NDimArray_hpp__
#define __NDimArray_hpp__

// Issues around the design of the array class:
//
//  Sparse vs Dense
//  Slicing support
//  Reference counted vs Copied
//  Scalars

/**
 * SimpleArray
 * MatrixArray
 * SparseArray
 */

template <typename T>
Array {
  virtual size_t length() const = 0;
  virtual size_t rows() const = 0;
  virtual size_t columns() const = 0;
  virtual bool isEmpty() const = 0;
  virtual bool isScalar() const = 0;
  virtual bool is2D() const = 0;
  virtual bool isVector() const = 0;
  virtual bool isColumnVector() const = 0;
  virtual bool isRowVector() const = 0;
  Array<T> resize(const Dimensions& a) const = 0;
  Array<T> vectorResize(const Dimensions& a) const = 0;
  void reshape(const Dimensions& a) = 0;
  Array<T> getVectorSubset(const Array<index_t> &index) = 0;
  Array<T> getNDimSubset(const std::vector<Array<index_t> > &index) = 0;
  Array<T> setVectorSubset
};

Scalar : Array {
  T value;
};

LargeArray : Array {
  Memblock<T> m_base;
  Dimensions m_dims;
};

SlicedArray : LargeArray {
  vector<vector<index> > m_slices;
};

class GenericArray;

template <class T>
NumericArray : GenericArray {
  Array<T> m_real;
  Array<T> m_imag;
};

StructArray : GenericArray {
  vector<string> m_fieldnames;
  vector<GenericArray> m_data;
};

CellArray : GenericArray {
  Array<GenericArray> m_vals;
}


/**
 * This is the generic array class for FreeMat.  It represents
 * an N-dimensional array of an arbitrary number of dimensions.
 * The array is templated, and some basic operations can be 
 * performed on the base class.
 */

typedef int64 index_t;
typedef int64 size_t;
typedef std::vector<std::string> StringVector;

class Dimensions : std::vector<size_t> {
  Dimensions() : std::vector<size_t> () {
    resize(2);
  }
  inline size_t getElementCount() const {
    if (empty) return 0;
    size_t m_length = 1;
    for (int i=0;i<size();i++)
      m_length *= at(i);
    return m_length;
  }
  inline const Dimensions& expandToCover(const Dimensions& a) const {
    
  }
};

template <class T>
class SimpleArray {
  /** Compute the length of the array
   * Get the length of the array as a vector.  This is equivalent
   * to computing length(this(:)).
   */
  inline size_t length() const {
    if (m_length_cache_valid)
      return m_length;
    m_length = m_dims.getElementCount();
    m_length_cache_valid = true;
    return m_length;
  }
  inline size_t rows() const {
    return m_dims[0];
  }
  inline size_t columns() const {
    return m_dims[1];
  }
  inline bool isEmpty() const {
    return (length() == 0);
  }
  inline bool isScalar() const {
    return (length() == 1);
  }
  inline bool is2D() const {
    return (length() == (m_dims[0]*m_dims[1]));
  }
  inline bool isVector() const {
    return ((length() == m_dims[0]) ||
	    (length() == m_dims[1]));
  }
  inline bool isColumnVector() const {
    return (length() == m_dims[0]);
  }
  inline bool isRowVector() const {
    return (length() == m_dims[1]);
  }
  /** Resize an array.
   * Resize the array to a new set of dimensions.  This resize operation
   * puts the contents of the array at the (0,...,0) corner of the new
   * array.  
   */
  virtual void resize(const Dimensions& a) = 0;
  /** Resize an array based on a vector indexing expression.
   * This method resizes an object so that an index of the type this(n)
   * is valid.  In particular, if "this" is a scalar, then this(n) extends
   * the array in the column dimension.  If "this" is a column vector, then
   * this(n) extends the array in the row dimension.  If "this" is a row
   * vector, then this(n) extends the array in the column direction.
   * For an arbitrarily dimensioned array, this(n) makes the array into 
   * a row vector of length n.
   */
  virtual void vectorResize(const Dimensions& a) = 0;
  /** Reshape an array.
   * Reshape the array along a new set of dimensions.  Valid provided that
   * setting the dimensions of the array to a does not change the number of
   * elements in the array.
   * Throws an exception if the new dimension has a different number of elements
   * than we currently have.
   */
  virtual void reshape(Dimensions& a) = 0;
  /** Get a vector subset of an array.
   * Get a subset of an Array.  This is for vector-indexing, meaning that
   * the argument is assumed to refer to the elements in their order as a vector.
   * So, x(10) is equivalent to x(:)(10), even if, say, x is 3 x 4.
   * Throws an exception if
   *  - the variable is empty
   *  - the argument subset exceeds our valid domain
   */
  SimpleArray<T> getVectorSubset(const SimpleArray<index_t> &index) = 0;
  /** Get an n-dimensional subset of an array.
   * Get a subset of an Array.  This if for n-Dimensional-indexing, meaning
   * that x(10) is really x(10,1).
   * Throws an exception if the variable is empty.
   */
  SimpleArray<T> getNDimSubset(const std::vector<SimpleArray<index_t> >&index) = 0;
  /** Set a vector subset of an array.
   * Set a subset of an Array.  Uses vector-indexing, meaning that the
   * argument is assumed to refer to the elements in their order as a vector.
   * So, x(10) is equivalent to x(:)(10), even if, say, x is 3 x 4.
   * Throws an exception if there is a size mismatch between the index and the data.
   */
  void setVectorSubset(const SimpleArray<index_t> &index, 
		       const SimpleArray<T> &data) = 0;
  /**
   * Set a subset of an Array.   This if for n-Dimensional-indexing, meaning
   * that x(10) is really x(10,1).
   * Throws an exception if there is a size mismatch between the index and the data.
   */
  void setNDimSubset(const std::vector<const SimpleArray<index_t> >&index,
		     const SimpleArray<T> &data) = 0;
  /**
   * Delete a subset of this array using the argument for vector indexing.
   * This is _much_ simpler than the planar case.  Here, we simply:
   *   -  Create a deletion map from the index variable.
   *   -  Adjust the size of the output, and reshape to
   *       a vector.
   *   -  Copy (and skip) as necessary.
   * The result is then resized using the same rules as in vectorResize.
   */ 
  void deleteVectorSubset(const SimpleArray<index_t> &index) = 0;
  /**
   * Delete a subset of this array using the arguments for n-Dimensional
   * indexing.  This method is the "planar" delete, meaning that its 
   * designed to delete all the entries in an N-ary array in one dimension.
   * It cannot be used to create "holes" in an array.
   * Throws an exception if the argument contains more than one non-colon index
   */
  void deleteNDimSubset(const std::vector<const SimpleArray<index_t> > &index) = 0;
private:  
  // The dimensions of the array.
  Dimensions m_dims;
  mutable bool m_length_cache_valid;
  mutable size_t m_length_cache;  
};

template <class T>
class DenseArray : public SimpleArray {
  virtual void resize(const Dimensions& a);
  /**
   * Permute our array according to the given permutation vector.  Note that
   * we assume that the permutation vector is of the correct size and is a
   * valid permutation.
   */
  void permute(const Dimensions& a);
private:
  std::vector<T> m_ddata;
};

template <class T>
class SparseArray : public SimpleArray {
  
};

class NumericNDimArray : public NDimArray {
  bool isPositive() const;
  bool isAllZeros() const;
};

template <class T>
class NumericNDimMatrix : public NumericNDimArray {
  bool isSymmetric() const;
  NumericNDimMatrix<T> getDiagonal() const;
};

class SparseNDimMatrix : public NumericNDimMatrix {
};

class StructureArray : public AbstractNDimArray {
  StringVector m_fields;
  
};

template <class T>
class NDimArray {
  // The dimensions of the array.
  std::vector<index_t> m_dims;
  // The actual data for the array.
  std::vector<T> m_data;
  
  
  
};


#endif
