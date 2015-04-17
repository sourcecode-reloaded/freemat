classdef baz < bar
    properties
        bazbaz
    end
    methods
        function x = baz
          x = x@bar(4)
      end
    end
end