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

#ifndef __Types_hpp__
#define __Types_hpp__

#include <QStringList>
#include <QVector>
#include <QtGlobal>
#include <QList>
#include <QRegExp>
#include <cmath>
#include "FastList.hpp"

typedef bool      logical;
typedef qint8     int8;
typedef quint8    uint8;
typedef qint16    int16;
typedef quint16   uint16;
typedef qint32    int32;
typedef quint32   uint32;
typedef qint64    int64;
typedef quint64   uint64;
typedef double index_t;
const int NDims = 6;

typedef QStringList StringVector;

inline bool IsInteger(float x) {
  return rintf(x) == x;
}

inline bool IsInteger(double x) {
  return rint(x) == x;
}

template <typename T>
inline bool IsNonNegative(T x) {
  return (x >= 0);
}

#endif
