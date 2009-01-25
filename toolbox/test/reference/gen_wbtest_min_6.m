function gen_wbtest_min_6(verbose)
  load wbinputs.mat
  n_ = numel(wbinputs);
  error_refs = zeros(n_,n_);
  y1_refs = cell(n_,n_);
  y2_refs = cell(n_,n_);
  for loopi=1:n_
    for loopj=1:n_
      x1 = wbinputs{loopi};
      x2 = wbinputs{loopj};
      y1 = [];
      y2 = [];
      try
        [y1,y2]=min(x1,x2);
      catch
        error_refs(loopi,loopj) = 1;
      end
      if (~error_refs(loopi,loopj))
       y1_refs(loopi,loopj) = {y1};
       y2_refs(loopi,loopj) = {y2};
      end
    end
  end
  save wbtest_min_6_ref.mat error_refs  y1_refs   y2_refs 
end
