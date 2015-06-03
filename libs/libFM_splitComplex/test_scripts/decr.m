function [obj,x] = decr(obj,val,amount)
  obj.length = obj.length - val + amount;
  x = amount;