% STD STD Standard Deviation Function
% 
% Usage
% 
% Computes the standard deviation of an array along a given dimension.  
% The general syntax for its use is
% 
%   y = std(x,d)
% 
% where x is an n-dimensions array of numerical type.
% The output is of the same numerical type as the input.  The argument
% d is optional, and denotes the dimension along which to take
% the variance.  The output y is the same size as x, except
% that it is singular along the mean direction.  So, for example,
% if x is a 3 x 3 x 4 array, and we compute the mean along
% dimension d=2, then the output is of size 3 x 1 x 4.
% STD STD Standard Deviation Function
% 
% Usage
% 
% Computes the standard deviation of an array along a given dimension.  
% The general syntax for its use is
% 
%   y = std(x,d)
% 
% where x is an n-dimensions array of numerical type.
% The output is of the same numerical type as the input.  The argument
% d is optional, and denotes the dimension along which to take
% the variance.  The output y is the same size as x, except
% that it is singular along the mean direction.  So, for example,
% if x is a 3 x 3 x 4 array, and we compute the mean along
% dimension d=2, then the output is of size 3 x 1 x 4.

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = std(varargin)
  y = sqrt(var(varargin{:}));
