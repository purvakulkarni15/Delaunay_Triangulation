#include "DelaunayTriangulation.h"
#include <algorithm>


DelaunayTriangulation::DelaunayTriangulation()
{
}

DelaunayTriangulation::~DelaunayTriangulation()
{
}

void DelaunayTriangulation::triangulate()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		vector<pair<Point*, Point*>>edgeBuffer;
		vector<pair<Point*, Point*>> doubleEdgesBuffer;

		for (vector<Triangle>::iterator it = triangles.begin(); it != triangles.end();)
		{
			if (it->isInsideCircumcircle(*vertices[i]))
			{

				if (std::find(edgeBuffer.begin(), edgeBuffer.end(), std::make_pair(it->e1->p1, it->e1->p2)) == edgeBuffer.end() &&
					std::find(edgeBuffer.begin(), edgeBuffer.end(), std::make_pair(it->e1->p2, it->e1->p1)) == edgeBuffer.end())
				{
					edgeBuffer.push_back(make_pair(it->e1->p1, it->e1->p2));
				}
				else {
					doubleEdgesBuffer.push_back(make_pair(it->e1->p1, it->e1->p2));
				}
				if (find(edgeBuffer.begin(), edgeBuffer.end(), make_pair(it->e2->p1, it->e2->p2)) == edgeBuffer.end() &&
					find(edgeBuffer.begin(), edgeBuffer.end(), make_pair(it->e2->p2, it->e2->p1)) == edgeBuffer.end())
				{
					edgeBuffer.push_back(make_pair(it->e2->p1, it->e2->p2));
				}
				else {
					doubleEdgesBuffer.push_back(make_pair(it->e2->p1, it->e2->p2));
				}
				if (find(edgeBuffer.begin(), edgeBuffer.end(), make_pair(it->e3->p1, it->e3->p2)) == edgeBuffer.end() &&
					find(edgeBuffer.begin(), edgeBuffer.end(), make_pair(it->e3->p2, it->e3->p1)) == edgeBuffer.end())
				{
					edgeBuffer.push_back(make_pair(it->e3->p1, it->e3->p2));
				}
				else {
					doubleEdgesBuffer.push_back(make_pair(it->e3->p1, it->e3->p2));
				}

				it = triangles.erase(it);
			}
			else {
				++it;
			}
		}

		for (vector<pair<Point*, Point*>>::iterator it = doubleEdgesBuffer.begin(); it != doubleEdgesBuffer.end(); it++)
		{
			auto it1 = find(edgeBuffer.begin(), edgeBuffer.end(), make_pair(it->first, it->second));
			auto it2 = find(edgeBuffer.begin(), edgeBuffer.end(), make_pair(it->second, it->first));

			if (it1 != edgeBuffer.end())
			{
				edgeBuffer.erase(it1);
			}
			else if (it2 != edgeBuffer.end()) {
				edgeBuffer.erase(it2);
			}
		}

		for (vector<pair<Point*, Point*>>::iterator it = edgeBuffer.begin(); it != edgeBuffer.end(); it++)
		{
			Triangle t = Triangle(vertices[i], it->first, it->second);
			pdd P = make_pair(vertices[i]->coordinates.x, vertices[i]->coordinates.y);
			pdd Q = make_pair(it->first->coordinates.x, it->first->coordinates.y);
			pdd R = make_pair(it->second->coordinates.x, it->second->coordinates.y);
			t.setCircumcirle(P, Q, R);
			triangles.push_back(t);
		}
		edgeBuffer.clear();
		doubleEdgesBuffer.clear();

	}


	for (vector<Point*>::iterator it = vertices.begin(); it != vertices.end();)
	{
		if (*it == superTriangle.p1 || *it == superTriangle.p2 || *it == superTriangle.p3)
		{
			it = vertices.erase(it);
		}
		else {
			++it;
		}
	}

	for (vector<Triangle>::iterator it = triangles.begin(); it != triangles.end();)
	{
		if (it->p1 == superTriangle.p1 || it->p2 == superTriangle.p2 || it->p3 == superTriangle.p3 ||
			it->p1 == superTriangle.p2 || it->p2 == superTriangle.p3 || it->p3 == superTriangle.p1 ||
			it->p1 == superTriangle.p3 || it->p2 == superTriangle.p1 || it->p3 == superTriangle.p2)
		{
			it = triangles.erase(it);
		}
		else {
			++it;
		}
	}
}

void DelaunayTriangulation::setSuperTriangle() 
{
	glm::vec3 mid = (float)0.5*glm::vec3(boundingBox[0].coordinates + boundingBox[1].coordinates);

	float xMin = boundingBox[0].coordinates.x;
	float xMax = boundingBox[1].coordinates.x;
	float xMid = mid.x;
	float yMin = boundingBox[0].coordinates.y;
	float yMax = boundingBox[1].coordinates.y;
	float yMid = mid.y;
	float zMin = boundingBox[0].coordinates.z;
	float zMax = boundingBox[1].coordinates.z;
	float zMid = mid.z;

	Point *p1 = (Point*)malloc(sizeof(Point));
	Point *p2 = (Point*)malloc(sizeof(Point));
	Point *p3 = (Point*)malloc(sizeof(Point));

	p1->coordinates = glm::vec3(xMid, +2 * abs(yMax - yMin), 0);
	p2->coordinates = glm::vec3(xMin-2*abs(xMax-xMin)-50.0,yMin-50.0,0);
	p3->coordinates = glm::vec3(xMax+2*abs(xMax-xMin)+50.0,yMin-50.0,0);

	superTriangle = Triangle(p1, p2, p3);

	vertices.push_back(superTriangle.p1);
	vertices.push_back(superTriangle.p2);
	vertices.push_back(superTriangle.p3);

	edges.push_back(superTriangle.e1);
	edges.push_back(superTriangle.e2);
	edges.push_back(superTriangle.e3);
	

	pdd P = make_pair(superTriangle.p1->coordinates.x, superTriangle.p1->coordinates.y);
	pdd Q = make_pair(superTriangle.p2->coordinates.x, superTriangle.p2->coordinates.y);
	pdd R = make_pair(superTriangle.p3->coordinates.x, superTriangle.p3->coordinates.y);
	
	superTriangle.setCircumcirle(P, Q, R);

	triangles.push_back(superTriangle);
} 