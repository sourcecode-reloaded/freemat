classdef DocPolynom
   % Documentation example
   % A value class that implements a data type for polynonials
   % See Implementing a Class for Polynomials in the
   % MATLAB documentation for more information.
   
   properties
      coef
   end
   
   % Class methods
   methods
      function obj = DocPolynom(c)
         % Construct a DocPolynom object using the coefficients supplied
         if isa(c,'DocPolynom')
            obj.coef = c.coef;
         else
            obj.coef = c(:).';
         end
      end % DocPolynom
      function obj = set.coef(obj,val)
         if ~isa(val,'double')
            error('Coefficients must be of class double')
         end         
         ind = find(val(:).'~=0);
         if ~isempty(ind);
            obj.coef = val(ind(1):end);
         else
            obj.coef = val;
         end
      end % set.coef
      
      function c = double(obj)
         c = obj.coef;
      end % double
      
      function obj = uminus(obj)
          obj = -1*obj;
      end
      
      function [q,r] = mrdivide(obj1,obj2)
          [q,r] = deconv(obj1.coef,obj2.coef);
          if q ~= 0
            q = DocPolynom(q);
          end
          flag = 0;
          for i=1:length(r)
              if r(i) ~= 0
                  flag = 1;
              end
          end
          if flag == 1
              warning('Polynomial division resulted in a remainder')
              r = DocPolynom(r);
          end
      end
      
      function str = char(obj)
         % Created a formated display of the polynom
         % as powers of x
         if all(obj.coef == 0)
            s = '0';
            str = s;
         else
            d = length(obj.coef)-1;
            s = cell(1,d);
            ind = 1;
            for a = obj.coef;
               if a ~= 0;
                  if ind ~= 1
                     if a > 0
                        s(ind) = {' + '};
                        ind = ind + 1;
                     else
                        s(ind) = {' - '};
                        a = -a; %#ok<FXSET>
                        ind = ind + 1;
                     end
                  end
                  if a ~= 1 || d == 0
                     if a == -1
                        s(ind) = {'-'};
                        ind = ind + 1;
                     else
                        s(ind) = {num2str(a)};
                        ind = ind + 1;
                        if d > 0
                           s(ind) = {'*'};
                           ind = ind + 1;
                        end
                     end
                  end
                  if d >= 2
                     s(ind) = {['x^' int2str(d)]};
                     ind = ind + 1;
                  elseif d == 1
                     s(ind) = {'x'};
                     ind = ind + 1;
                  end
               end
               d = d - 1;
            end
            str = [s{:}];
         end
      end % char
      
      function disp(obj)
         % DISP Display object in MATLAB syntax
         c = char(obj);
         if iscell(c)
            disp(['     ' c{:}])
         else
            disp(c)
         end
      end % disp
      
      function b = subsref(a,s)
         % SUBSREF Implementing the following syntax: 
         % obj([1 ...])
         % obj.coef
         % obj.plot
         % out = obj.method(args)
         % out = obj.method
         switch s(1).type
            case '()'
               ind = s.subs{:};
               b = a.polyval(ind);
            case '.'
               switch s(1).subs
                  case 'coef'
                     b = a.coef;
                  case 'plot'
                     a.plot;
                  otherwise
                     if length(s)>1
                        b = a.(s(1).subs)(s(2).subs{:});
                     else
                        b = a.(s.subs);
                     end
               end
            otherwise
               error('Specify value for x as obj(x)')
         end
      end % subsref
      
      function r = plus(obj1,obj2)
         % PLUS  Implement obj1 + obj2 for DocPolynom
         obj1 = DocPolynom(obj1);
         obj2 = DocPolynom(obj2);
         k = length(obj2.coef) - length(obj1.coef);
         r = DocPolynom([zeros(1,k) obj1.coef] + [zeros(1,-k) obj2.coef]);
      end % plus
      
      function r = mpower(obj1,x)
          obj1 = DocPolynom(obj1);
          if x < 1
              error('DocPolynom can only handle positive, integer powers');
          end
          if (round(x)-x)~= 0
              error('DocPolynom can only handle positive, integer powers');
          end
          r = obj1;
          for i=1:x-1
              r = r*obj1;
          end
      end
      
      function r = minus(obj1,obj2)
         % MINUS Implement obj1 - obj2 for DocPolynoms.
         obj1 = DocPolynom(obj1);
         obj2 = DocPolynom(obj2);
         k = length(obj2.coef) - length(obj1.coef);
         r = DocPolynom([zeros(1,k) obj1.coef] - [zeros(1,-k) obj2.coef]);
      end % minus
      
      function r = mtimes(obj1,obj2)
         % MTIMES   Implement obj1 * obj2 for DocPolynoms.
         obj1 = DocPolynom(obj1);
         obj2 = DocPolynom(obj2);
         r = DocPolynom(conv(obj1.coef,obj2.coef));
      end % mtimes
      
      function r = roots(obj)
         % ROOTS.  ROOTS(obj) is a vector containing the roots of obj.
         r = roots(obj.coef);
      end % roots
      
      function y = polyval(obj,x)
         % POLYVAL  POLYVAL(obj,x) evaluates obj at the points x.
         y = polyval(obj.coef,x);
      end % polyval
      
      function q = diff(obj)
         % DIFF  DIFF(obj) is the derivative of the polynom obj.
         c = obj.coef;
         d = length(c) - 1;  % degree
         q = DocPolynom(obj.coef(1:d).*(d:-1:1));
      end % diff
      
      function plot(obj,varargin)
         % PLOT  PLOT(obj) plots the polynom obj
         if nargin==1
             r = max(abs(roots(obj)));
             x = (-1.1:0.01:1.1)*r;
         else
             x = varargin{1};
         end
         y = polyval(obj,x);
         plot(x,y);
         c = char(obj);
         title(['y = ' c{:}])
         xlabel('X')
         ylabel('Y','Rotation',0)
         grid on
      end % plot
   end % methods 
end % classdef