%!
%@Module ISA Test Type of Variable
%@@Section INSPECTION
%@@Usage
%Tests the type of a variable.  The syntax for its
%use is
%@[
%   y = isa(x,type)
%@]
%where @|x| is the variable to test, and @|type| is
%the type.  Supported built-in types are
%\begin{itemize}
%\item @|'cell'| for cell-arrays
%\item @|'struct'| for structure-arrays
%\item @|'logical'| for logical arrays
%\item @|'uint8'| for unsigned 8-bit integers
%\item @|'int8'| for signed 8-bit integers
%\item @|'uint16'| for unsigned 16-bit integers
%\item @|'int16'| for signed 16-bit integers
%\item @|'uint32'| for unsigned 32-bit integers
%\item @|'int32'| for signed 32-bit integers
%\item @|'uint64'| for unsigned 64-bit integers
%\item @|'int64'| for signed 64-bit integers
%\item @|'single'| for 32-bit floating point numbers
%\item @|'double'| for 64-bit floating point numbers
%\item @|'char'| for string arrays
%\end{itemize}
%If the argument is a user-defined type (via the @|class| function), then
%the name of that class is returned.
%@@Examples
%Here are some examples of the @|isa| call.
%@<
%a = {1}
%isa(a,'char')
%isa(a,'cell')
%@>
%Here we use @|isa| along with shortcut boolean evaluation to 
%safely determine if a variable contains the string @|'hello'|
%@<
%a = 'hello'
%isa(a,'char') && strcmp(a,'hello')
%@>
%@@Tests
%@$exact#y1=isa(x1,'single')
%@$exact#y1=isa(x1,'uint8')
%@$exact#y1=isa(x1,'char')
%!

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function y = isa(x,type)
  if (~strcmp(typeof(type),'char'))
    error('type argument to isa must be a string');
  end
  y = strcmp(class(x),type);
