%!
%@Module COSH Hyperbolic Cosine Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the hyperbolic cosine of the argument.
%The syntax for its use is
%@[
%   y = cosh(x)
%@]
%@@Function Internals
%The @|cosh| function is computed from the formula
%\[
%   \cosh(x) = \frac{e^x+e^{-x}}{2}
%\]
%@@Examples
%Here is a simple plot of the hyperbolic cosine function
%@<
%x = linspace(-5,5);
%plot(x,cosh(x)); grid('on');
%mprint('coshplot');
%@>
%@figure coshplot
%@@Tests
%@$y1=cosh(x1)
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = cosh(x)
  y = (exp(x)+exp(-x))/2;
  
