function result = test_is2dview_002
result = false;
a = figure;
surf(peaks);
result = (is2dview == false);
close(a);

