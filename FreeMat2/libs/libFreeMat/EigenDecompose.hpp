/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __EigenDecompose_hpp__
#define __EigenDecompose_hpp__

#include "Array.hpp"

/**
 * Compute the eigendecomposition of the matrix $$A$$, the two matrices
 * $$V$$ and $$D$$, where $$D$$ is diagonal, and $$V$$ has unit norm
 * columns.  If $$A$$ is real, the eigenvectors $$V$$ are real, and 
 * the eigenvalues come in conjugate pairs.  
 */
void EigenDecomposeCompactSymmetric(const Array& A, Array& D);
void EigenDecomposeFullSymmetric(const Array& A, Array& V, Array& D);
void EigenDecomposeFullGeneral(const Array& A, Array& V, Array& D, bool balanceFlag);
void EigenDecomposeCompactGeneral(const Array& A, Array& D, bool balanceFlag);
bool GeneralizedEigenDecomposeCompactSymmetric(const Array& A, const Array& B, Array& D);
bool GeneralizedEigenDecomposeFullSymmetric(const Array& A, const Array& B, Array& V, Array& D);
void GeneralizedEigenDecomposeFullGeneral(const Array& A, const Array& B, Array& V, Array& D);
void GeneralizedEigenDecomposeCompactGeneral(const Array& A, const Array& B, Array& D);
ArrayVector SparseEigDecompose(int nargout, Array A, int k, QString whichFlag);
ArrayVector SparseEigDecomposeShifted(int nargout, Array A, int k, double shift[2]);

#endif
