% Regression test function (black blox) for FreeMat v svn
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_fullfile
  bbtest_success = 1;
NumErrors = 0;
try
  fullfile('path','to','my','file.m')

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
