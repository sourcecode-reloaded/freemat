classdef shnd < handle
    properties
        prop
        prop2 = 5
    end
    events
        trigger
    end
    methods
        function obj=shnd(x)
            obj.prop = x;
        end
        function withdraw(obj,p)
            newbal = obj.prop - p;
            if (newbal < 0) 
                notify(obj,'trigger')
            end
        end
        function p=plus(a,b)
           p = shnd(a.prop + b.prop); 
        end
        function p=horzcat(a,b)
           p = shnd([a.prop,b.prop]);
       end
        function p=vertcat(a,b)
           p = shnd([a.prop;b.prop]);
       end
       function p=polyval(a,t)
           p = a.prop*t;
       end
       function y = subsindex(a)
           y = a.prop-1;
       end
       function a = subsasgn(a,s,b)
           a
           s
           print('b = \n')
           b
           switch s(1).type
             case '()'
               a.prop(s.subs{:}) = b;
             case '.'
               switch s(1).subs
                 case 'coef'
                   a.prop = b;
                 otherwise
                   if (numel(s) > 1)
                       a.(s(1).subs)(s(2).subs{:}) = b;
                   else
                       a.(s.subs) = b;
                   end
               end
           end
       end
        function b = subsref(a,s)
           switch s(1).type
             case '()'
               ind = s.subs{:};
               b = a.polyval(ind);
             case '.'
               switch s(1).subs
                 case 'coef'
                   b = a.prop;
                 otherwise
                   if numel(s)>1
                       b = a.(s(1).subs)(s(2).subs{:});
                   else
                       b = a.(s.subs);
                   end
               end
           end
       end
        function delete(obj)
            'bye'
        end
    end
end