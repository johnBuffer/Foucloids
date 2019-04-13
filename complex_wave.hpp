#pragma once

#include <vector>

struct Point
{
	Point(): 
		x(0.0),
		y(0.0)
	{}

	Point(double x_, double y_) :
		x(x_),
		y(y_)
	{}

	double x, y;
};

struct PolarPoint
{
	PolarPoint() :
		r(0.0),
		a(0.0)
	{}

	PolarPoint(double r_, double a_) :
		r(r_),
		a(a_)
	{}

	PolarPoint(const Point& point)
	{
		double vx(point.x);
		double vy(point.y);

		r = sqrt(vx*vx + vy*vy);
		a = acos(vx / r);
		if (vy < 0)
			a *= -1.0;
	}

	double r, a;
};

struct ComplexWave
{
	ComplexWave(const std::vector<Point>& signal, uint32_t k)
	{

	}
};
