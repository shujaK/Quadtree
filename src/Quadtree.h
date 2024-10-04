#pragma once
#include "vec2.h"
#include "quad.h"
#include "circle.h"
#include <vector>

template <typename T>
struct Quadtree {
	const int capacity;
	int numPoints = 0;
	vec2<T> *points;

	quad<T> boundary;

	Quadtree<T>* tr = nullptr;
	Quadtree<T>* tl = nullptr;
	Quadtree<T>* br = nullptr;
	Quadtree<T>* bl = nullptr;

	Quadtree(quad<T> _boundary, const int _capacity) : boundary(_boundary), capacity(_capacity) 
	{
		points = new vec2<T>[_capacity];
	}

	bool Quadtree::isLeaf()
	{
		return ((tr == nullptr) && (tl == nullptr) && (br == nullptr) && (bl == nullptr));
	}

	bool Quadtree::subdivide()
	{
		T dSize = boundary.size / 2;
		quad<T> tlBoundary(vec2<T>(boundary.center.x - dSize, boundary.center.y + dSize), dSize);
		quad<T> trBoundary(vec2<T>(boundary.center.x + dSize, boundary.center.y + dSize), dSize);
		quad<T> blBoundary(vec2<T>(boundary.center.x - dSize, boundary.center.y - dSize), dSize);
		quad<T> brBoundary(vec2<T>(boundary.center.x + dSize, boundary.center.y - dSize), dSize);

		tr = new Quadtree<T>(trBoundary, capacity);
		tl = new Quadtree<T>(tlBoundary, capacity);
		br = new Quadtree<T>(brBoundary, capacity);
		bl = new Quadtree<T>(blBoundary, capacity);

		return true;
	}

	bool Quadtree::insert(vec2<T> p)
	{
		if (!boundary.containsPoint(p)) return false;

		if (numPoints < capacity && this->isLeaf())
		{
			points[numPoints] = p;
			numPoints++;

			return true;
		}

		if (this->isLeaf())
		{
			this->subdivide();
		}

		if (tr->insert(p)) return true;
		if (tl->insert(p)) return true;
		if (br->insert(p)) return true;
		if (bl->insert(p)) return true;

		return false;
	}
	std::vector<vec2<T>> Quadtree::query(quad<T> range)
	{
		std::vector<vec2<T>> allPoints;

		if (!boundary.intersectsQuad(range))
		{
			return allPoints;
		}
		else
		{
			for (int i = 0; i < numPoints; i++)
			{
				if (range.containsPoint(points[i]))
				{
					allPoints.push_back(points[i]);
				}
			}
		}

		if (!this->isLeaf())
		{
			auto vtr = tr->query(range);
			allPoints.insert(allPoints.end(), vtr.begin(), vtr.end());

			auto vtl = tl->query(range);
			allPoints.insert(allPoints.end(), vtl.begin(), vtl.end());

			auto vbr = br->query(range);
			allPoints.insert(allPoints.end(), vbr.begin(), vbr.end());

			auto vbl = bl->query(range);
			allPoints.insert(allPoints.end(), vbl.begin(), vbl.end());
		}

		return allPoints;
	}

	std::vector<vec2<T>> Quadtree::queryCircle(circle<T> circle)
	{
		std::vector<vec2<T>> allPoints;

		if (!boundary.intersectsCircle(circle))
		{
			return allPoints;
		}
		else
		{
			for (int i = 0; i < numPoints; i++)
			{
				if (circle.containsPoint(points[i]))
				{
					allPoints.push_back(points[i]);
				}
			}
		}

		if (!this->isLeaf())
		{
			auto vtr = tr->queryCircle(circle);
			allPoints.insert(allPoints.end(), vtr.begin(), vtr.end());

			auto vtl = tl->queryCircle(circle);
			allPoints.insert(allPoints.end(), vtl.begin(), vtl.end());

			auto vbr = br->queryCircle(circle);
			allPoints.insert(allPoints.end(), vbr.begin(), vbr.end());

			auto vbl = bl->queryCircle(circle);
			allPoints.insert(allPoints.end(), vbl.begin(), vbl.end());
		}

		return allPoints;
	}
};