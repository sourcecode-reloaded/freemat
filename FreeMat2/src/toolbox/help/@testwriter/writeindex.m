
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function writeindex(&p)
  fname = sprintf('%s/toolbox/test/wb_testmatrix.mat',p.sourcepath);
  recs = p.recs;
  save(fname,'recs');
