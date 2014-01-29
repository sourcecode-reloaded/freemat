function result = test_print_002
result = false;
a = figure;
image(peaks);
print test2.jpg
print test2.png
print test2.pdf
print test2.svg
close(a);
result = true;
