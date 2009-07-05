function test_val = wbtest_fileparts_2(verbose)
  load reference/wbinputs.mat
  load reference/wbtest_fileparts_2_ref.mat
  fail_count = 0;
  error_flag = 0;
  y1 = [];
  y2 = [];
  y3 = [];
  y4 = [];
  try
    [y1,y2,y3,y4]=fileparts('c:\home\basu\testfile.mgz');
  catch
    error_flag = 1;
  end
  if (error_flag && ~error_refs)
     printf('Mismatch Errors: [y1,y2,y3,y4]=fileparts(''c:\home\basu\testfile.mgz'')\n');
     fail_count = fail_count + 1;
  elseif (~error_flag && ~error_refs && ~wbtest_exact(y1,y1_refs{1}))
    printf('Mismatch (exact): output 1 [y1,y2,y3,y4]=fileparts(''c:\home\basu\testfile.mgz'')\n');
    fail_count = fail_count + 1;
  elseif (~error_flag && ~error_refs && ~wbtest_exact(y2,y2_refs{1}))
    printf('Mismatch (exact): output 2 [y1,y2,y3,y4]=fileparts(''c:\home\basu\testfile.mgz'')\n');
    fail_count = fail_count + 1;
  elseif (~error_flag && ~error_refs && ~wbtest_exact(y3,y3_refs{1}))
    printf('Mismatch (exact): output 3 [y1,y2,y3,y4]=fileparts(''c:\home\basu\testfile.mgz'')\n');
    fail_count = fail_count + 1;
  elseif (~error_flag && ~error_refs && ~wbtest_exact(y4,y4_refs{1}))
    printf('Mismatch (exact): output 4 [y1,y2,y3,y4]=fileparts(''c:\home\basu\testfile.mgz'')\n');
    fail_count = fail_count + 1;
  end
  test_val = (fail_count == 0);
end
