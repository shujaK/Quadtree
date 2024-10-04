#pragma once

#include "vec2.h"
#include "circle.h"

template <typename T>
struct quad {
    vec2<T> center;
    T size;

    quad(vec2<T> _center, T _size) : center(_center), size(_size) {}

    bool quad::containsPoint(vec2<T> p)
    {
        return (p.x >= center.x - size && p.x < center.x + size &&
            p.y >= center.y - size && p.y < center.y + size);
    }

    bool quad::intersectsQuad(quad<T>& other)
    {
        if (center.x + size < other.center.x - other.size ||
            center.x - size > other.center.x + other.size || 
            center.y + size < other.center.y - other.size ||  
            center.y - size > other.center.y + other.size)    
        {
            return false;
        }
        return true;
    }

    bool quad::intersectsCircle(circle<T>& other)
    {
        T closestX = std::max(center.x - size, std::min(other.center.x, center.x + size));
        T closestY = std::max(center.y - size, std::min(other.center.y, center.y + size));

        return other.containsPoint(vec2<T>(closestX, closestY));
    }
};