tlist = dir('gen_*.m');
for i=1:numel(tlist)
  [m,n,o,p] = fileparts(tlist(i).name);
  fprintf('Running %s...\n',n);
  try
  feval(n);
  catch
  end
end
