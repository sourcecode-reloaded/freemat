/*
 * Copyright (c) 2009 Samit Basu
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
#ifndef __SparseCCS_hpp__
#define __SparseCCS_hpp__

#include "FMLib.hpp"
#include "SparseMatrix.hpp"
#include "Array.hpp"

FMVector<uint32> CompressCCSCols(const FMVector<uint32> &cols, index_t colcount);
FMVector<uint32> DecompressCCSCols(const FMVector<uint32> &ccols, index_t colcount);

template <typename T>
void SparseToCCS(const SparseMatrix<T>&A,
		 FMVector<uint32> &rowstart,
		 FMVector<uint32> &colstart,
		 FMVector<T> &Adata) {
  FMVector<uint32> cols;
  ConstSparseIterator<T> iter(&A);
  while (iter.isValid()) {
    cols << uint32(iter.col());
    rowstart << uint32(iter.row()-1);
    Adata << iter.value();
    iter.next();
  }
  colstart = CompressCCSCols(cols,A.cols());
}

template <typename T>
void CCSToSparse(SparseMatrix<T> &A,
		 const FMVector<uint32> &rowstart,
		 const FMVector<uint32> &colstart,
		 const FMVector<T> &Adata) {
  FMVector<uint32> cols = DecompressCCSCols(colstart,A.cols());
  for (int i=0;i<cols.size();i++) 
    A.set(NTuple(rowstart[i]+1,cols[i]),Adata[i]);
}

template <typename T>
void CCSToSparse(SparseMatrix<T> &Areal,
		 SparseMatrix<T> &Aimag,
		 const FMVector<uint32> &rowstart,
		 const FMVector<uint32> &colstart,
		 const FMVector<T> &Areal_part,
		 const FMVector<T> &Aimag_part) {
  FMVector<uint32> cols = DecompressCCSCols(colstart,Areal.cols());
  for (int i=0;i<cols.size();i++) {
    Areal.set(NTuple(rowstart[i]+1,cols[i]),Areal_part[i]);
    Aimag.set(NTuple(rowstart[i]+1,cols[i]),Aimag_part[i]);
  }
}
		 

template <typename T>
void SparseToCCS(const SparseMatrix<T> &Areal, 
		 const SparseMatrix<T> &Aimag,
		 FMVector<uint32> &rowstart,
		 FMVector<uint32> &colstart,
		 FMVector<T> &Areal_part,
		 FMVector<T> &Aimag_part) {
  FMVector<uint32> cols;
  ConstComplexSparseIterator<T> iter(&Areal,&Aimag);
  while (iter.isValid()) {
    cols << uint32(iter.col());
    rowstart << uint32(iter.row()-1);
    Areal_part << iter.realValue();
    Aimag_part << iter.imagValue();
    iter.next();
  }
  colstart = CompressCCSCols(cols,Areal.cols());
}

Array IJVToSparse(const BasicArray<index_t> &ip,const BasicArray<index_t> &jp,const Array &dat);
Array IJVToSparse(const BasicArray<index_t> &ip, const BasicArray<index_t> &jp, const Array &dat,
		  index_t rows, index_t cols);

Array SparseToIJV(const Array &a, Array &rows, Array &cols);

template <class T>
class RLEEncoderComplex {
  FMVector<T> buffer;
  int m;
  int zlen;
  int state;
public:
  RLEEncoderComplex() {
    m = 0;
    state = 0;
    zlen = 0;
  }
  int row() {
    return m;
  }
  void set(int p) {
    if (p <= m) return;
    p -= m;
    if (state == 0) {
      zlen = p;
      state = 1;
    } else
      zlen += p;
    m += p;  
  }
  void push(T valr, T vali) {
    if (state == 0) {
      if ((valr != 0) || (vali != 0)) {
	buffer << valr;
	buffer << vali;
      } else {
	state = 1;
	zlen = 1;
      }
    } else {
      if ((valr == 0) && (vali == 0)) {
	zlen++;
      } else {
	if (zlen) {
	  buffer << 0;
	  buffer << 0;
	  buffer << zlen;
	}
	buffer << valr;
	buffer << vali;
	state = 0;
	zlen = 0;
      }
    }
    m++;
  }
  void end(int len) {
    set(len);
    if (zlen>0) {
      buffer << 0;
      buffer << 0;
      buffer << zlen;
    }
    state = 0;
  }
  FMVector<T> copyout() {
    return buffer;
  }
};


template <class T>
class RLEEncoder {
  FMVector<T> buffer;
  int m;
  int zlen;
  int state;
public:
  RLEEncoder() {
    m = 0;
    state = 0;
    zlen = 0;
  }
  int row() {return m;}
  void set(int p) {
    if (p <= m) return;
    p -= m;
    if (state == 0) {
      zlen = p;
      state = 1;
    } else
      zlen += p;
    m += p;  
  }
  void push(T val) {
    if (state == 0) {
      if (val != 0) {
	buffer << val;
      } else {
	state = 1;
	zlen = 1;
      }
    } else {
      if (val == 0) {
	zlen++;
      } else {
	if (zlen) {
	  buffer << 0;
	  buffer << zlen;
	}
	buffer << val;
	state = 0;
	zlen = 0;
      }
    }
    m++;
  }
  void end(int len) {
    set(len);
    if (zlen) {
      buffer << 0;
      buffer << zlen;
    }
    state = 0;
  }
  FMVector<T> copyout() {
    return buffer;
  }
};

template <class T>
class RLEDecoder {
  const FMVector<T> &data;
  int m;
  int n;
  int len;
public:
  RLEDecoder(const FMVector<T> &str) : data(str) {
    m = 0;
    n = 1;
    len = str.size();
  }
  int row() {
    return m;
  }
  void update() {
    while ((m < len) && (data[n] == 0)) {
      m += (int) data[n+1];
      n += 2;
      if ((m < len) && (n>((int)data[0]))) {
	throw Exception("Invalid data string!\n");
      }
    }
  }
  void advance() {
    if (m < len) {
      m++;
      n++;
      update();
    }
  }
  T value() {
    if (m >= len)
      throw Exception("RLE Decoder overflow - corrupted sparse matrix string encountered");
    return data[n];
  }
  bool more() {
    return (m < len);
  }
  int nnzs() {
    int nnz = 0;
    int t = 0;
    int p = 1;
    while (t < len) {
      if (data[p] != 0) {
	nnz++;
	p++;
	t++;
      } else {
	t += (int) data[p+1];
	p += 2;
      }
    }
    return nnz;
  }
};

template <class T>
class RLEDecoderComplex {
  const FMVector<T>& data;
  int m;
  int n;
  int len;
public:
  RLEDecoderComplex(const FMVector<T>& str) : data(str) {
    m = 0;
    n = 1;
    len = str.size();
  }
  int row() {
    return m;
  }
  void update() {
    while ((m < len) && (data[n] == 0) && (data[n+1] == 0)) {
      m += (int) data[n+2];
      n += 3;
    }
  }
  void advance() {
    if (m < len) {
      m++;
      n+=2;
      update();
    }
  }
  T value_real() {
    if (m >= len)
      throw Exception("RLE DecoderComplex overflow - corrupted sparse matrix string encountered");
    return data[n];
  }
  T value_imag() {
    if (m >= len)
      throw Exception("RLE DecoderComplex overflow - corrupted sparse matrix string encountered");
    return data[n+1];
  }
  bool more() {
    return (m < len);
  }
  int nnzs() {
    int nnz = 0;
    int t = 0;
    int p = 1;
    while (t < len) {
      if ((data[p] != 0) || (data[p+1] != 0)) {
	nnz++;
	p+=2;
	t++;
      } else {
	t += (int) data[p+2];
	p += 3;
      }
    }
    return nnz;
  }
};


template <typename T>
Array FM3Sparse(const FMVector<FMVector<T> >& arg, NTuple dims) {
  SparseMatrix<T> retval(dims);
  for (int i=0;i<arg.size();i++) {
    RLEDecoder<T> rle(arg[i]);
    rle.update();
    while (rle.more()) {
      retval[NTuple(rle.row()+1,i+1)] = rle.value();
      rle.advance();
    }
  }
  return Array(retval);
}

template <typename T>
Array FM3SparseComplex(const FMVector<FMVector<T> >&arg, NTuple dims) {
  SparseMatrix<T> real(dims);
  SparseMatrix<T> imag(dims);
  for (int i=0;i<arg.size();i++) {
    RLEDecoderComplex<T> rle(arg[i]);
    rle.update();
    while (rle.more()) {
      real[NTuple(rle.row()+1,i+1)] = rle.value_real();
      imag[NTuple(rle.row()+1,i+1)] = rle.value_imag();
      rle.advance();
    }
  }
  return Array(real,imag);
}

template <typename T>
FMVector<FMVector<T> > SparseFM3(const SparseMatrix<T>& arg) {
  FMVector<FMVector<T> > ret(int(arg.cols()));
  ConstSparseIterator<T> iter(&arg);
  while (iter.isValid()) {
    RLEEncoder<T> rle;
    index_t col_number = iter.col();
    while (iter.col() == col_number) {
      rle.set(int(iter.row()-1));
      rle.push(iter.value());
      iter.next();
    }
    rle.end(int(arg.rows()));
    ret[int(col_number-1)] = rle.copyout();
  }
  return ret;
}

template <typename T>
FMVector<FMVector<T> > SparseFM3(const SparseMatrix<T>& real, const SparseMatrix<T>& imag) {
  FMVector<FMVector<T> > ret(int(real.cols()));
  ConstComplexSparseIterator<T> iter(&real,&imag);
  while (iter.isValid()) {
    RLEEncoderComplex<T> rle;
    index_t col_number = iter.col();
    while (iter.col() == col_number) {
      rle.set(int(iter.row()-1));
      rle.push(iter.realValue(),iter.imagValue());
      iter.next();
    }
    rle.end(int(real.rows()));
    ret[int(col_number-1)] = rle.copyout();
  }
  return ret;
}


#endif
