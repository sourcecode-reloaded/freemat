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
#include <boost/timer/timer.hpp>
#include <time.h>
#include "Array.hpp"

//@@Signature
//function tic TicFunction jitsafe
//inputs none
//outputs none
//DOCBLOCK freemat_tic
  
static boost::timer::cpu_timer *timer = NULL;

ArrayVector TicFunction(int nargout, const ArrayVector& arg) {
  if (!timer)
    timer = new boost::timer::cpu_timer;
  timer->start();
  return ArrayVector();
}

//@@Signature
//function clock ClockFunction jitsafe
//inputs none
//outputs y
//DOCBLOCK freemat_clock
ArrayVector ClockFunction(int nargout, const ArrayVector& arg) {
  time_t rawtime;
  struct tm * timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  Array retvec(Double,NTuple(1,6));
  BasicArray<double> &dp(retvec.real<double>());
  dp[1] = timeinfo->tm_year;
  dp[2] = timeinfo->tm_mon;
  dp[3] = timeinfo->tm_mday;
  dp[4] = timeinfo->tm_hour;
  dp[5] = timeinfo->tm_min;
  dp[6] = timeinfo->tm_sec;
  return ArrayVector(retvec);
}

//@@Signature
//function clocktotime ClockToTimeFunction jitsafe
//inputs x
//outputs y
//DOCBLOCK freemat_clocktotime
ArrayVector ClockToTimeFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("clocktotime expects 1 argument - a vector in clock format: [year month day hour minute seconds]");
  Array targ(arg[0].toClass(Double));
  if (targ.length() != 6)
    throw Exception("clocktotime expects 1 argument - a vector in clock format: [year month day hour minute seconds]");
  const BasicArray<double> &dp(targ.constReal<double>());
  struct tm timeinfo;
  timeinfo.tm_year = int(dp[1]);
  timeinfo.tm_mon = int(dp[2]);
  timeinfo.tm_mday = int(dp[3]);
  timeinfo.tm_hour = int(dp[4]);
  timeinfo.tm_min = int(dp[5]);
  timeinfo.tm_sec = int(dp[6]);
  double retval = mktime(&timeinfo);
  return ArrayVector(Array(retval));
}

//@@Signature
//function toc TocFunction jitsafe
//inputs none
//outputs y
//DOCBLOCK freemat_toc
ArrayVector TocFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(timer->elapsed().wall/1.0e9));
}
