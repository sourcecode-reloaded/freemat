function y = debugtest(x)
  y = x + 1;
  if (x > 32)
      y = y - 2;
      y = [y,y];
  else
      for i=1:100
          y = y + 1;
          try
              z = x + 4;
              fft(z);
          catch
              q = z + y;
          end
      end
  end
end