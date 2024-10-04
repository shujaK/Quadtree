#pragma once

template <typename T>
struct vec2 {
	T x, y;
	vec2(T fx, T fy) : x(fx), y(fy) {}
	vec2() : x(T()), y(T()) {}
};