%!
%@Module POLYVAL Evaluate Polynomial Fit at Selected Points
%@@Section CURVEFIT
%@@Usage
%The @|polyval| routine has the following syntax
%@[
%  y = polyval(p,x)
%@]
%where @|p| is a vector of polynomial coefficients,
%in decreasing degree (as generated by @|polyfit|, for example).
%If @|x| is a matrix, the polynomial is evaluated in the matrix
%sense (in which case @|x| must be square).
%@@Function Internals
%The polynomial is evaluated using a recursion method.  If the
%polynomial is
%\[
%   p(x) = p_1 x^n + p_2 x^{n-1} + \dots + p_n x + p_{n+1}
%\]
%then the calculation is performed as
%\[
%   p(x) = ((p_1) x + p_2) x + p_3
%\]
%@@Example
%Here is a plot of @|x^3| generated using polyval
%@<
%p = [1 0 0 0]
%x = linspace(-1,1);
%y = polyval(p,x);
%plot(x,y,'r-')
%mprint polyval1
%@>
%Here is the resulting plot
%@figure polyval1
%@@Tests
%@$near#y1=polyval([1,0,0,0],linspace(-1,1))
%!

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function y = polyval(p,x)
y = p(1);
n = prod(size(p));
for i = 2:n
  y = y.*x + p(i);
end
