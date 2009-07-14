% NDGRID NDGRID Generate N-Dimensional Grid
% 
% Usage
% 
% Generates N-dimensional grids, each of which is constant in all but
% one dimension.  The syntax for its use is either
% 
%    [y1, y2, ..., ym] = ndgrid(x1, x2, ..., xn)
% 
% where m <= n or 
% 
%    [y1, y2, ..., ym] = ndgrid(x1)
% 
% which is equivalent to the first form, with x1=x2=...=xn.  Each
% output yi is an n-dimensional array, with values such that
% ndgrid is useful for evaluating multivariate functionals over a
% range of arguments.  It is a generalization of meshgrid, except
% that meshgrid transposes the dimensions corresponding to the 
% first two arguments to better fit graphical applications.
% NDGRID NDGRID Generate N-Dimensional Grid
% 
% Usage
% 
% Generates N-dimensional grids, each of which is constant in all but
% one dimension.  The syntax for its use is either
% 
%    [y1, y2, ..., ym] = ndgrid(x1, x2, ..., xn)
% 
% where m <= n or 
% 
%    [y1, y2, ..., ym] = ndgrid(x1)
% 
% which is equivalent to the first form, with x1=x2=...=xn.  Each
% output yi is an n-dimensional array, with values such that
% ndgrid is useful for evaluating multivariate functionals over a
% range of arguments.  It is a generalization of meshgrid, except
% that meshgrid transposes the dimensions corresponding to the 
% first two arguments to better fit graphical applications.
% NDGRID NDGRID Generate N-Dimensional Grid
% 
% Usage
% 
% Generates N-dimensional grids, each of which is constant in all but
% one dimension.  The syntax for its use is either
% 
%    [y1, y2, ..., ym] = ndgrid(x1, x2, ..., xn)
% 
% where m <= n or 
% 
%    [y1, y2, ..., ym] = ndgrid(x1)
% 
% which is equivalent to the first form, with x1=x2=...=xn.  Each
% output yi is an n-dimensional array, with values such that
% ndgrid is useful for evaluating multivariate functionals over a
% range of arguments.  It is a generalization of meshgrid, except
% that meshgrid transposes the dimensions corresponding to the 
% first two arguments to better fit graphical applications.
% NDGRID NDGRID Generate N-Dimensional Grid
% 
% Usage
% 
% Generates N-dimensional grids, each of which is constant in all but
% one dimension.  The syntax for its use is either
% 
%    [y1, y2, ..., ym] = ndgrid(x1, x2, ..., xn)
% 
% where m <= n or 
% 
%    [y1, y2, ..., ym] = ndgrid(x1)
% 
% which is equivalent to the first form, with x1=x2=...=xn.  Each
% output yi is an n-dimensional array, with values such that
% ndgrid is useful for evaluating multivariate functionals over a
% range of arguments.  It is a generalization of meshgrid, except
% that meshgrid transposes the dimensions corresponding to the 
% first two arguments to better fit graphical applications.

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function varargout = ndgrid(varargin)
  if (nargin == 0)
    error('ndgrid requires at least one argument');
  end
  if (nargin == 1)
    varargin = repmat(varargin,[1 max(nargout,2)]);
    nargin = length(varargin);
  end
  if (nargout > nargin)
    error('ndgrid expects number of outputs to be less than or equal to the number of inputs');
  end
  
  % Convert the input vectors into column vectors
  for i=1:length(varargin)
    x = full(varargin{i});
    varargin{i} = x(:);
    dims(i) = numel(varargin{i});
  end
  
  % dims is the dimension of the output arrays.
  % We now look over the output arrays, reshaping
  % them to be single dimension arrays along their
  % corresponding dimension
  varargout = cell(1,nargout);
  for i=1:nargout
    rdims = dims*0+1;
    rdims(i) = dims(i);
    x = reshape(varargin{i},rdims);
    rdims = dims;
    rdims(i) = 1;
    varargout{i} = repmat(x,rdims);
  end
