classdef sclass < handle
    properties
        prop1 = 1
        prop2
    end
    properties (Constant)
        prop3 = 1/2
    end
    properties
        material = 'carbon steel';
        kop = 3 + 42
    end
    properties (Dependent)
        computed
    end
    methods
        function foo = sclass(initial)
            foo.prop1 = initial;
        end
        function delete(obj)
            'fairwell cruel world'
            obj.prop1
            obj.material
            obj.computed
        end
        function obj = incr(obj,amt)
            obj.prop1 = obj.prop1 + amt;
        end
        function x = double(obj)
            x = obj.prop1;
        end
        function val = get.material(obj)
            val = ['carbon 13 ' obj.material obj.kop];
        end
        function val = get.kop(obj)
            val = ' blah';
        end
        function obj = set.prop2(obj,value)
            if (value < 0)
                value = -value;
            end
            obj.prop2 = value;
        end
        function obj = set.prop1(obj,value)
            obj.prop1 = value;
            obj.prop2 = value;
        end
        function val = get.computed(obj)
            val = obj.prop2 * 2;
        end
        function obj = set.computed(obj,val)
            obj.prop2 = val / 2;
        end
    end
    methods (Static = true)
        function x = hello
            x = 'hello there';
        end
    end
end
