classdef bar < mab & foo & handle
    properties
        bar1 = 3
        bar2 = 7
    end
    events
       TestEvent
    end
    methods
        function x = bar(n)
          x = x@mab(n)
        end
        function disp(x)
            mab@disp(x)
            print('there\n');
        end
        function obj = fbar1(obj,n)
            obj.bar1 = obj.bar1 + n
        end
    end
end
