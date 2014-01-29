function result = test_line_linestyle_001
a = figure;
p = plot(randn(100,1));
linestyles = {'-','--',':','-.','none'};
result = false;
for i = 1:numel(linestyles)
  try
    set(p,'linestyle',linestyles{i});
    draw
  catch
    lasterr
    return 
  end
end
close(a)
result = true;
