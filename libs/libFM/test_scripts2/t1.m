function y = t1(a,b,c)
  d = a + 3
  global e g h
  persistent k j i
  for i=1:1:100
    k = k + i
  end
  function z = t2(x)
    z = x * d
  end
  [a4,b4,c4] = svd(rand)
end