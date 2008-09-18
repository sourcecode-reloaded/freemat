function wbtestcompare(fname_input,fname_base,fname_ref)
  load(fname_input);
  load(fname_base);
  outputs_base = outputs;
  recs_base = recs;
  load(fname_ref);
  outputs_ref = outputs;
  recs_ref = recs;
  ecount = 0;
  for ndx=1:size(recs_ref,1);
    if (recs_ref(ndx,5) && ~recs_base(ndx,5))
      printf('Failed: %d expr: %s inputs %d %d\n',ndx,exprs{recs_ref(ndx,4)},recs_ref(ndx,1),recs_ref(ndx,2));
      ecount = ecount + 1;
    end
  end
  keyboard