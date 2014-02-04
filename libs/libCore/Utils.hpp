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

#ifndef __Utils_hpp__
#define __Utils_hpp__

#include "Types.hpp"
#include "Array.hpp"
#include "FMLib.hpp"

bool contains(StringVector& list, FMString s, bool regexpmode);

NTuple ArrayVectorAsDimensions(const ArrayVector &arg);

double ArrayRange(const Array& dp);
double ArrayMin(const Array& dp);
double ArrayMax(const Array& dp);

Array DoubleVectorFromFMList(FMList<uint32> &ref);

int digitvalue(char x);

void SwapBuffer(char* cp, int count, int elsize);

FMString ReadFMStringFromFile(FMFile *fp);

void WriteFMStringToFile(FMFile *fp, FMString txt);

double FMFileReadInteger(FMFile *fp, int base, int nMaxDigits = 1024 );

double FMFileReadFloat(FMFile *fp, int nMaxDigits = 1024 );

FMString FMFileReadString(FMFile *fp, int nMaxChars = 65535 );

char FMFileReadChar(FMFile *fp);
#endif
