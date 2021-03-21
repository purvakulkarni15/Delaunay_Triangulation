#pragma once
#include "Point.h"
class Edge
{
public:
	Edge();
	~Edge();

	Point* p1, *p2;

	Edge(Point*, Point*);
	
};

