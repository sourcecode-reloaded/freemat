% POLYVAL POLYVAL Evaluate Polynomial Fit at Selected Points
% 
% Usage
% 
% The polyval routine has the following syntax
% 
%   y = polyval(p,x)
% 
% where p is a vector of polynomial coefficients,
% in decreasing degree (as generated by polyfit, for example).
% If x is a matrix, the polynomial is evaluated in the matrix
% sense (in which case x must be square).
% POLYVAL POLYVAL Evaluate Polynomial Fit at Selected Points
% 
% Usage
% 
% The polyval routine has the following syntax
% 
%   y = polyval(p,x)
% 
% where p is a vector of polynomial coefficients,
% in decreasing degree (as generated by polyfit, for example).
% If x is a matrix, the polynomial is evaluated in the matrix
% sense (in which case x must be square).

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function y = polyval(p,x)
y = p(1);
n = prod(size(p));
for i = 2:n
  y = y.*x + p(i);
end
