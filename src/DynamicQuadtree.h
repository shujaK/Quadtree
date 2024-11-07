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

	typename std::list<QcItem<T>>::iterator getIterator()
	{
		return pQItem->pQcItem;
	}
};

template <typename T>
class QItem {
public:
	typename std::list<QcItem<T>>::iterator pQcItem;
	std::list<QItem<T>>* nodeItems;
	quad<T> boundary;

	DynamicQuadtree<T>* node;

	QItem(typename std::list<QcItem<T>>::iterator _pQcItem, std::list<QItem<T>>* pni, const quad<T>& _boundary, DynamicQuadtree<T>* n)
		: pQcItem(_pQcItem), nodeItems(pni), boundary(_boundary), node(n) {}

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
		if (!root.boundary.containsPoint(item))
		{
			return;
		}

		QcItem<T> newItem;
		newItem.item = item;

		allItems.emplace_back(newItem);
		auto it = std::prev(allItems.end());

		// QcItem.pQItem = QItem
		auto result = root.insert(it);
		if (!result.second)
		{
			// std::cout << "Failed to insert item into quadtree.\n";
		}
		else
		{
			allItems.back().pQItem = result.first;
		}
	}

	QcList query(circle<T> range)
	{
		QcList res;

		for (auto& qci : root.query(range))
		{
			res.push_back(*qci);
		}

		return res;
	}

	void remove(typename QcList::iterator pQCI, sf::RenderWindow& window)
	{

		DynamicQuadtree<T>* node = pQCI->pQItem->node;

		std::list<QItem<T>>* currNodeItems = pQCI->pQItem->nodeItems;
		currNodeItems->erase(pQCI->pQItem);

		while (node->parent && node->parent->internal)
		{
			auto parent = node->parent;
			int itemCount = 0;
			if (parent->tr) itemCount += parent->tr->items.size();
			if (parent->tl) itemCount += parent->tl->items.size();
			if (parent->br) itemCount += parent->br->items.size();
			if (parent->bl) itemCount += parent->bl->items.size();

			if (itemCount <= parent->capacity)
			{
				parent->internal = false;
				parent->items.clear();

				if (parent->tr)
				{
					parent->items.insert(parent->items.end(), parent->tr->items.begin(), parent->tr->items.end());
					delete parent->tr;
					parent->tr = nullptr;
					window.clear();
					drawQuadtreeDebug(window, &root);
				}
				if (parent->tl)
				{
					parent->items.insert(parent->items.end(), parent->tl->items.begin(), parent->tl->items.end());
					delete parent->tl;
					parent->tl = nullptr;
					window.clear();
					drawQuadtreeDebug(window, &root);
				}
				if (parent->br)
				{
					parent->items.insert(parent->items.end(), parent->br->items.begin(), parent->br->items.end());
					delete parent->br;
					parent->br = nullptr;
					window.clear();
					drawQuadtreeDebug(window, &root);
				}
				if (parent->bl)
				{
					parent->items.insert(parent->items.end(), parent->bl->items.begin(), parent->bl->items.end());
					delete parent->bl;
					parent->bl = nullptr;
					window.clear();
					drawQuadtreeDebug(window, &root);
				}

				node = parent;
			}
		}

		allItems.erase(pQCI);
	}

	void remove(QcItem<T> item, sf::RenderWindow& window)
	{
		typename QcList::iterator pQCI = item.getIterator();

		remove(pQCI, window);
	}
};

// underlying structure
template <typename T>
struct DynamicQuadtree {
	const int capacity;
	std::list<QItem<T>> items;
	bool internal = false;
	quad<T> boundary;

	DynamicQuadtree<T>* parent = nullptr;
	DynamicQuadtree<T>* tr = nullptr;
	DynamicQuadtree<T>* tl = nullptr;
	DynamicQuadtree<T>* br = nullptr;
	DynamicQuadtree<T>* bl = nullptr;

	DynamicQuadtree(quad<T> _boundary, const int _capacity) : boundary(_boundary), capacity(_capacity) {}

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

		quad<T> trBoundary(vec2<T>(boundary.center.x + dSize, boundary.center.y - dSize), dSize);
		tr = new DynamicQuadtree<T>(trBoundary, capacity);
		tr->parent = this;

		quad<T> tlBoundary(vec2<T>(boundary.center.x - dSize, boundary.center.y - dSize), dSize);
		tl = new DynamicQuadtree<T>(tlBoundary, capacity);
		tl->parent = this;

		quad<T> brBoundary(vec2<T>(boundary.center.x + dSize, boundary.center.y + dSize), dSize);
		br = new DynamicQuadtree<T>(brBoundary, capacity);
		br->parent = this;

		quad<T> blBoundary(vec2<T>(boundary.center.x - dSize, boundary.center.y + dSize), dSize);
		bl = new DynamicQuadtree<T>(blBoundary, capacity);
		bl->parent = this;


		return true;
	}

	std::pair<typename std::list<QItem<T>>::iterator, bool> insert(typename std::list<QcItem<T>>::iterator pQcItem)
	{
		if (!boundary.containsPoint(pQcItem->item))
		{
			// The item is out of the boundary
			return { items.end(), false }; // Indicate failure
		}

		if (isLeaf() && items.size() < capacity && !internal)
		{
			// There is space in this node
			QItem<T> newItem(pQcItem, &items, boundary, this);
			items.push_back(newItem);

			auto it = --items.end();
			return { it, true };
		}

		if (isLeaf() && items.size() >= capacity && !internal)
		{
			// No space and this is a leaf node, so we subdivide
			internal = true;
			subdivide();

			for (auto& pt : items)
			{
				if (tr->boundary.containsPoint(pt.pQcItem->item)) tr->insert(pt.pQcItem);
				else if (tl->boundary.containsPoint(pt.pQcItem->item)) tl->insert(pt.pQcItem);
				else if (br->boundary.containsPoint(pt.pQcItem->item)) br->insert(pt.pQcItem);
				else if (bl->boundary.containsPoint(pt.pQcItem->item)) bl->insert(pt.pQcItem);
			}

			items.clear();
		}

		// Try to insert into the appropriate quadrant
		if (tr->boundary.containsPoint(pQcItem->item)) return tr->insert(pQcItem);
		else if (tl->boundary.containsPoint(pQcItem->item)) return tl->insert(pQcItem);
		else if (br->boundary.containsPoint(pQcItem->item)) return br->insert(pQcItem);
		else if (bl->boundary.containsPoint(pQcItem->item)) return bl->insert(pQcItem);

		// If we reach here, something went wrong
		return { items.end(), false }; // Indicate failure
	}

	// a list of iterators that point to the corresponding Qcitems
	std::list<typename std::list<QcItem<T>>::iterator> query(circle<T> range)
	{
		std::list<std::list<QcItem<T>>::iterator> res;

		if (!boundary.intersectsCircle(range))
		{
			return res;
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

		if (!internal)
		{
			for (auto& item : items)
			{
				if (range.containsPoint(item.pQcItem->item))
				{
					res.push_back(item.pQcItem);
				}
			}
		}

		return res;
	}

};