classdef mab
    properties
        mab1
        mab2 = 7
    end
    methods
        function obj = mab(val)
            obj.mab1 = val + obj.mab2
        end
        function disp(x)
            print('Hello\n');
        end
        function x = hook(x,n)
            x.mab1 = n*8;
        end
        function y = closer(x,n)
            function h = adder(m)
                h = m + x.mab1;
            end
            y = @adder;
        end
    end
end
