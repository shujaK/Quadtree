#pragma once
#include "vec2.h"

template <typename T>
struct circle {
	vec2<T> center;
	T radius;

	circle(vec2<T> _center, T _radius) : center(_center), radius(_radius) {}

	bool circle::containsPoint(vec2<T>& p)
	{
		return ((p.x - center.x) * (p.x - center.x)) + ((p.y - center.y) * (p.y - center.y)) < (radius * radius);
	}
};