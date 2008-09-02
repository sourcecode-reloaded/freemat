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
#include "Array.hpp"
#include "Interpreter.hpp"
#include "FunctionDef.hpp"
#include "Exception.hpp"

static ArrayVector params;
static Array xval;
static Array yval;
static Array wval;
static Interpreter *a_eval;
static FunctionDef *a_funcDef;
static double *xbuffer;
static double *ybuffer;

extern "C" { 
  void fcnstub(int* m, int *n, double *x, double *fvec, int *iflag) {
    double *xp, *yp, *rp, *wp;
    xp = xval.real<double>().data();
    yp = yval.real<double>().data();
    wp = wval.real<double>().data();
    memcpy(xp,x,sizeof(double)*(*n));
    ArrayVector tocall(params);
    tocall.push_front(xval);
    ArrayVector cval(a_funcDef->evaluateFunction(a_eval,tocall,1));
    if (cval.size() == 0)
      throw Exception("function to be optimized does not return any outputs!");
    if (int(cval[0].length()) != (*m))
      throw Exception("function output does not match size of vector 'y'");
    Array f(cval[0]);
    f = f.asDenseArray().toClass(Double);
    rp = (double*) f.real<double>().data();
    int i;
    for (i=0;i<(*m);i++) {
      fvec[i] = wp[i]*(yp[i] - rp[i]);
    }
  }
}

typedef void (*fcnptr)(int*, int*, double*, double*, int*);

extern "C"
void lmdif1_(fcnptr, int*m, int*n, double*x, double*fvec, double*tol,
	     int*info, int*iwa, double*wa, int*lwa);

//!
//@Module FITFUN Fit a Function
//@@Section CURVEFIT
//@@Usage
//Fits @|n| (non-linear) functions of @|m| variables using least squares
//and the Levenberg-Marquardt algorithm.  The general syntax for its usage
//is
//@[
//  [xopt,yopt] = fitfun(fcn,xinit,y,weights,tol,params...)
//@]
//Where @|fcn| is the name of the function to be fit, @|xinit| is the
//initial guess for the solution (required), @|y| is the right hand side,
//i.e., the vector @|y| such that:
//\[
//   xopt = \arg \min_{x} \|\mathrm{diag}(weights)*(f(x) - y)\|_2^2,
//\]
//the output @|yopt| is the function @|fcn| evaluated at @|xopt|.  
//The vector @|weights| must be the same size as @|y|, and contains the
//relative weight to assign to an error in each output value.  Generally,
//the ith weight should reflect your confidence in the ith measurement.
//The parameter @|tol| is the tolerance used for convergence.
//The function @|fcn| must return a vector of the same size as @|y|,
//and @|params| are passed to @|fcn| after the argument @|x|, i.e.,
//\[
//  y = fcn(x,param1,param2,...).
//\]
//Note that both @|x| and @|y| (and the output of the function) must all
//be real variables.  Complex variables are not handled yet.
//@@Tests
//@{ test_fitfun1.m
//% Test the fitfun bug (bug 1514605)
//function test_val = test_fitfun1
//O = 3/4*pi;
//y0 = cos(O);
//[x y] = fitfun('cos',3.5/4*pi,y0,1,0.01);
//test_val = abs((x-O)/O*100)<.1;
//@}
//@@Signature
//sfunction fitfun FitFunFunction
//inputs fcn xinit y weights tol varargin
//outputs xopt yopt
//!
ArrayVector FitFunFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size()<4) 
    throw Exception("fitfun requires at least four arguments");
  if (!(arg[0].isString()))
    throw Exception("first argument to fitfun must be the name of a function (i.e., a string)");
  QString fname = arg[0].asString();
  eval->rescanPath();
  Context *context = eval->getContext();
  FuncPtr funcDef;
  if (!context->lookupFunction(fname,funcDef))
    throw Exception(QString("function ") + fname + " undefined!");
  funcDef->updateCode(eval);
  if (funcDef->scriptFlag)
    throw Exception("cannot use feval on a script");
  a_funcDef = funcDef;
  a_eval = eval;
  // Get the initial guess vector
  Array xinit(arg[1].asDenseArray().toClass(Double));
  int m, n;
  n = int(xinit.length());
  // Get the right hand side vector
  Array yvec(arg[2].asDenseArray().toClass(Double));
  m = int(yvec.length());
  yval = yvec;
  xval = xinit;
  wval = arg[3].asDenseArray().toClass(Double);
  if (int(wval.length()) != m)
    throw Exception("weight vector must be the same size as the output vector y");
  // Get the tolerance
  double tol = arg[4].asDouble();
  // Copy the arg array
  params = arg;
  params.pop_front();
  params.pop_front();
  params.pop_front();
  params.pop_front();
  params.pop_front();
  // Test to make sure the function works....
  ArrayVector tocall(params);
  tocall.push_front(xinit);
  ArrayVector cval(funcDef->evaluateFunction(eval,tocall,1));
  if (cval.size() == 0)
    throw Exception("function to be optimized does not return any outputs!");
  if (int(cval[0].length()) != m)
    throw Exception("function output does not match size of vector 'y'");
  // Call the lmdif1
  int *iwa;
  iwa = new int[n];
  int lwa;
  lwa = m*n+5*n+m;
  double *wa;
  int info;
  wa = new double[lwa];
  xbuffer = new double[n];
  ybuffer = new double[m];
  memcpy(xbuffer,xinit.real<double>().data(),sizeof(double)*n);
  memset(ybuffer,0,sizeof(double)*m);
  lmdif1_(fcnstub,&m,&n,xbuffer,ybuffer,&tol,&info,iwa,wa,&lwa);
  if (info == 0)
    throw Exception("Illegal input parameters to lmdif (most likely more variables than functions?)");
  if (info == 5)
    eval->warningMessage("number of calls to the supplied function has reached or exceeded the limit (200*(number of variables + 1)");
  if (info == 6)
    eval->warningMessage("tolerance is too small - no further reduction in the sum of squares is possible");
  if (info == 7)
    eval->warningMessage("tolerance is too small - no further improvement in the approximate solution x is possible");
  delete[] wa;
  delete[] iwa;
  memcpy(xval.real<double>().data(),xbuffer,sizeof(double)*n);
  delete[] xbuffer;
  delete[] ybuffer;
  tocall = params;
  tocall.push_front(xval);
  cval = funcDef->evaluateFunction(eval,tocall,1);
  ArrayVector retval;
  retval.push_back(xval);
  retval.push_back(cval[0]);
  return retval;
}
