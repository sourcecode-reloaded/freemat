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

/**
 * Compute the eigen decomposition of an arbitrary, real valued
 * matrix of size $$n \times n$$.  The $$n \times n$$ matrix of
 * real eigenvectors are stored in the array v, which must be 
 * pre-allocated before the routine is called.  The $$n$$ complex 
 * eigenvalues are stored in the array d, with the real and 
 * imaginary parts stored sequentially.
 */
template <typename T>
void realEigenDecompose(int n, T *v, T *d, T *a, bool eigenvectors, bool balance);
template <typename T>
void realEigenDecomposeSymmetric(int n, T *v, T *d, T *a, bool eigenvectors);
template <typename T>
void realGenEigenDecompose(int n, T *v, T *d, T *a, T *b, bool eigenvectors);
template <typename T>
bool realGenEigenDecomposeSymmetric(int n, T *v, T *d, T *a, T *b, bool eigenvectors);

/**
 * Compute the eigen decomposition of an arbitrary, complex valued
 * matrix of size $$n \times n$$.  The $$n \times n$$ matrix of
 * complex eigenvectors are stored in the array v, which must be 
 * pre-allocated before the routine is called.  The $$n$$ complex 
 * eigenvalues are stored in the array d.
 */
template <typename T>
void complexEigenDecompose(int n, T *v, T *d, T *a, bool eigenvectors, bool balance);
template <typename T>
void complexEigenDecomposeSymmetric(int n, T *v, T *d, T *a, bool eigenvectors);
template <typename T>
void complexGenEigenDecompose(int n, T *v, T *d, T *a, T *b, bool eigenvectors);
template <typename T>
bool complexGenEigenDecomposeSymmetric(int n, T *v, T *d, T *a, T *b, bool eigenvectors);

#endif
