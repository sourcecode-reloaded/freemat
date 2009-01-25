function test_val = test_ctype1
  ctypedefine('struct','tstruct','v1','int32','v2','double','v3','float','v4','uint8[20]');
  g.v1 = 54; g.v2 = pi; g.v3 = exp(1); g.v4 = 'hello';
  h = ctypefreeze(g,'tstruct');
  k = ctypethaw(h,'tstruct');
  test_val = issame(k,g);
