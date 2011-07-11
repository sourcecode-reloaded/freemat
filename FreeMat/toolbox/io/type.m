% TYPE Type Contents of File or Function
% 
% Usage
% 
% Displays the contents of a file or a function to the screen
% or console.  The syntax for its use is
% 
%    type filename
%    type('filename')
% 
% or
% 
%    type function
%    type('function')
% 
% in which case the function named 'function.m' will be displayed.

function type(filename)
fp = fopen(filename,'r');
if (fp == -1)
  f = which(filename);
  if (isempty(f)), return; end
  filename = f;
  fp = fopen(filename,'r');
end
if (fp == -1), return; end
while (~feof(fp))
  printf('%s',fgetline(fp));
end
fclose(fp);
