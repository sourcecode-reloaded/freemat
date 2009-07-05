function test_val = wbtest_conv2_2(verbose)
  load reference/wbinputs.mat
  load reference/wbtest_conv2_2_ref.mat
  fail_count = 0;
  for loopi=1:numel(wbinputs)
    for loopj=1:numel(wbinputs)
      x1 = wbinputs{loopi};
      x2 = wbinputs{loopj};
      error_flag = 0;
     y1 = [];
      try
        y1=conv2(x1,x2,'same');
      catch
        error_flag = 1;
      end
    if (error_flag && ~error_refs(loopi,loopj))
       printf('Mismatch Errors: input %d, %d y1=conv2(x1,x2,''same'')\n',loopi,loopj);
        fail_count = fail_count + 1;
  elseif (~error_flag && ~error_refs(loopi,loopj) && ~wbtest_near(y1,y1_refs{loopi,loopj}))
    printf('Mismatch (near): input %d,%d output 1 y1=conv2(x1,x2,''same'')\n',loopi,loopj);
    fail_count = fail_count + 1;
    end
  end
  test_val = (fail_count == 0);
end
