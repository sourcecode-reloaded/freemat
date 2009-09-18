function test_val = wbtest_matrixpower_1(verbose)
  load reference/wbinputs.mat
  load reference/wbtest_matrixpower_1_ref.mat
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
    if (error_flag && ~error_refs(loopi,loopj) && ~((any(loopi==[2,18,64,50:52])||(any(loopj==[50:52])))))
       printf('Mismatch Errors: input %d, %d y1=x1^x2\n',loopi,loopj);
        fail_count = fail_count + 1;
  elseif (~error_flag && ~error_refs(loopi,loopj) && ~wbtest_near(y1,y1_refs{loopi,loopj}) && ~((any(loopi==[2,18,64,50:52])||(any(loopj==[50:52])))))
    printf('Mismatch (near): input %d,%d output 1 y1=x1^x2\n',loopi,loopj);
    fail_count = fail_count + 1;
    end
  end
  test_val = (fail_count == 0);
end
