% DEC2BIN DEC2BIN Convert Decimal to Binary String
% 
% Usage
% 
% Converts an integer to a binary string.  The syntax for its
% use is
% 
%    y = dec2bin(x,n)
% 
% where x is the positive integer, and n is the number of
% bits to use in the representation.  Alternately, if you leave
% n unspecified, 
% 
%    y = dec2bin(x)
% 
% the minimum number of bits needed to represent x are used.
% If x is a vector, then the resulting y is a character
% matrix.

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function t = dec2bin(x,n)
  x = x(:); 
  if (~exist('n') && max(x) > 0)
    n = ceil(log2(max(x)));
  elseif (~exist('n'))
    t = zeros(size(x));
    return;
  elseif (max(x) == 0)
    t = zeros(size(x));
    return;
  end
  t = string(int2bin(x,n)+'0');