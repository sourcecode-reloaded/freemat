%!
%@Module ASECD Inverse Secant Degrees Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the inverse secant of the argument, but returns
%the argument in degrees instead of radians (as is the case
%for @|asec|. The syntax for its use is
%@[
%   y = asecd(x)
%@]
%@@Examples
%The inverse secant of @|2/sqrt(2)| should be 45 degrees:
%@<
%asecd(2/sqrt(2))
%@>
%and the inverse secant of @|2| should be 60 degrees:
%@<
%asecd(2)
%@>
%@@Tests
%@$"y=asecd(0.342)","0-99.43555562312426i","close"
%@$"y=asecd(2)","60","close"
%@$"y=asecd(float(2.523))","float(66.6495819)","close"
%@$"y=asecd(inf)","90","close"
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = asecd(x)
  if (nargin == 0 || ~isnumeric(x))
    error('asecd expects a numeric input');
  end
  y = rad2deg(asec(x));
