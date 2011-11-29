% Regression test function (black box) for struct
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_variables_struct
  bbtest_success = 1;
NumErrors = 0;
try
  y = struct('foo',{1,3,4},'bar',{'cheese','cola','beer'},'key',508)
catch
  NumErrors = NumErrors + 1;
end
try
  y(1)
catch
  NumErrors = NumErrors + 1;
end
try
  y(2)
catch
  NumErrors = NumErrors + 1;
end
try
  y(3)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  Test(2,3).Type = 'Beer';
catch
  NumErrors = NumErrors + 1;
end
try
  Test(2,3).Ounces = 12;
catch
  NumErrors = NumErrors + 1;
end
try
  Test(2,3).Container = 'Can';
catch
  NumErrors = NumErrors + 1;
end
try
  Test(2,3)
catch
  NumErrors = NumErrors + 1;
end
try
  Test(1,1)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
