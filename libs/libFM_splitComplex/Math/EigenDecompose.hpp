#ifndef __EigenDecompose_hpp__
#define __EigenDecompose_hpp__

#include "Object.hpp"

namespace FM {
  void EigenDecomposeCompactSymmetric(Object A, Object& D, ThreadContext *ctxt);
  void EigenDecomposeFullSymmetric(const Object& A, Object& V, Object& D, ThreadContext *ctxt);
  void EigenDecomposeFullGeneral(const Object& A, Object& V, Object& D, bool balanceFlag, ThreadContext *ctxt);
  void EigenDecomposeCompactGeneral(Object A, Object& D, bool balanceFlag, ThreadContext *ctxt);
  bool GeneralizedEigenDecomposeCompactSymmetric(Object A, Object B, Object& D, ThreadContext *ctxt);
  bool GeneralizedEigenDecomposeFullSymmetric(Object A,  Object B, Object& V, Object& D, ThreadContext *ctxt);
  void GeneralizedEigenDecomposeFullGeneral(Object A, Object B, Object& V, Object& D, ThreadContext *ctxt);
  void GeneralizedEigenDecomposeCompactGeneral(Object A, Object B, Object& D, ThreadContext *ctxt);
}

#endif
