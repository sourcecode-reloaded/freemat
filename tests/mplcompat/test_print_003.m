function result = test_print_003
result = false;
a = figure;
surf(peaks);
print test3.jpg
print test3.png
print test3.pdf
print test3.svg
close(a);
result = true;
