classdef foo < handle
  properties
      foo1
      foo2
  end
  methods
      function obj = foo
          obj.foo1 = 'constructed';
      end
      function delete(obj)
          'bye!'
      end
  end
end
