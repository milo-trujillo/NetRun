#include <iostream>

using namespace std;

//
// int_to_string - Convert an integer to a C++ string
//
// Quick and dirty, we go over an integer and convert every digit to a char,
// then append those to a string before returning it.
//
string int_to_string( int given )
{
	string buffer = "";
	if( given < 0 )
	{
		buffer.push_back('-');
		given *= -1;
	}
	while( given > 0 )
	{
		int x = given % 10;
		char foo = '0' + x;
		buffer.push_back(foo);
		given /= 10;
	}
	string buffer2;
	for( int i = buffer.length() - 1; i >= 0; i-- )
	{
		buffer2.push_back( buffer.at(i) );
	}
	return buffer2;
}
