#include "Triangle.h"

Triangle::Triangle()
{
}

Triangle::~Triangle()
{
}

void lineFromPoints(pdd, pdd, double &, double &, double &);
void perpendicularBisectorFromLine(pdd, pdd, double &, double &, double &);
pdd lineLineIntersection(double, double, double, double, double, double);

Triangle::Triangle(Point *p1, Point *p2, Point *p3)
{
	this->p1 = p1;
	this->p2 = p2;
	this->p3 = p3;

	e1 = (Edge*)malloc(sizeof(Edge));
	e2 = (Edge*)malloc(sizeof(Edge));
	e3 = (Edge*)malloc(sizeof(Edge));

	*e1 = Edge(p1, p2);
	*e2 = Edge(p2, p3);
	*e3 = Edge(p3, p1);
}

bool Triangle::isInsideCircumcircle(Point p)
{
	if (glm::length(p.coordinates - center.coordinates) < radius)
	{
		return true;
	}

	return false;
}

void Triangle::setCircumcirle(pdd P, pdd Q, pdd R)
{
	double a, b, c;
	lineFromPoints(P, Q, a, b, c);
	double e, f, g;
	lineFromPoints(Q, R, e, f, g);
	perpendicularBisectorFromLine(P, Q, a, b, c);
	perpendicularBisectorFromLine(Q, R, e, f, g);
	pdd circumcenter = lineLineIntersection(a, b, c, e, f, g);
	if (circumcenter.first == FLT_MAX && circumcenter.second == FLT_MAX) {
	}
	else {
		center.coordinates.x = circumcenter.first;
		center.coordinates.y = circumcenter.second;
		center.coordinates.z = 0;

		radius = glm::length(center.coordinates - p1->coordinates);
	}
}

void lineFromPoints(pdd P, pdd Q, double &a, double &b, double &c) {
	a = Q.second - P.second;
	b = P.first - Q.first;
	c = a * (P.first) + b * (P.second);
}
void perpendicularBisectorFromLine(pdd P, pdd Q, double &a, double &b, double &c) {
	pdd mid_point = make_pair((P.first + Q.first) / 2, (P.second + Q.second) / 2);
	c = -b * (mid_point.first) + a * (mid_point.second);
	double temp = a;
	a = -b;
	b = temp;
}
pdd lineLineIntersection(double a1, double b1, double c1, double a2, double b2, double c2) {
	double determinant = a1 * b2 - a2 * b1;
	if (determinant == 0) {
		return make_pair(FLT_MAX, FLT_MAX);
	}
	else {
		double x = (b2*c1 - b1 * c2) / determinant;
		double y = (a1*c2 - a2 * c1) / determinant;
		return make_pair(x, y);
	}
}