#pragma once
#include "Triangle.h"
#include "Point.h"
#include "Edge.h"
#include <vector>
#include <windows.h>
using namespace std;

class DelaunayTriangulation
{
public:
	DelaunayTriangulation();
	~DelaunayTriangulation();

	vector<Point*> vertices;
	vector<Triangle> triangles;
	vector<Edge*> edges;

	Point boundingBox[2];
	Triangle superTriangle;

	void setSuperTriangle();
	void triangulate();
};

