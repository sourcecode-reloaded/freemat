function y = merge(mask,tval,fval)
  if (isscalar(mask))
    if (mask) 
      y = tval;
    else
      y = fval;
    end
  else
    y = tval;
    y(~mask) = fval(~mask);
  end
end
