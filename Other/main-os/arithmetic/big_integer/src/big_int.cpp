#include "../include/big_int.h"

std::strong_ordering big_int::operator<=>(const big_int &other) const noexcept
{
	return compare(other, 0);
}

std::strong_ordering big_int::compare(const big_int &other, size_t shift) const noexcept
{
	if (_digits.size() == 0 && other._digits.size() == 0)
		return std::strong_ordering::equal;
	else if (_sign != other._sign)
		return _sign ? std::strong_ordering::greater : std::strong_ordering::less;
	else
	{
		auto res = compare_no_sign(_digits, other._digits, shift);

		if (_sign)
			return res;
		else
		{
			return res == std::strong_ordering::equal ? res : res == std::strong_ordering::less ? std::strong_ordering::greater : std::strong_ordering::less;
		}
	}
}

std::strong_ordering
big_int::compare_no_sign(const std::vector<unsigned int> &lhs, const std::vector<unsigned int> &rhs,
	size_t shift) noexcept
{
	if (lhs.size() != rhs.size() + shift)
	{
		return lhs.size() <=> (rhs.size() + shift);
	} else if (lhs.size() == 0 && rhs.size() == 0)
	{
		return std::strong_ordering::equal;
	} else
	{
		auto lbeg = lhs.rbegin(), lend = lhs.rend(), rbeg = rhs.rbegin(), rend = rhs.rend();

		for(; lbeg < lend; ++lbeg, ++rbeg)
		{
			auto l = *lbeg;
			auto r = rbeg < rend ? *rbeg : 0;

			if (l != r)
				return l <=> r;
		}

		return std::strong_ordering::equal;
	}
}

void big_int::optimise() noexcept
{
	while(!_digits.empty() && _digits.back() == 0)
		_digits.pop_back();

	if (_digits.empty())
		_sign = true;
}

big_int::big_int(const std::vector<unsigned int> &digits, bool sign) : _digits(digits), _sign(sign) {}

big_int::big_int(std::vector<unsigned int> &&digits, bool sign) : _digits(std::move(digits)), _sign(sign) {}

big_int::operator bool()
{
	return !_digits.empty();
}

big_int::big_int() : _digits(), _sign(true) {}

big_int &big_int::operator++()
{
	return operator+=(1);
}


big_int big_int::operator++(int)
{
	auto tmp = *this;
	++(*this);
	return tmp;
}

big_int &big_int::operator--()
{
	return operator-=(1);
}


big_int big_int::operator--(int)
{
	auto tmp = *this;
	--(*this);
	return tmp;
}

big_int &big_int::operator+=(const big_int &other)
{
	return plus_assign(other);
}

big_int &big_int::operator-=(const big_int &other)
{
	return minus_assign(other);
}

big_int big_int::operator+(const big_int &other) const
{
	auto tmp = *this;
	return tmp += other;
}

big_int big_int::operator-(const big_int &other) const
{
	auto tmp = *this;
	return tmp -= other;
}

big_int big_int::operator*(const big_int &other) const
{
	auto tmp = *this;
	return tmp *= other;
}

big_int big_int::operator/(const big_int &other) const
{
	auto tmp = *this;
	return tmp /= other;
}

big_int big_int::operator%(const big_int &other) const
{
	auto tmp = *this;
	return tmp %= other;
}

big_int big_int::operator&(const big_int &other) const
{
	auto tmp = *this;
	return tmp &= other;
}

big_int big_int::operator|(const big_int &other) const
{
	auto tmp = *this;
	return tmp |= other;
}

big_int big_int::operator^(const big_int &other) const
{
	auto tmp = *this;
	return tmp ^= other;
}

big_int big_int::operator<<(size_t shift) const
{
	auto tmp = *this;
	return tmp <<= shift;
}

big_int big_int::operator>>(size_t shift) const
{
	auto tmp = *this;
	return tmp >>= shift;
}

big_int &big_int::operator%=(const big_int &other)
{
	return operator-=((*this / other) * other);
}

big_int big_int::operator~() const
{
	auto res = *this;
	res._sign = !res._sign;
	for (auto& num : res._digits)
	{
		num = ~num;
	}
	res.optimise();
	return res;
}

big_int &big_int::operator&=(const big_int &other)
{
	if (!_sign && other._sign)
		_sign = true;

	for(size_t i = 0; i < _digits.size(); ++i)
		_digits[i] &= i < other._digits.size() ? other._digits[i] : 0;

	optimise();
	return *this;
}

big_int &big_int::operator|=(const big_int &other)
{
	if (_sign && !other._sign)
		_sign = false;

	if (_digits.size() < other._digits.size())
		_digits.resize(other._digits.size(), 0);

	for(size_t i = 0; i < _digits.size(); ++i)
		_digits[i] |= other._digits[i];

	optimise();
	return *this;
}

big_int &big_int::operator^=(const big_int &other)
{
	if (_sign != other._sign)
		_sign = true;
	else
		_sign = false;

	if (_digits.size() < other._digits.size())
		_digits.resize(other._digits.size(), 0);

	for(size_t i = 0; i < _digits.size(); ++i)
		_digits[i] ^= other._digits[i];

	optimise();
	return *this;
}

big_int &big_int::operator<<=(size_t shift)
{
	if (shift / (8 * sizeof(unsigned int)) > 0)
	{
		size_t n = shift / (8 * sizeof(unsigned int));

		std::vector<unsigned int> vec(n, 0);

		_digits.insert(_digits.begin(), vec.begin(), vec.end());

		shift %= 8 * sizeof(unsigned int);
	}

	if (shift == 0)
		return *this;

	unsigned int c = 0;

	for(auto & num : _digits)
	{
		auto tmp = num;
		num = (num << shift) | c;
		c = tmp >> (8 * sizeof(unsigned int) - shift);
	}

	if (c != 0)
		_digits.push_back(c);

	optimise();
	return *this;
}

big_int &big_int::operator>>=(size_t shift)
{
	if (shift / (8 * sizeof(unsigned int)) > 0)
	{
		size_t n = shift / (8 * sizeof(unsigned int));

		if (n > _digits.size())
		{
			_sign = true;
			_digits.clear();
			return *this;
		}

		_digits.erase(_digits.begin(), _digits.begin() + n);

		shift %= 8 * sizeof(unsigned int);
	}

	if (shift == 0)
		return *this;


	unsigned int c = 0;

	for(auto& num : std::views::reverse(_digits))
	{
		auto tmp = num;
		num = (num >> shift) | c;
		c =  tmp << (8 * sizeof(unsigned int) - shift);
	}

	optimise();
	return *this;
}

big_int &big_int::plus_assign(const big_int &other, size_t shift)
{
	if (_sign == other._sign)
	{
		plus_assign_no_sign(_digits, other._digits, shift);
	} else
	{
		auto comp = compare_no_sign(_digits, other._digits, shift);
		if (comp == std::strong_ordering::equal)
		{
			_sign = true;
			_digits.clear();
		} else if (comp == std::strong_ordering::less)
		{
			_sign = !_sign;
			minus_assign_no_sign_reverse(other._digits, _digits, shift);
		} else
		{
			minus_assign_no_sign(_digits, other._digits, shift);
		}
	}

	optimise();
	return *this;
}

big_int &big_int::minus_assign(const big_int &other, size_t shift)
{
	if (_sign != other._sign)
	{
		plus_assign_no_sign(_digits, other._digits, shift);
	} else
	{
		auto comp = compare_no_sign(_digits, other._digits, shift);
		if (comp == std::strong_ordering::equal)
		{
			_sign = true;
			_digits.clear();
		} else if (comp == std::strong_ordering::less)
		{
			_sign = !_sign;
			minus_assign_no_sign_reverse(other._digits, _digits, shift);
		} else
		{
			minus_assign_no_sign(_digits, other._digits, shift);
		}
	}

	optimise();
	return *this;
}

big_int &big_int::operator*=(const big_int &other)
{
	_sign = _sign == other._sign;
	multiply_assign_no_sign(_digits, other._digits);

	optimise();
	return *this;
}

big_int &big_int::operator/=(const big_int &other)
{
	_sign = _sign == other._sign;
	divide_assign_no_sign(_digits, other._digits);

	optimise();
	return *this;
}

void big_int::plus_assign_no_sign(std::vector<unsigned int> &lhs, const std::vector<unsigned int> &rhs, size_t shift)
{
	if (lhs.size() < rhs.size() + shift)
		lhs.resize(rhs.size() + shift, 0);

	unsigned int c = 0;

	for(size_t i = 0; i < lhs.size(); ++i)
	{
		unsigned int a = lhs[i], b = i < shift ? 0 : i - shift < rhs.size() ? rhs[i - shift] : 0;

		unsigned int tmp = (a & mask) + (b & mask) + c;

		c = tmp & (1u << (sizeof(unsigned int) * 4)) ? 1 : 0;

		tmp &= mask;

		unsigned int tmp2 = ((a >> (sizeof(unsigned int) * 4)) & mask) + ((b >> (sizeof(unsigned int) * 4)) & mask) + c;

		c = tmp2 & (1u << (sizeof(unsigned int) * 4)) ? 1 : 0;

		tmp2 &= mask;

		lhs[i] = (tmp2 << (sizeof(unsigned int) * 4)) + tmp;
	}

	if (c != 0)
		lhs.push_back(c);
}

void big_int::minus_assign_no_sign(std::vector<unsigned int> &lhs, const std::vector<unsigned int> &rhs, size_t shift)
{
	if (lhs.size() < rhs.size() + shift)
		lhs.resize(rhs.size() + shift, 0);

	unsigned int c = 0;

	for(size_t i = 0; i < lhs.size(); ++i)
	{
		unsigned int a = lhs[i], b = i < shift ? 0 : i - shift < rhs.size() ? rhs[i - shift] : 0;
		b += c;

		if (c != 0 && b == 0)
			continue;

		c = b > a;

		lhs[i] = a - b;
	}
}

void big_int::minus_assign_no_sign_reverse(const std::vector<unsigned int> &lhs, std::vector<unsigned int> &rhs,
	size_t shift)
{
	if (rhs.size() < lhs.size() + shift)
		rhs.resize(lhs.size() + shift, 0);

	unsigned int c = 0;

	for(size_t i = 0; i < rhs.size(); ++i)
	{
		unsigned int a = i < shift ? 0 : i - shift < lhs.size() ? lhs[i - shift] : 0, b = rhs[i];
		b += c;

		if (c != 0 && b == 0)
		{
			rhs[i] = a;
			continue;
		}

		c = b > a;

		rhs[i] = a - b;
	}
}

bool big_int::need_karatsuba() const noexcept
{
	return true;
}

bool big_int::need_newton() const noexcept
{
	return false;
}

void big_int::multiply_assign_no_sign(std::vector<unsigned int> &lhs, const std::vector<unsigned int> &rhs) const
{
	if (lhs.empty() || rhs.empty())
	{
		lhs.clear();
		return;
	}

	lhs = std::move(need_karatsuba() ? multiply_karatsuba(lhs, rhs) : multiply_common(lhs, rhs));
}

void big_int::divide_assign_no_sign(std::vector<unsigned int> &lhs, const std::vector<unsigned int> &rhs) const
{
	if (rhs.empty())
		throw std::invalid_argument("Division by zero");

	if (lhs.empty() || (rhs.size() == 1 && rhs[0] == 1))
		return;

	if (lhs.size() < rhs.size())
	{
		lhs.clear();
		return;
	}

	lhs = std::move(need_newton() ? divide_newton(lhs, rhs) : divide_common(lhs, rhs));
}

std::vector<unsigned int>
big_int::multiply_common(const std::vector<unsigned int> &lhs, const std::vector<unsigned int> &rhs) const
{
	std::vector<unsigned int> res;

	for(size_t i = 0; i < lhs.size(); ++i)
	{
		for(size_t j = 0; j < rhs.size(); ++j)
		{
			unsigned int a0 = lhs[i] & mask, a1 = (lhs[i] >> (sizeof(unsigned int) * 4)) & mask, b0 = rhs[j] & mask, b1 = (rhs[j] >> (sizeof(unsigned int) * 4)) & mask;
			plus_assign_no_sign(res, summ_four(a0 * b0, a1 * b0, a0 * b1, a1 * b1), i + j);
		}
	}

	return res;
}

std::vector<unsigned int> big_int::summ_four(unsigned int a0b0, unsigned int a1b0, unsigned int a0b1, unsigned int a1b1)
{
	std::vector<unsigned int> fi{a0b0}, se{a1b0 << (sizeof(unsigned int) * 4), a1b0 >> (sizeof(unsigned int) * 4)}, th{a0b1 << (sizeof(unsigned int) * 4), a0b1 >> (sizeof(unsigned int) * 4)},
		fo{0, a1b1};

	plus_assign_no_sign(fi, se);
	plus_assign_no_sign(fi, th);
	plus_assign_no_sign(fi, fo);

	return fi;
}

void big_int::move_caret(std::vector<unsigned int>& vec, unsigned int new_var)
{
	while (!vec.empty() && vec.back() == 0)
		vec.pop_back();
	vec.insert(vec.begin(), new_var);
}

unsigned int big_int::find_quolitent(const std::vector<unsigned int> &numerator, const std::vector<unsigned int> &denominator) const
{
	unsigned int res = 0;
	std::vector<unsigned int> vec;

	for(int i = sizeof(unsigned int) * 8 - 1; i >= 0; --i)
	{
		auto tmp = res;
		res |= 1u << i;
		vec.clear();
		vec.push_back(res);

		multiply_assign_no_sign(vec, denominator);

		while(!vec.empty() && vec.back() == 0)
			vec.pop_back();

		auto comp = compare_no_sign(vec, numerator);

		if (comp == std::strong_ordering::equal)
			return res;
		if (comp == std::strong_ordering::greater)
			res = tmp;
	}

	return res;
}

std::vector<unsigned int>
big_int::divide_common(const std::vector<unsigned int> &lhs, const std::vector<unsigned int> &rhs) const
{
	std::vector<unsigned int> res;
	std::vector<unsigned int> vec(rhs.size() - 1, 0);
	for(size_t i = 0; i < vec.size(); ++i)
	{
		vec[i] = lhs[i + 1 + lhs.size() - rhs.size()];
	}

	for(long long i = lhs.size() - rhs.size(); i >= 0; --i)
	{
		move_caret(vec, lhs[i]);
		unsigned int d = find_quolitent(vec, rhs);
		res.insert(res.begin(), d);
		std::vector<unsigned int> tmp{d};
		multiply_assign_no_sign(tmp, rhs);
		minus_assign_no_sign(vec, tmp);
	}

	return res;
}

big_int::big_int(const std::string &num, unsigned int radix) : _sign(true), _digits()
{
	if (radix > 36 || radix < 2)
		throw std::invalid_argument("Radix must be in interval [2, 36], but is " + std::to_string(radix));

	auto it = num.begin(), end = num.end();

	bool sign = true;

	if (it != end)
	{
		if (*it == '-' || *it == '+')
		{
			sign = *it == '+';
			++it;
		}
		for (; it < end; ++it)
		{
			char sym = *it;
			unsigned int c = sym >= '0' && sym <= '9' ? sym - '0' : tolower(sym) >= 'a' && tolower(sym) <= 'z' ? tolower(sym) - 'a' + 10 : radix + 1;
			if (c >= radix)
				throw std::invalid_argument("Digit must be less then radix");

			std::vector<unsigned int> tmp{radix};

			multiply_assign_no_sign(_digits, tmp);

			tmp[0] = c;

			plus_assign_no_sign(_digits, tmp);

			optimise();
		}
	}
	_sign = sign;
}

std::string big_int::to_string() const
{
	if(_digits.empty())
		return "0";

	std::stringstream res;
	auto tmp = *this;

	bool sign = tmp._sign;

	tmp._sign = true;

	while (tmp > 0_bi)
	{
		auto val = tmp % 10_bi;
		tmp /= 10;
		res << char('0' + (val._digits.empty() ? 0 : val._digits[0]));
	}

	if (!sign)
	{
		res << '-';
	}

	std::string d = res.str();
	std::reverse(d.begin(), d.end());
	return d;
}

std::ostream &operator<<(std::ostream &stream, const big_int &value) noexcept
{
	stream << value.to_string();

	return stream;
}

std::istream &operator>>(std::istream &stream, big_int &value)
{
	std::string val;

	stream >> val;

	value = big_int(val);

	return stream;
}

std::vector<unsigned int>
big_int::multiply_karatsuba(const std::vector<unsigned int> &lhs, const std::vector<unsigned int> &rhs) const
{
	auto n = __detail::nearest_greater_power_of_2(std::max(lhs.size(), rhs.size()));

	if (n == 1)
	{
		unsigned int a = lhs.empty() ? 0 : lhs[0], b = rhs.empty() ? 0 : rhs[0];
		unsigned int a0 = a & mask, a1 = (a >> (sizeof(unsigned int) * 4)) & mask, b0 = b & mask, b1 = (b >> (sizeof(unsigned int) * 4)) & mask;
		return summ_four(a0 * b0, a1 * b0, a0 * b1, a1 * b1);
	}

	std::vector<unsigned int> a0, a1, b0, b1;

	a0.reserve(n/2);
	a1.reserve(n/2);
	b0.reserve(n/2);
	b1.reserve(n/2);

	for(size_t i = 0; i < n/2; ++i)
	{
		if (i < lhs.size())
			a0.push_back(lhs[i]);
		if (i < rhs.size())
			b0.push_back(rhs[i]);
	}
	for(size_t i = n/2; i < n; ++i)
	{
		if (i < lhs.size())
			a1.push_back(lhs[i]);
		if (i < rhs.size())
			b1.push_back(rhs[i]);
	}

	auto a0b0 = multiply_karatsuba(a0, b0);
	auto a1b1 = multiply_karatsuba(a1, b1);
	plus_assign_no_sign(a0, a1);
	plus_assign_no_sign(b0, b1);
	auto other = multiply_karatsuba(a0, b0);
	minus_assign_no_sign(other, a0b0);
	minus_assign_no_sign(other, a1b1);

	plus_assign_no_sign(a0b0, other, n/2);
	plus_assign_no_sign(a0b0, a1b1, n);

	return a0b0;
}

std::vector<unsigned int>
big_int::divide_newton(const std::vector<unsigned int> &lhs, const std::vector<unsigned int> &rhs) const
{
	std::vector<big_int> rev_a;
	std::vector<big_int> rev_b;

	rev_a.reserve(lhs.size());
	rev_b.reserve(rhs.size());

	for(auto num : std::views::reverse(lhs))
	{
		rev_a.emplace_back(num);
	}

	for(auto num : std::views::reverse(rhs))
	{
		rev_b.emplace_back(num);
	}

	auto rev_b_1 = newton_transform(rev_b);

	auto rev_q = multiply_vectors(rev_a, rev_b_1);
	modulo_vector(rev_q, lhs.size() - rhs.size() + 1);

	big_int val(1);

	big_int q;

	for (auto& num : std::views::reverse(rev_q))
	{
		q += num * val;
		val <<= 32;
	}

	big_int a(lhs);
	big_int b(rhs);

	auto r = a - q * b;

	bool sign = r._sign;
	r._sign = true;

	if (r >= b)
	{
		r._sign = sign;
		q += r / b;
		r %= b;
	}

	r._sign = sign;

	if (r < 0_bi)
	{
		--q;
	}

	return q._digits;
}

bool big_int::operator==(const big_int& other) const noexcept {
	if (_sign != other._sign || _digits.size() != other._digits.size())
		return false;

	auto it1 = _digits.begin();
	auto it2 = other._digits.begin();

	while (it1 != _digits.end() && it2 != other._digits.end()) {
		if (*it1 != *it2) {
			return false;
		}
		++it1;
		++it2;
	}

	return true;
}
std::vector<big_int> big_int::newton_transform(const std::vector<big_int> &f)
{
	std::vector<big_int> g{1};

	auto l = logl(f.size());
	size_t r = double((size_t)l) == l ? size_t(l) : size_t(l) + 1;
	if (r < 2)
		r = 2;

	for (size_t i = 1; i <= r; ++i)
	{
		g = minus_vectors( multiply_vector(g, 2), multiply_vectors(f, multiply_vectors(g, g)));
		modulo_vector(g, 1ULL << i);
	}

	return g;
}

std::vector<big_int> big_int::multiply_vector(const std::vector<big_int> &lhs, size_t val)
{
	auto res = lhs;

	for (auto& num : res)
	{
		num *= val;
	}

	return res;
}

void big_int::modulo_vector(std::vector<big_int> &val, size_t power)
{
	while (val.size() > power)
		val.pop_back();
}

std::vector<big_int> big_int::minus_vectors(const std::vector<big_int> &lhs, const std::vector<big_int> &rhs)
{
	std::vector<big_int> res;

	res.reserve(std::max(lhs.size(), rhs.size()));

	for(size_t i = 0; i < std::max(lhs.size(), rhs.size()); ++i)
	{
		res.push_back((i < lhs.size() ? lhs[i] : 0_bi) - (i < rhs.size() ? rhs[i] : 0_bi));
	}

	return  res;
}

std::vector<big_int> big_int::plus_vectors(const std::vector<big_int> &lhs, const std::vector<big_int> &rhs)
{
	std::vector<big_int> res;

	res.reserve(std::max(lhs.size(), rhs.size()));

	for(size_t i = 0; i < std::max(lhs.size(), rhs.size()); ++i)
	{
		res.push_back((i < lhs.size() ? lhs[i] : 0_bi) + (i < rhs.size() ? rhs[i] : 0_bi));
	}

	return  res;
}

std::vector<big_int> big_int::multiply_vectors(const std::vector<big_int> &lhs, const std::vector<big_int> &rhs)
{
	std::vector<big_int> res(lhs.size() + rhs.size(), 0_bi);

	for(size_t i = 0; i < lhs.size(); ++i)
	{
		for(size_t j = 0; j < rhs.size(); ++j)
		{
			res[i + j] += lhs[i] * rhs[j];
		}
	}

	return res;
}

template<std::integral Num>
big_int::big_int(Num d) : _sign(true)
{
	if (d != 0)
	{
		if constexpr (std::signed_integral<Num>)
		{
			_sign = d >= 0;
			_digits.push_back(d >= 0 ? d : -1 * d);
		} else
		{
			_digits.push_back(d);
		}
	}
}

big_int operator""_bi(unsigned long long n)
{
	return n;
}

big_int big_int::factorial(big_int n)
{
	if (n < big_int("0")){
		throw std::invalid_argument("Must be not negative number");
	}
	if (n == big_int("0") || n == big_int("1"))
	{
		return big_int("1");
	}
	return n * factorial(n-big_int("1"));
}
