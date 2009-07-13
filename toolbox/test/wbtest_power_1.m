function test_val = wbtest_power_1(verbose)
  load reference/wbinputs.mat
  load reference/wbtest_power_1_ref.mat
  fail_count = 0;
  for loopi=1:numel(wbinputs)
    for loopj=1:numel(wbinputs)
      x1 = wbinputs{loopi};
      x2 = wbinputs{loopj};
      error_flag = 0;
     y1 = [];
      try
        y1=x1^x2;
      catch
        error_flag = 1;
      end
    if (error_flag && ~error_refs(loopi,loopj) && ~(false))
       printf('Mismatch Errors: input %d, %d y1=x1^x2\n',loopi,loopj);
        fail_count = fail_count + 1;
        keyboard
  elseif (~error_flag && ~error_refs(loopi,loopj) && ~wbtest_near(y1,y1_refs{loopi,loopj}) && ~(false))
    printf('Mismatch (exact): input %d,%d output 1 y1=x1^x2\n',loopi,loopj);
    fail_count = fail_count + 1;
    keyboard
    end
  end
  test_val = (fail_count == 0);
end
