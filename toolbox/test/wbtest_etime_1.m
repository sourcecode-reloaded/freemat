function test_val = wbtest_etime_1(verbose)
  load reference/wbinputs.mat
  load reference/wbtest_etime_1_ref.mat
  fail_count = 0;
  error_flag = 0;
  y1 = [];
  try
    a1=1e3*[2.006,.006,.009,.011,.044,.01907414];a2=1e3*[2.006,.006,.009,.011,.044,.024688253];y1=etime(a2,a1);
  catch
    error_flag = 1;
  end
  if (error_flag && ~error_refs)
     printf('Mismatch Errors: a1=1e3*[2.006,.006,.009,.011,.044,.01907414];a2=1e3*[2.006,.006,.009,.011,.044,.024688253];y1=etime(a2,a1)\n');
     fail_count = fail_count + 1;
  elseif (~error_flag && ~error_refs && ~wbtest_near(y1,y1_refs{1}))
    printf('Mismatch (near): output 1 a1=1e3*[2.006,.006,.009,.011,.044,.01907414];a2=1e3*[2.006,.006,.009,.011,.044,.024688253];y1=etime(a2,a1)\n');
    fail_count = fail_count + 1;
  end
  test_val = (fail_count == 0);
end
