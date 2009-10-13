function test_val = wbtest_det_1(verbose)
  myloc=fileparts(which('wbtest_det_1'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  load([myloc,dirsep,'reference',dirsep,'wbtest_det_1_ref.mat'])
  fail_count = 0;
  for loopi=1:numel(wbinputs)
    x1 = wbinputs{loopi};
    error_flag = 0;
  y1 = [];
    try
      y1=det(x1);
    catch
      error_flag = 1;
    end
    if (error_flag && ~error_refs(loopi) && ~(false))
       printf('Mismatch Errors: input %d y1=det(x1)\n',loopi);
       fail_count = fail_count + 1;
    elseif (~error_flag && ~error_refs(loopi) && ~wbtest_near(y1,y1_refs{loopi}) && ~(false))
      printf('Mismatch (near): input %d output 1 y1=det(x1)\n',loopi);
      fail_count = fail_count + 1;
    end
  test_val = (fail_count == 0);
end
