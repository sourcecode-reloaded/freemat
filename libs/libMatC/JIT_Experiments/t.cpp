#include <stdio.h>

int blah = 10;
double blah2 = 100.e3;

class A{
	private:
		int b;
	public:
		A(){ b = 0; }
};

int f_t( void )
{
	int a = 42;
	return a;
}

int main( void )
{
	return f_t();
}
