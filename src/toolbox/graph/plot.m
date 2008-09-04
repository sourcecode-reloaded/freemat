%!
%@Module PLOT Plot Function
%@@Section HANDLE
%@@Usage
%This is the basic plot command for FreeMat.  The general syntax for its
%use is
%@[
%  plot(<data 1>,{linespec 1},<data 2>,{linespec 2}...,properties...)
%@]
%where the @|<data>| arguments can have various forms, and the
%@|linespec| arguments are optional.  We start with the
%@|<data>| term, which can take on one of multiple forms:
%\begin{itemize}
%  \item Vector Matrix Case -- In this case the argument data is a pair
%    of variables.  A set of @|x| coordinates in a numeric vector, and a 
%    set of @|y| coordinates in the columns of the second, numeric matrix.
%    @|x| must have as many elements as @|y| has columns (unless @|y|
%    is a vector, in which case only the number of elements must match).  Each
%    column of @|y| is plotted sequentially against the common vector @|x|.
%  \item Unpaired Matrix Case -- In this case the argument data is a 
%    single numeric matrix @|y| that constitutes the @|y|-values
%    of the plot.  An @|x| vector is synthesized as @|x = 1:length(y)|,
%    and each column of @|y| is plotted sequentially against this common @|x|
%    axis.
%  \item Complex Matrix Case -- Here the argument data is a complex
%    matrix, in which case, the real part of each column is plotted against
%    the imaginary part of each column.  All columns receive the same line
%    styles.
%\end{itemize}
%Multiple data arguments in a single plot command are treated as a \emph{sequence}, meaning
%that all of the plots are overlapped on the same set of axes.
%The @|linespec| is a string used to change the characteristics of the line.  In general,
%the @|linespec| is composed of three optional parts, the @|colorspec|, the 
%@|symbolspec| and the @|linestylespec| in any order.  Each of these specifications
%is a single character that determines the corresponding characteristic.  First, the 
%@|colorspec|:
%\begin{itemize}
%  \item @|'r'| - Color Red
%  \item @|'g'| - Color Green
%  \item @|'b'| - Color Blue
%  \item @|'k'| - Color Black
%  \item @|'c'| - Color Cyan
%  \item @|'m'| - Color Magenta
%  \item @|'y'| - Color Yellow
%\end{itemize}
%The @|symbolspec| specifies the (optional) symbol to be drawn at each data point:
%\begin{itemize}
%  \item @|'.'| - Dot symbol
%  \item @|'o'| - Circle symbol
%  \item @|'x'| - Times symbol
%  \item @|'+'| - Plus symbol
%  \item @|'*'| - Asterisk symbol
%  \item @|'s'| - Square symbol
%  \item @|'d'| - Diamond symbol
%  \item @|'v'| - Downward-pointing triangle symbol
%  \item @|'^'| - Upward-pointing triangle symbol
%  \item @|'<'| - Left-pointing triangle symbol
%  \item @|'>'| - Right-pointing triangle symbol
%\end{itemize}
%The @|linestylespec| specifies the (optional) line style to use for each data series:
%\begin{itemize}
%  \item @|'-'| - Solid line style
%  \item @|':'| - Dotted line style
%  \item @|'-.'| - Dot-Dash-Dot-Dash line style
%  \item @|'--'| - Dashed line style
%\end{itemize}
%For sequences of plots, the @|linespec| is recycled with color order determined
%by the properties of the current axes.  You can also use the @|properties|
%argument to specify handle properties that will be inherited by all of the plots
%generated during this event.  Finally, you can also specify the handle for the
%axes that are the target of the @|plot| operation.
%@[
   %  handle = plot(handle,...)
%@]
%@@Example
%The most common use of the @|plot| command probably involves the vector-matrix
%paired case.  Here, we generate a simple cosine, and plot it using a red line, with
%no symbols (i.e., a @|linespec| of @|'r-'|).
%@<
%x = linspace(-pi,pi);
%y = cos(x);
%plot(x,y,'r-');
%mprint plot1
%@>
%which results in the following plot.
%@figure plot1
%
%Next, we plot multiple sinusoids (at different frequencies).  First, we construct
%a matrix, in which each column corresponds to a different sinusoid, and then plot
%them all at once.
%@<
%x = linspace(-pi,pi);
%y = [cos(x(:)),cos(3*x(:)),cos(5*x(:))];
%plot(x,y);
%mprint plot2
%@>
%In this case, we do not specify a @|linespec|, so that we cycle through the
%colors automatically (in the order listed in the previous section).
%@figure plot2
%
%This time, we produce the same plot, but as we want to assign individual
%@|linespec|s to each line, we use a sequence of arguments in a single plot
%command, which has the effect of plotting all of the data sets on a common 
%axis, but which allows us to control the @|linespec| of each plot. In 
%the following example, the first line (harmonic) has red, solid lines with 
%times symbols
%marking the data points, the second line (third harmonic) has blue, solid lines
%with right-pointing triangle symbols, and the third line (fifth harmonic) has
%green, dotted lines with asterisk symbols.
%@<
%plot(x,y(:,1),'rx-',x,y(:,2),'b>-',x,y(:,3),'g*:');
%mprint plot3
%@>
%@figure plot3
%
%The second most frequently used case is the unpaired matrix case.  Here, we need
%to provide only one data component, which will be automatically plotted against
%a vector of natural number of the appropriate length.  Here, we use a plot sequence
%to change the style of each line to be dotted, dot-dashed, and dashed.
%@<
%plot(y(:,1),'r:',y(:,2),'b;',y(:,3),'g|');
%mprint plot4
%@>
%Note in the resulting plot that the @|x|-axis no longer runs from @|[-pi,pi]|, but 
%instead runs from @|[1,100]|.
%@figure plot4
%
%The final case is for complex matrices.  For complex arguments, the real part is
%plotted against the imaginary part.  Hence, we can generate a 2-dimensional plot
%from a vector as follows.
%@<
%y = cos(2*x) + i * cos(3*x);
%plot(y);
%mprint plot5
%@>
%@figure plot5
%
%Here is an example of using the handle properties to influence the behavior
%of the generated lines.
%@<
%t = linspace(-3,3);
%plot(cos(5*t).*exp(-t),'r-','linewidth',3);
%mprint plot6
%@>
%@figure plot6
%@@Tests
%@{ test_plot1.m
%% test plot of an empty argument
%function test_val = test_plot1
%a = [];
%try
%  % If this causes a segfault, it won't be caught.
%  plot(a,a);
%catch
%end
%close all;
%test_val = 1;
%@}
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function ohandle = plot(varargin)
   % Check for an axes handle
   if (nargin>=2)
      if (isnumeric(varargin{1}) & (length(varargin{1})==1) & ...
         ishandle(varargin{1},'axes'))
         handle = varargin{1}(1);
         varargin(1) = [];
         nargin = nargin - 1;
      else   
         handle = newplot;
      end
   else
      handle = newplot;
   end
   saveca = gca;
   axes(handle);
   % search for the propertyname/value pairs
   propstart = 0;
   if (nargin > 2)
      propstart = nargin-1;
      while ((propstart >= 1) & isa(varargin{propstart},'string') & ...
         pvalid('line',varargin{propstart}))
         propstart = propstart - 2;
      end
      propstart = propstart + 2;
   end
   propset = {};
   if ((propstart > 0) & (propstart < nargin))
	propset = varargin(propstart:end);
	varargin(propstart:end) = [];
   end
   h = [];
   while (~isempty(varargin))
      cs = ''; ms = ''; ps = '';
      if (length(varargin) == 1)
         h = [h,plot_single(varargin{1},handle,propset)];
         varargin(1) = [];
      elseif (islinespec(varargin{2},cs,ms,ps))
         h = [h,plot_single(varargin{1},handle,completeprops(cs,ms,ps,propset))];
         varargin(1:2) = [];
      elseif (length(varargin) ==2)
         h = [h,plot_double(varargin{1},varargin{2},handle,propset)];
         varargin(1:2) = [];
      elseif (islinespec(varargin{3},cs,ms,ps))
         h = [h,plot_double(varargin{1},varargin{2},handle,...
         completeprops(cs,ms,ps,propset))];
         varargin(1:3) = [];
      else
         h = [h,plot_double(varargin{1},varargin{2},handle,propset)];
         varargin(1:2) = [];
      end
   end
   axes(saveca);
   if (nargout > 0)
     ohandle = h;
   end
   
function h = plot_single(Y,handle,lineprops)
   h = [];
   if (isvector(Y)) Y = Y(:); end;
   if (isreal(Y))
      n = 1:size(Y,1);
      for i=1:size(Y,2)
         h = [h,tplotvector(handle,n,Y(:,i),lineprops)];
      end
   else
      for i=1:size(Y,2)
         h = [h,tplotvector(handle,real(Y(:,i)),imag(Y(:,i)),lineprops)];
      end      
   end
   
function h = plot_double(X,Y,handle,lineprops)
   h = [];
   [X,Y]=matchmat(X,Y);
   if (isvector(X)) X = X(:); end;
   if (isvector(Y)) Y = Y(:); end;
   for i=1:size(Y,2)
      h = [h,tplotvector(handle,X(:,i),Y(:,i),lineprops)];
   end
   
function [a,b] = matchmat(a,b)
   if (isvector(a) & ~isvector(b))
      if (length(a)==size(b,1))
         a = repmat(a(:),[1,size(b,2)]);
         return
      else
         if (length(a) == size(b,2))
            b=b';
            a = repmat(a(:)',[size(b,2),1])';
            return
         else
            error('plot(X,Y) dimensions do not match');
         end
      end
   end
      
   if (~isvector(a) & isvector(b))
      if (length(b) == size(a,1))
         b = repmat(b(:),[1,size(a,2)]);
      else
         if (length(b) == size(a,2))
            a=a';
            b = repmat(b(:)',[size(a,2),1])';
         else
            error('plot(X,Y) where one argument is a vector requires the other argument to have a matching dimension');
         end
      end
   end
   
   
function k = tplotvector(handle,x,y,lineprops)
   ndx = length(get(handle,'children'))+1;
   % Get the colororder
   colororder = get(handle,'colororder');
   % select the row using a modulo
   ndxmod = uint32(mod(ndx-1,size(colororder,1))+1);
   if (~any(strcmp(lineprops,'color')))
     lineprops = [lineprops,{'markeredgecolor',colororder(ndxmod,:),'markerfacecolor',colororder(ndxmod,:)}];
   end
   k = hline('xdata',x,'ydata',y,'color',colororder(ndxmod,:),lineprops{:});
   