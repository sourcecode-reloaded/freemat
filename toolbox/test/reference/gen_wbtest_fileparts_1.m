function gen_wbtest_fileparts_1(verbose)
  load wbinputs.mat
  error_refs = 0;
  y1 = [];
  y2 = [];
  y3 = [];
  y4 = [];
  try
    [y1,y2,y3,y4]=fileparts('/home/basu/testfile.mgz');
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  y2_refs = {y2};
  y3_refs = {y3};
  y4_refs = {y4};
  end
  save wbtest_fileparts_1_ref.mat error_refs  y1_refs   y2_refs   y3_refs   y4_refs 
end
