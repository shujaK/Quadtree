# Quadtree

A simple and efficient implementation of a Quadtree data structure in C++.

## Overview

A Quadtree is a tree data structure in which each internal node has exactly four children. It is commonly used to partition a two-dimensional space by recursively subdividing it into four quadrants or regions. This implementation is useful for applications such as collision detection, view frustum culling, and spatial indexing.

## Features

- Point insertion with capacity checking
- Boundary checking with quads and circles
- Querying points within a rectangular or circular range
- Automatic subdivision when capacity is reached

## Demo
Included is a demonstration of the quadtree using SFML

![](https://github.com/shujaK/Quadtree/blob/ec81499f8e841439d1894430f6272afad1335d93/demos/demo.gif)


## Requirements
- vec2.h
- quad.h
- circle.h
- quadtree.h
- SFML (for visualization, not included)
