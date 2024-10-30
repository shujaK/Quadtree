#pragma once
#include "vec2.h"
#include "quad.h"
#include "circle.h"
#include <vector>
#include <list>

template <typename T>
class Quadtree {
public:
    const int capacity;
    int numPoints = 0;

    using it = typename std::list<vec2<T>>::iterator;
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

    bool insert(it p)
    {
        if (!boundary.containsPoint(*p)) return false;

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
class QuadtreeContainer {
    using container = std::list<vec2<T>>;
    container allItems;

    using iterator = typename container::iterator;

    Quadtree<iterator> root;

public:
    QuadtreeContainer(quad<T>& _boundary, const int _capacity) : root(_boundary, _capacity) {}

    size_t size()
    {
        return allItems.size();
    }

    iterator begin()
    {
        return allItems.begin();
    }

    iterator end()
    {
        return allItems.end();
    }

    typename container::const_iterator cbegin()
    {
        return allItems.cbegin();
    }

    typename container::const_iterator cend()
    {
        return allItems.cend();
    }

    void insert(vec2<T>& item)
    {
        allItems.push_back(item);
        root.insert((iterator)std::prev(allItems.end()));
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
};
