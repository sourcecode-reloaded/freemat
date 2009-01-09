%!
%@Module HEX2DEC Convert Hexadecimal Numbers To Decimal
%@@Section ELEMENTARY
%@@Usage
%Converts a hexadecimal number (encoded as a string matrix) into integers.
%The syntax for its use is
%@[
%   y = hex2dec(x)
%@]
%where @|x| is a character matrix where each row represents an integer
%in hexadecimal form.  The output is of type @|FM_DOUBLE|.
%@@Examples
%@<
%hex2dec('3ff')
%@>
%Or for a more complex example
%@<
%hex2dec(['0ff';'2de';'123'])
%@>
%@@Tests
%@$exact|y1=hex2dec(x1)
%!

function y=hex2dec( s )

d = abs( lower( s ) );

if( any( ~((d >= 'a' && d <= 'f') || ( d >= '0' && d <= '9' ) )) )
	error('only characters 0..9 and a..f are allowed')
end
di =( d >= '0' && d <= '9' );
d( di ) = d( di ) - '0';
hi = ( d >= 'a' && d <= 'f' );
d( hi ) = d( hi ) - 'a' + 10;

ex=16.^(length(d)-1:-1:0);
y=d*(ex');
