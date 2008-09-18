%!
%@Module SINH Hyperbolic Sine Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the hyperbolic sine of the argument.
%The syntax for its use is
%@[
%   y = sinh(x)
%@]
%@@Function Internals
%The @|sinh| function is computed from the formula
%\[
%   \sinh(x) = \frac{e^x+e^{-x}}{2}
%\]
%@@Examples
%Here is a simple plot of the hyperbolic sine function
%@<
%x = linspace(-5,5);
%plot(x,sinh(x)); grid('on');
%mprint('sinhplot');
%@>
%@figure sinhplot
%@@Tests
%@$y1=sinh(x1)
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = sinh(x)
  y = (exp(x)-exp(-x))/2;
  
