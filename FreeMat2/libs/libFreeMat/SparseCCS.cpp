


#include "SparseCCS.hpp"
#include "Math.hpp"

Array IJVForm::rows() {
  return m_rows;
}

Array IJVForm::cols() {
  return m_cols;
}

Array IJVForm::indices() {
  return Add(Multiply(Subtract(m_cols,Array(double(1))),
		      Array(double(m_size_rows))),m_rows); 
}

Array IJVForm::values() {
  return m_values;
}

index_t IJVForm::nnz() {
  return m_values.length();
}
