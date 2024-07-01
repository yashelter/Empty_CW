#ifndef MP_OS_FRACTION_H
#define MP_OS_FRACTION_H

#include <big_int.h>

class fraction final
{

public:

	big_int _numerator;
	big_int _denominator;

public:

    fraction(
		big_int &&numerator,
		big_int &&denominator);



public:

    fraction &operator+=(fraction const &other);
    fraction operator+(fraction const &other) const;
    fraction &operator-=(fraction const &other);
    fraction operator-(fraction const &other) const;
    fraction &operator*=(fraction const &other);
    fraction operator*(fraction const &other) const;
    fraction &operator/=(fraction const &other);
    fraction operator/(fraction const &other) const;

public:

    bool operator==(fraction const &other) const;
    bool operator!=(fraction const &other) const;

public:

    bool operator>=(fraction const &other) const;
    bool operator>(fraction const &other) const;
    bool operator<=(fraction const &other) const;
    bool operator<(fraction const &other) const;

public:

    friend std::ostream &operator<<(
        std::ostream &stream,
        fraction const &obj);

    friend std::istream &operator>>(
        std::istream &stream,
        fraction &obj);

public:
	fraction abs() const;
    fraction sin(fraction const &epsilon) const;
    fraction cos(fraction const &epsilon) const;
    fraction tg(fraction const &epsilon) const;
    fraction ctg(fraction const &epsilon) const;
    fraction sec(fraction const &epsilon) const;
    fraction cosec(fraction const &epsilon) const;
    fraction arcsin(fraction const &epsilon) const;
    fraction arccos(fraction const &epsilon) const;
    fraction arctg(fraction const &epsilon) const;
    fraction arcctg(fraction const &epsilon) const;
    fraction arcsec(fraction const &epsilon) const;
    fraction arccosec(fraction const &epsilon) const;

public:
    fraction pow(size_t degree) const;
private:

	static void simplify(fraction &f);
	static big_int find_denominator(const fraction &a, const fraction &b);

public:

    fraction root(
        size_t degree,
        fraction const &epsilon) const;

public:

    fraction log2(fraction const &epsilon) const;

    fraction ln(fraction const &epsilon) const;

    fraction lg(fraction const &epsilon) const;

};

#endif //MP_OS_FRACTION_H