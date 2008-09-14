%!
%@Module STRCMPI String Compare Case Insensitive Function
%@@Section STRING
%@@Usage
%Compares two strings for equality ignoring case.  The general
%syntax for its use is 
%@[
%   p = strcmpi(x,y)
%@]
%where @|x| and @|y| are two strings, or cell arrays of strings.
%See @|strcmp| for more help.
%@@Tests
%@$y1=strcmpi(x1,x2)
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = strcmpi(source,pattern)
  y = strcmp(upper(source),upper(pattern));
  
