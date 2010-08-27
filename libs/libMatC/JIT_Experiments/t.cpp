#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <QtCore/QVector>
//#include <QtCore/QtCore>

//#include "mytestheader"

int blah = 10;
bool blah1 = false;
double blah2 = 100.e3;

class A{
	private:
		bool b;
	public:
		A(){ b = true; }
		int a_1( bool c ){ return 1; }
};

extern "C" int f_t( void )
{
//	int a = 42;
	int a = atoi("42");
	printf("%d\n", a);
	return a;
}

bool f_1( int a )
{
	return a;
}

int main( void )
{
	A c;
	c.a_1( true );
	return f_t();
}
