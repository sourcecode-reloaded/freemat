function b = excb(x)
  b = x + 2
  x = x - 3
  throw 'Uncaught!'