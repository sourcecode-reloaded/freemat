function wbtestcompare(fname_input,fname_base,fname_ref)
  diary wbtest.txt
  load(fname_input);
  load(fname_base);
  outputs_base = outputs;
  recs_base = recs;
  load(fname_ref);
  outputs_ref = outputs;
  recs_ref = recs;
  e1count = 0;
  e2count = 0;
  for ndx=1:size(recs_ref,1);
    rec_ref = recs_ref(ndx,:);
    rec_base = recs_base(ndx,:);
    expr = exprs{rec_ref(4)}; expr(end) = [];
    if (rec_ref(5) && ~rec_base(5))
      printf('Failed: %d expr: %s inputs %d %d\n',ndx,expr,rec_ref(1),rec_ref(2));
      e1count = e1count + 1;
    elseif (~rec_ref(5) && ~rec_base(5))
      if (~issame(outputs_base{rec_base(6)},outputs_ref{rec_ref(6)}))
        printf('Mismatch: %d expr: %s inputs %d %d outputs %d %d\n',ndx,expr,rec_ref(1),rec_ref(2),rec_ref(6),rec_base(6));
        e2count = e2count + 1;
      end
    end
  end
  diary off
