% Regression test function (black box) for getpath
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_os_getpath
  bbtest_success = 1;
NumErrors = 0;
try
  getpath
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
