function [Z,x,y] = peaks(varargin)
  if (nargin == 0)
    [x,y] = meshgrid(linspace(-3,3,49),linspace(-3,3,49));
  elseif (nargin == 1)
    if (isscalar(varargin{1}))
      N = varargs{1};
      [x,y] = meshgrid(linspace(-3,3,N),linspace(-3,3,N));
    else
      N = numel(varargs{1});
      [x,y] = meshgrid(linspace(-3,3,N),linspace(-3,3,N));
    end
  else
    x = varargs{1}
    y = varargs{2}
  end
  Z = 3 * (1 - x) .^ 2 .* exp(- x .^ 2 - (y + 1) .^ 2) - 10 * (x / 5 - x .^ 3 - y .^ 5) .* exp(- x .^ 2 - y .^ 2) - 1 / 3 * exp(- (x + 1) .^ 2 - y .^ 2);

