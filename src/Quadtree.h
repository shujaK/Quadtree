#pragma once
#include "vec2.h"
#include "quad.h"
#include "circle.h"
#include <vector>
#include <list>

template <typename T>
class QuadTreeItemInfo {
public:
	typename std::list<vec2<T>>::iterator pItem; // Store the iterator to the item
	quad<T> boundary; // Store the boundary

	// Constructor
	QuadTreeItemInfo(typename std::list<vec2<T>>::iterator item = typename std::list<vec2<T>>::iterator(), quad<T> b = quad<T>())
		: pItem(item), boundary(b)
	{}

	// Optional: You can add methods to check if the item is valid
	bool isValid() const
	{
		return pItem != nullptr; // Change this based on your criteria for a valid item
	}
};


template <typename T>
class Quadtree {
public:
	using it = typename std::list<vec2<T>>::iterator;

	const int capacity;
	int numPoints = 0;
	it* points;
	quad<T> boundary;

	Quadtree<T>* tr = nullptr;
	Quadtree<T>* tl = nullptr;
	Quadtree<T>* br = nullptr;
	Quadtree<T>* bl = nullptr;

	Quadtree(quad<T>& _boundary, const int _capacity) : boundary(_boundary), capacity(_capacity)
	{
		points = new it[_capacity];
	}

	~Quadtree()
	{
		delete[] points;
		delete tr;
		delete tl;
		delete br;
		delete bl;
	}

	bool isLeaf()
	{
		return ((tr == nullptr) && (tl == nullptr) && (br == nullptr) && (bl == nullptr));
	}

	bool subdivide()
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

	QuadTreeItemInfo<T> insert(it p)
	{
		if (!boundary.containsPoint(*p)) return QuadTreeItemInfo();

		if (numPoints < capacity && this->isLeaf())
		{
			points.push_back(p);
			numPoints++;
			return QuadTreeItemInfo(p, boundary);
		}

		if (this->isLeaf())
		{
			this->subdivide();
		}

		if (auto result = tr->insert(p); result.pItem != nullptr) return result;
		if (auto result = tl->insert(p); result.pItem != nullptr) return result;
		if (auto result = br->insert(p); result.pItem != nullptr) return result;
		if (auto result = bl->insert(p); result.pItem != nullptr) return result;

		return QuadTreeItemInfo();
	}


	void queryRect(quad<T>& rect, std::list<it>& dest)
	{
		if (!boundary.intersectsQuad(rect)) { return; }
		else
		{
			for (int i = 0; i < numPoints; i++)
			{
				if (rect.containsPoint(*points[i]))
				{
					dest.push_back(points[i]);
				}
			}
		}

		if (!this->isLeaf())
		{
			tr->queryRect(rect, dest);
			tl->queryRect(rect, dest);
			br->queryRect(rect, dest);
			bl->queryRect(rect, dest);
		}
	}

	void queryCircle(circle<T>& circle, std::list<it>& dest)
	{
		if (!boundary.intersectsCircle(circle)) { return; }
		else
		{
			for (int i = 0; i < numPoints; i++)
			{
				if (circle.containsPoint(*points[i]))
				{
					dest.push_back(points[i]);
				}
			}
		}

		if (!this->isLeaf())
		{
			tr->queryCircle(circle, dest);
			tl->queryCircle(circle, dest);
			br->queryCircle(circle, dest);
			bl->queryCircle(circle, dest);
		}
	}
};

template <typename T>
class QuadTreeItem {
public:
	vec2<T> item;

	QuadTreeItemInfo<T> itemInfo;
};

template <typename T>
class QuadtreeContainer {
	using container = std::list<QuadTreeItem<T>>;
	container allItems;

	using iterator = typename container::iterator;

	Quadtree<T> root;

public:
	QuadtreeContainer(quad<T>& _boundary, const int _capacity) : root(_boundary, _capacity) {}

	size_t size() { return allItems.size() }

	iterator begin() { return allItems.begin() }

	iterator end() { return allItems.end() }

	typename container::const_iterator cbegin() const { return allItems.cbegin(); }

	typename container::const_iterator cend() const { return allItems.cend() }

	void insert(vec2<T>& item)
	{
		QuadTreeItem<T> newItem;
		newItem.item = item;

		allItems.emplace_back(newItem);
		allItems.back().itemInfo = root.insert(std::prev(allItems.end()));
	}

	std::list<iterator> queryRect(quad<T>& rect)
	{
		std::list<iterator> itemPointers;
		root.queryRect(rect, itemPointers);
		return itemPointers;
	}

	std::list<iterator> queryCircle(circle<T>& circle)
	{
		std::list<iterator> itemPointers;
		root.queryCircle(circle, itemPointers);
		return itemPointers;
	}

	Quadtree<T>* getRoot()
	{
		return &root;
	}


};

template <typename T>
struct StaticQuadtree {
	const int capacity;
	int numPoints = 0;
	vec2<T>* points;

	quad<T> boundary;

	StaticQuadtree<T>* tr = nullptr;
	StaticQuadtree<T>* tl = nullptr;
	StaticQuadtree<T>* br = nullptr;
	StaticQuadtree<T>* bl = nullptr;

	StaticQuadtree(quad<T> _boundary, const int _capacity) : boundary(_boundary), capacity(_capacity)
	{
		points = new vec2<T>[_capacity];
	}

	bool StaticQuadtree::isLeaf()
	{
		return ((tr == nullptr) && (tl == nullptr) && (br == nullptr) && (bl == nullptr));
	}

	bool StaticQuadtree::subdivide()
	{
		T dSize = boundary.size / 2;
		quad<T> tlBoundary(vec2<T>(boundary.center.x - dSize, boundary.center.y + dSize), dSize);
		quad<T> trBoundary(vec2<T>(boundary.center.x + dSize, boundary.center.y + dSize), dSize);
		quad<T> blBoundary(vec2<T>(boundary.center.x - dSize, boundary.center.y - dSize), dSize);
		quad<T> brBoundary(vec2<T>(boundary.center.x + dSize, boundary.center.y - dSize), dSize);

		tr = new StaticQuadtree<T>(trBoundary, capacity);
		tl = new StaticQuadtree<T>(tlBoundary, capacity);
		br = new StaticQuadtree<T>(brBoundary, capacity);
		bl = new StaticQuadtree<T>(blBoundary, capacity);

		return true;
	}

	bool StaticQuadtree::insert(vec2<T> p)
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
	std::vector<vec2<T>> StaticQuadtree::query(quad<T> range)
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

	std::vector<vec2<T>> StaticQuadtree::queryCircle(circle<T> circle)
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