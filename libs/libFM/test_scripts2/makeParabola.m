function p = makeParabola(a,b,c)
    p = @parabola;
    function y = parabola(x)
        y = a*x.*x + b*x + c;
    end
end
