#pragma once
#include <vector>
#include "point.hpp"

double sum(const std::vector<double>& in)
{
	double sum_vec = 0.0;
	for (const double& d : in)
	{
		sum_vec += d;
	}

	return sum_vec;
}

const std::vector<double> computeDistances(const std::vector<double>& x, const std::vector<double>& y)
{
	uint32_t size(x.size());
	std::vector<double> dists(size);
	for (uint32_t i(0); i < size-1; ++i)
	{
		double vx(x[i] - x[i + 1]);
		double vy(y[i] - y[i + 1]);
		dists[i] = sqrt(vx*vx + vy*vy);
	}

	double vx(x[size-1] - x[0]);
	double vy(y[size-1] - y[0]);
	dists[size - 1] = sqrt(vx*vx + vy * vy);

	return dists;
}

const std::vector<double> computeDistances(const std::vector<Point>& points)
{
	uint32_t size(points.size());
	std::vector<double> dists(size);
	for (uint32_t i(0); i < size; ++i)
	{
		uint32_t next_index((i + 1) % size);
		dists[i] = distance(points[i], points[next_index]);
	}

	return dists;
}

void join(double x1, double y1, double x2, double y2, std::vector<double>& in_x, std::vector<double>& in_y, std::vector<double>& dists, double step)
{
	double vx = x2 - x1;
	double vy = y2 - y1;
	double distance(sqrt(vx*vx + vy*vy));

	vx /= distance;
	vy /= distance;

	double progress(0.0);
	while (progress + step < distance)
	{
		progress += step;

		double x = x1 + vx * progress;
		double y = y1 + vy * progress;

		in_x.push_back(x);
		in_y.push_back(y);
		dists.push_back(step);
	}

	in_x.push_back(x2);
	in_y.push_back(y2);
	dists.push_back(distance - progress);
}



