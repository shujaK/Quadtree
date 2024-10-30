#pragma once

template <typename T>
struct vec2 {
	T x, y;
	vec2(T fx, T fy) : x(fx), y(fy) {}
	vec2() : x(T()), y(T()) {}

	vec2& operator+=(const vec2& rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		return *this;
	}

	vec2& operator*=(const T& rhs)
	{
		this->x *= rhs;
		this->y *= rhs;
		return *this;
	}

	vec2& operator/=(const T& rhs)
	{
		this->x /= rhs;
		this->y /= rhs;
		return *this;
	}

	auto magnitude()
	{
		return (sqrt(x * x + y * y));
	}

	void normalize()
	{
		this->x /= this->magnitude();
		this->y /= this->magnitude();
	}

	friend vec2 operator*(vec2 lhs, const T& rhs)
	{
		lhs *= rhs;
		return lhs;
	}
};