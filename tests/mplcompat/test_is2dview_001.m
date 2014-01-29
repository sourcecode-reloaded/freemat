function result = test_is2dview_001
result = false;
a = figure;
imagesc(peaks,[0,5],'visible','off');
result = (is2dview == true);
close(a);

