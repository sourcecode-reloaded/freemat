function test_val = wbtest_ipermute_2(verbose)
  load reference/wbinputs.mat
  load reference/wbtest_ipermute_2_ref.mat
  fail_count = 0;
  for loopi=1:numel(wbinputs)
    x1 = wbinputs{loopi};
    error_flag = 0;
  y1 = [];
    try
      y1=size(ipermute(permute(x1),[3,4,2,1]),[3,4,2,1]);
    catch
      error_flag = 1;
    end
    if (error_flag && ~error_refs(loopi))
       printf('Mismatch Errors: input %d y1=size(ipermute(permute(x1),[3,4,2,1]),[3,4,2,1])\n',loopi);
        fail_count = fail_count + 1;
  elseif (~error_flag && ~error_refs && ~wbtest_exact(y1,y1_refs{loopi}))
    printf('Mismatch (exact): input %d output 1 y1=size(ipermute(permute(x1),[3,4,2,1]),[3,4,2,1])\n',loopi);
    fail_count = fail_count + 1;
  end
  test_val = (fail_count == 0);
end
