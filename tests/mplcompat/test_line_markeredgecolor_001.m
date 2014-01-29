function result = test_line_markeredgecolor_001
a = figure;
p = plot(randn(100,1),'-s');
colornames = {'none','y','yellow','m','magenta','c','cyan','r','red','g','green','b','blue','w','white','k','black'};
result = false;
for i = 1:numel(colornames)
  try
    set(p,'markeredgecolor',colornames{i});
    draw
  catch
    lasterr
    return 
  end
end
close(a)
result = true;
