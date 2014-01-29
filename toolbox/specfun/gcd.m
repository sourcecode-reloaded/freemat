function y = gcd(a,b)
  if (isreal(a) && isreal(b))
    y = gcd_real_case(a,b);
  else
    y = gcd_complex_case(a,b);
  end
end

function y = gcd_real_case(a,b)
  a = abs(a);
  b = abs(b);
  if (any(a ~= fix(a))) error('gcd arguments must be integers'); end
  if (any(b ~= fix(b))) error('gcd arguments must be integers'); end
  if (isscalar(a)) a = repmat(a,size(b)); end
  if (isscalar(b)) b = repmat(b,size(a)); end
  if (numel(a) ~= numel(b)) error('gcd arguments must be the same size or a scalar/vector pair'); end
  y = zeros(size(a));
  n = numel(a);
  for i=1:n
    av = a(i);
    bv = b(i);
    while (bv ~= 0)
      tv = rem(av,bv);
      av = bv;
      bv = tv;
    end
    y(i) = av;
  end
end

function y = gcd_complex_case(a,b)
  if (any(real(a) ~= fix(real(a)))) error('gcd arguments must be integers for real and complex parts'); end
  if (any(imag(a) ~= fix(imag(a)))) error('gcd arguments must be integers for real and complex parts'); end
  if (any(real(b) ~= fix(real(b)))) error('gcd arguments must be integers for real and complex parts'); end
  if (any(imag(b) ~= fix(imag(b)))) error('gcd arguments must be integers for real and complex parts'); end
  if (isscalar(a)) a = repmat(a,size(b)); end
  if (isscalar(b)) b = repmat(b,size(a)); end
  if (numel(a) ~= numel(b)) error('gcd arguments must be the same size or a scalar/vector pair'); end
  y = zeros(size(a));
  n = numel(a);
  for i=1:n
    av = a(i);
    bv = b(i);
    if (abs(av) < abs(bv))
       t = av;
       av = bv;
       bv = t;
    end
    while (abs(bv) ~= 0)
      tv = rem(av,bv);
      av = bv;
      bv = tv;
    end
    y(i) = av;
  end
end
  
