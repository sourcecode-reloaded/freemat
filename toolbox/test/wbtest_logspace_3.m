function test_val = wbtest_logspace_3(verbose)
  myloc=fileparts(which('wbtest_logspace_3'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  load([myloc,dirsep,'reference',dirsep,'wbtest_logspace_3_ref.mat'])
  fail_count = 0;
  error_flag = 0;
  y1 = [];
  try
    y1=logspace(0,pi,3);
  catch
    error_flag = 1;
  end
  if (error_flag && ~error_refs)
     printf('Mismatch Errors: y1=logspace(0,pi,3)\n');
     fail_count = fail_count + 1;
  elseif (~error_flag && ~error_refs && ~wbtest_near(y1,y1_refs{1}))
    printf('Mismatch (near): output 1 y1=logspace(0,pi,3)\n');
    fail_count = fail_count + 1;
  end
  test_val = (fail_count == 0);
end
