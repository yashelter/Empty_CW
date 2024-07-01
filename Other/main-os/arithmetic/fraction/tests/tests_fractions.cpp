#include <fraction.h>
#include <iostream>

using std::cout;
using std::endl;

int main(int argc,char *argv[])
{
	fraction eps (1_bi, 100_bi);

	fraction a(big_int("1"), big_int("2"));
	fraction b(big_int("12"), big_int("16"));
	cout << a + b << std::endl;
	cout << a - b << std::endl;
	cout << a * b << std::endl;
	cout << a / b << std::endl;

	if (a > b)
	{
		if (a != b)
			cout << a << " greater than " << b << endl;
	}
	else
	{
		cout << b << " greater than " << a << endl;
	}

	a = fraction(big_int("1"), big_int("10"));
	b = fraction(big_int("-121"), big_int("-1210"));
	if (a == b)
	{
		cout << "a and b are equal" << endl;
	}
	try
	{
		a = fraction(big_int("1"), big_int("0"));
		b = fraction(big_int("-121"), big_int("-1210"));
		a += b;
	}catch (std::invalid_argument &ex)
	{
		cout << "Getted error : \n";
		cout << ex.what() << endl;
	}
	//std::cin >> a;

	cout << a << endl;
	std::cout << "\n------------------------------------------\n";

	fraction f (big_int("271"), big_int("100"));
	std::cout << (f.pow(2)).ln(eps) << std::endl;

	fraction g (big_int("10"), big_int("1"));
	std::cout << g.lg(eps) << std::endl;

	a = fraction(big_int("1"), big_int("2"));
	cout << a.cos(eps) << endl;

	a = fraction(big_int("1"), big_int("2"));
	cout << a.arccos(eps) << endl;

	a = fraction(big_int("1"), big_int("2"));
	cout << a.tg(eps) << endl;

	a = fraction(big_int("1"), big_int("2"));
	cout << a.sec(eps) << endl;

	a = fraction(big_int("1"), big_int("2"));
	cout << a.cosec(eps) << endl;

	a = fraction(big_int("1"), big_int("2"));
	cout << a.arccosec(eps) << endl;

	a = fraction(big_int("1"), big_int("2"));
	cout << a.arctg(eps) << endl;

	a = fraction(big_int("1"), big_int("2"));
	cout << a.arcctg(eps) << endl;

	a = fraction(big_int("1"), big_int("2"));
	cout << a.sin(eps) << endl;

	a = fraction(big_int("1"), big_int("2"));
	cout << a.arcsin(eps) << endl;

	a = fraction(big_int("1"), big_int("2"));
	cout << a.cos(eps) << endl;

	a = fraction(big_int("1"), big_int("2"));
	cout << a.arccos(eps) << endl;

}