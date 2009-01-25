function test_val = wbtest_svd_2(verbose)
  load reference/wbinputs.mat
  load reference/wbtest_svd_2_ref.mat
  fail_count = 0;
  for loopi=1:numel(wbinputs)
    x1 = wbinputs{loopi};
    error_flag = 0;
  y1 = [];
  y2 = [];
  y3 = [];
    try
      [y1,y2,y3] = svd(x1);
    catch
      error_flag = 1;
    end
    if (error_flag && ~error_refs(loopi))
       printf('Mismatch Errors: input %d [y1,y2,y3] = svd(x1)\n',loopi);
        fail_count = fail_count + 1;
  elseif (~error_flag && ~error_refs && ~wbtest_near(y1,y1_refs{loopi}))
    printf('Mismatch (near): input %d output 1 [y1,y2,y3] = svd(x1)\n',loopi);
    fail_count = fail_count + 1;
  end
  elseif (~error_flag && ~error_refs && ~wbtest_near(y2,y2_refs{loopi}))
    printf('Mismatch (near): input %d output 2 [y1,y2,y3] = svd(x1)\n',loopi);
    fail_count = fail_count + 1;
  end
  elseif (~error_flag && ~error_refs && ~wbtest_near(y3,y3_refs{loopi}))
    printf('Mismatch (near): input %d output 3 [y1,y2,y3] = svd(x1)\n',loopi);
    fail_count = fail_count + 1;
  end
  test_val = (fail_count == 0);
end
