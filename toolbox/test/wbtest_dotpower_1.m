function test_val = wbtest_dotpower_1(verbose)
  load reference/wbinputs.mat
  load reference/wbtest_dotpower_1_ref.mat
  fail_count = 0;
  for loopi=1:numel(wbinputs)
    for loopj=1:numel(wbinputs)
      x1 = wbinputs{loopi};
      x2 = wbinputs{loopj};
      error_flag = 0;
      try
        y=x1.^x2;
      catch
        error_flag = 1;
      end
    if (error_flag && ~error_refs(loopi,loopj))
       printf('Mismatch Errors: input %d, %d y=x1.^x2\n',loopi,loopj);
        fail_count = fail_count + 1;
    end
  end
  test_val = (fail_count == 0);
end
