% Regression test function (black blox) for FreeMat v4.0
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_strcmp
  bbtest_success = 1;
NumErrors = 0;
try
  x1 = 'astring';

catch
  NumErrors = NumErrors + 1;
end
try
  x2 = 'bstring';

catch
  NumErrors = NumErrors + 1;
end
try
  x3 = 'astring';

catch
  NumErrors = NumErrors + 1;
end
try
  strcmp(x1,x2)

catch
  NumErrors = NumErrors + 1;
end
try
  strcmp(x1,x3)

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  x = {'astring','bstring',43,'astring'}

catch
  NumErrors = NumErrors + 1;
end
try
  p = strcmp(x,'astring')

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  strcmp({'this','is','a','pickle'},{'what','is','to','pickle'})

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  strcmp({'this','is','a','pickle'},['peter ';'piper ';'hated ';'pickle'])

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
