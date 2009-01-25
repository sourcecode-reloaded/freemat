% Regression test function (black blox) for FreeMat v svn
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_strfind
  bbtest_success = 1;
NumErrors = 0;
try
  a = 'how now brown cow?'

catch
  NumErrors = NumErrors + 1;
end
try
  b = strfind(a,'ow')

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  a = {'how now brown cow','quick brown fox','coffee anyone?'}

catch
  NumErrors = NumErrors + 1;
end
try
  b = strfind(a,'ow')

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
