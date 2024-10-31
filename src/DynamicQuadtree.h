#pragma once
#include "vec2.h"
#include "quad.h"
#include "circle.h"
#include <vector>
#include <list>

#include <list>

template <typename T>
class QItem;

template <typename T>
class QcItem;

template <typename T>
struct DynamicQuadtree;

template <typename T>
class QcItem {
public:
	vec2<T> item;
	typename std::list<QItem<T>>::iterator pQItem;


};

template <typename T>
class QItem {
public:
	typename std::list<QcItem<T>>::iterator pQcItem;
	quad<T> boundary;

	QItem(typename std::list<QcItem<T>>::iterator _pQcItem, const quad<T>& _boundary)
		: pQcItem(_pQcItem), boundary(_boundary) {}
};

// container
template <typename T>
class Quadtree {
public:
	using QcList = std::list<QcItem<T>>;
	QcList allItems;

	DynamicQuadtree<T> root;

	Quadtree(quad<T> _boundary, const int _capacity) : root(_boundary, _capacity) {}

	void insert(vec2<T>& item)
	{
		QcItem<T> newItem;
		newItem.item = item;

		allItems.emplace_back(newItem);
		// QcItem.pQItem = QItem
		allItems.back().pQItem = root.insert(std::prev(allItems.end()));
	}

	std::list<vec2<T>> query(circle<T> range)
	{
		std::list<vec2<T>> res;

		for (auto& qci : root.query(range))
		{
			res.push_back(qci.item);
		}

		return res;
	}
};

// underlying structure
template <typename T>
struct DynamicQuadtree {
	const int capacity;
	std::list<QItem<T>> items;

	quad<T> boundary;

	DynamicQuadtree<T>* tr = nullptr;
	DynamicQuadtree<T>* tl = nullptr;
	DynamicQuadtree<T>* br = nullptr;
	DynamicQuadtree<T>* bl = nullptr;

	DynamicQuadtree(quad<T> _boundary, const int _capacity) : boundary(_boundary), capacity(_capacity){	}

	~DynamicQuadtree()
	{
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

		tr = new DynamicQuadtree<T>(trBoundary, capacity);
		tl = new DynamicQuadtree<T>(tlBoundary, capacity);
		br = new DynamicQuadtree<T>(brBoundary, capacity);
		bl = new DynamicQuadtree<T>(blBoundary, capacity);

		return true;
	}

	typename std::list<QItem<T>>::iterator insert(typename std::list<QcItem<T>>::iterator pQcItem)
	{
		if (!boundary.containsPoint(pQcItem->item))
		{
			// The item is out of the boundary
			throw std::out_of_range("Item out of boundary");
		}

		if (isLeaf() && items.size() < capacity)
		{
			// There is space in this node
			QItem<T> newItem(pQcItem, boundary);
			items.push_back(newItem);
			return --items.end();
		}

		if (isLeaf())
		{
			// No space and this is a leaf node, so we subdivide
			subdivide();
		}

		// Try to insert into the appropriate quadrant
		if (tr->boundary.containsPoint(pQcItem->item)) return tr->insert(pQcItem);
		else if (tl->boundary.containsPoint(pQcItem->item)) return tl->insert(pQcItem);
		else if (br->boundary.containsPoint(pQcItem->item)) return br->insert(pQcItem);
		else if (bl->boundary.containsPoint(pQcItem->item)) return bl->insert(pQcItem);
	}

	std::list<QItem<T>> query(circle<T> range)
	{
		// SHOULD PASS QITEMS UP TO CONTAINER
		std::list<QcItem<T>> res;

		if (!boundary.intersectsCircle(range))
		{
			return res;
		}
		else
		{
			for (QItem<T>& i: items)
			{
				if (range.containsPoint(i.pQcItem->item))
				{
					res.push_back(i.pQcItem->item);
				}
			}
		}

		if (!this->isLeaf())
		{
			auto vtr = tr->query(range);
			res.insert(res.end(), vtr.begin(), vtr.end());

			auto vtl = tl->query(range);
			res.insert(res.end(), vtl.begin(), vtl.end());

			auto vbr = br->query(range);
			res.insert(res.end(), vbr.begin(), vbr.end());

			auto vbl = bl->query(range);
			res.insert(res.end(), vbl.begin(), vbl.end());
		}

		return res;
	}

};


template <typename T>
struct aStaticQuadtree {
	const int capacity;
	int numPoints = 0;
	vec2<T>* points;

	quad<T> boundary;

	aStaticQuadtree<T>* tr = nullptr;
	aStaticQuadtree<T>* tl = nullptr;
	aStaticQuadtree<T>* br = nullptr;
	aStaticQuadtree<T>* bl = nullptr;

	aStaticQuadtree(quad<T> _boundary, const int _capacity) : boundary(_boundary), capacity(_capacity)
	{
		points = new vec2<T>[_capacity];
	}

	~aStaticQuadtree()
	{
		delete[] points;

		delete tr;
		delete tl;
		delete br;
		delete bl;
	}

	bool aStaticQuadtree::isLeaf()
	{
		return ((tr == nullptr) && (tl == nullptr) && (br == nullptr) && (bl == nullptr));
	}

	bool aStaticQuadtree::subdivide()
	{
		T dSize = boundary.size / 2;
		quad<T> tlBoundary(vec2<T>(boundary.center.x - dSize, boundary.center.y + dSize), dSize);
		quad<T> trBoundary(vec2<T>(boundary.center.x + dSize, boundary.center.y + dSize), dSize);
		quad<T> blBoundary(vec2<T>(boundary.center.x - dSize, boundary.center.y - dSize), dSize);
		quad<T> brBoundary(vec2<T>(boundary.center.x + dSize, boundary.center.y - dSize), dSize);

		tr = new aStaticQuadtree<T>(trBoundary, capacity);
		tl = new aStaticQuadtree<T>(tlBoundary, capacity);
		br = new aStaticQuadtree<T>(brBoundary, capacity);
		bl = new aStaticQuadtree<T>(blBoundary, capacity);

		return true;
	}

	bool aStaticQuadtree::insert(vec2<T> p)
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
	std::vector<vec2<T>> aStaticQuadtree::query(quad<T> range)
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

	std::vector<vec2<T>> aStaticQuadtree::queryCircle(circle<T> circle)
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