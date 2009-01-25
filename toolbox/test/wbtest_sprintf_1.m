function test_val = wbtest_sprintf_1(verbose)
  load reference/wbinputs.mat
  load reference/wbtest_sprintf_1_ref.mat
  fail_count = 0;
  error_flag = 0;
  y1 = [];
  try
    y1=sprintf('hello %d',5);
  catch
    error_flag = 1;
  end
  if (error_flag && ~error_refs)
     printf('Mismatch Errors: y1=sprintf('hello %d',5)\n');
     fail_count = fail_count + 1;
  elseif (~error_flag && ~error_refs && ~wbtest_exact(y1,y1_refs{1}))
    printf('Mismatch (exact): output 1 y1=sprintf('hello %d',5)\n');
    fail_count = fail_count + 1;
  end
  test_val = (fail_count == 0);
end
