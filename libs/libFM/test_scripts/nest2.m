function y = nest2(x)
  z = x*x;
  y = @nest;
 
  function t = nest(a)
      b = add(z,a);
      t = b - x;
  end
end
