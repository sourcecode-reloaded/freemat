
template <class T>
void SimpleArray::resize(const Dimensions& a) {
  Dimensions newSize(m_dims.expandToCover(a));
  if (newSize == m_dims) return;
  if (newSize.getElementCount() == length()) {
    reshape(newSize);
    return;
  }

}
