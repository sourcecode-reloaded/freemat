%!
%@Module CSCH Hyperbolic Cosecant Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the hyperbolic cosecant of the argument.
%The syntax for its use is
%@[
%   y = csch(x)
%@]
%@@Function Internals
%The @|csch| function is computed from the formula
%\[
%   \mathrm{csch}(x) = \frac{1}{\sinh(x)}
%\]
%@@Examples
%Here is a simple plot of the hyperbolic cosecant function
%@<
%x1 = -pi+.01:.01:-.01;
%x2 = .01:.01:pi-.01;
%plot(x1,csch(x1),x2,csch(x2)); grid('on');
%mprint('cschplot');
%@>
%@figure cschplot
%@@Tests
%@$near|y1=csch(x1)
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = csch(x)
  y = 1.0 ./ sinh(x);
  