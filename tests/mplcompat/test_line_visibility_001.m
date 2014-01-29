function result = test_line_visibility_001
a = figure;
p = plot(randn(100,1));
result = false;
set(p,'visible','on');
draw
if (~strcmp(get(p,'visible'),'on'))
  return
end
set(p,'visible','off');
draw
if (~strcmp(get(p,'visible'),'off'))
  return
end
result = true;
close(a);
return
