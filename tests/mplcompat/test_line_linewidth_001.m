function result = test_line_linewidth_001
a = figure;
p = plot(randn(100,1));
linewidths = {0.25,0.5,1,2,4};
result = false;
for i = 1:numel(linewidths)
  try
    set(p,'linewidth',linewidths{i});
    draw
  catch
    lasterr
    return 
  end
end
close(a)
result = true;
