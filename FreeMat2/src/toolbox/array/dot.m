%!
%@Module DOT Dot Product Function
%@@Section ELEMENTARY
%@@Usage
%Computes the scalar dot product of its two arguments.  The general
%syntax for its use is
%@[
%  y = dot(x,z)
%@]
%where @|x| and @|z| are numerical vectors of the same length.  If 
%@|x| and @|z| are multi-dimensional arrays of the same size, then
%the dot product is taken along the first non-singleton dimension.
%You can also specify the dimension to take the dot product along using
%the alternate form
%@[
%  y = dot(x,z,dim)
%@]
%where @|dim| specifies the dimension to take the dot product along.
%@@Tests
%@$y1=dot(x1,x2)
%@$y1=dot(x1,x2,2)
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = dot(x,z,dim)
  if (isvector(x) && isvector(z))
    if (numel(x) ~= numel(z))
      error('dot requires its arguments to be equal length');
    end
    y = x(:)'*z(:);
  else
    try
      q = x.*z;
    catch
      error('dot requires multi-dimensional arguments to be the same size and numeric');
    end
    if (~exist('dim'))
      y = sum(q);
    else
      y = sum(q,dim);
    end
  end
  
    
