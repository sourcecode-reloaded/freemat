function test_val = wbtest_rref_3(verbose)
  load reference/wbinputs.mat
  load reference/wbtest_rref_3_ref.mat
  fail_count = 0;
  for loopi=1:numel(wbinputs)
    x1 = wbinputs{loopi};
    error_flag = 0;
  y1 = [];
  y2 = [];
    try
      [y1,y2]=rref(x1,.0001);
    catch
      error_flag = 1;
    end
    if (error_flag && ~error_refs(loopi))
       printf('Mismatch Errors: input %d [y1,y2]=rref(x1,.0001)\n',loopi);
        fail_count = fail_count + 1;
  elseif (~error_flag && ~error_refs(loopi) && ~wbtest_near(y1,y1_refs{loopi}))
    printf('Mismatch (near): input %d output 1 [y1,y2]=rref(x1,.0001)\n',loopi);
    fail_count = fail_count + 1;
  end
  elseif (~error_flag && ~error_refs(loopi) && ~wbtest_near(y2,y2_refs{loopi}))
    printf('Mismatch (near): input %d output 2 [y1,y2]=rref(x1,.0001)\n',loopi);
    fail_count = fail_count + 1;
  end
  test_val = (fail_count == 0);
end