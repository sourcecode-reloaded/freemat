function test_val = wbtest_find_7(verbose)
  load reference/wbinputs.mat
  load reference/wbtest_find_7_ref.mat
  fail_count = 0;
  for loopi=1:numel(wbinputs)
    x1 = wbinputs{loopi};
    error_flag = 0;
  y1 = [];
  y2 = [];
  y3 = [];
    try
      [y1,y2,y3]=find(x1);
    catch
      error_flag = 1;
    end
    if (error_flag && ~error_refs(loopi))
       printf('Mismatch Errors: input %d [y1,y2,y3]=find(x1)\n',loopi);
        fail_count = fail_count + 1;
  elseif (~error_flag && ~error_refs(loopi) && ~wbtest_exact(y1,y1_refs{loopi}))
    printf('Mismatch (exact): input %d output 1 [y1,y2,y3]=find(x1)\n',loopi);
    fail_count = fail_count + 1;
  end
  elseif (~error_flag && ~error_refs(loopi) && ~wbtest_exact(y2,y2_refs{loopi}))
    printf('Mismatch (exact): input %d output 2 [y1,y2,y3]=find(x1)\n',loopi);
    fail_count = fail_count + 1;
  end
  elseif (~error_flag && ~error_refs(loopi) && ~wbtest_exact(y3,y3_refs{loopi}))
    printf('Mismatch (exact): input %d output 3 [y1,y2,y3]=find(x1)\n',loopi);
    fail_count = fail_count + 1;
  end
  test_val = (fail_count == 0);
end
