function y = anon(a,b,c)
function y = adder(n)
  y = n + a + b + c;
end
y = @adder;
end
