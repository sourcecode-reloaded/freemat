function result = test_print_001
result = false;
a = figure;
plot(rand(1,100));
print test1.jpg
print test1.png
print test1.pdf
print test1.svg
close(a);
result = true;
