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

void join(const Point& p1, const Point& p2, std::vector<Point>& signal, std::vector<double>& dists, double step)
{
	Point v(p2 - p1);
	double vx = v.x;
	double vy = v.y;
	double dist(distance(v));

	if (dist < 0.01)
		return;

	vx /= dist;
	vy /= dist;

	double progress(0.0);
	while (progress + step < dist)
	{
		progress += step;

		double x = p1.x + vx * progress;
		double y = p1.y + vy * progress;

		signal.emplace_back(x, y);
		dists.push_back(step);
	}

	signal.push_back(p2);
	dists.push_back(dist - progress);
}

sf::CircleShape getDisc(float radius, float x, float y, const sf::Color& color)
{
	sf::CircleShape circle(radius);
	circle.setFillColor(color);
	circle.setOrigin(radius, radius);
	circle.setPosition(x, y);

	return circle;
}

sf::CircleShape getCircle(float radius, float x, float y, float thickness, const sf::Color& color)
{
	sf::CircleShape circle(radius);
	circle.setOutlineColor(color);
	circle.setFillColor(sf::Color(0, 0, 0, 0));
	circle.setOutlineThickness(thickness);
	circle.setOrigin(radius, radius);
	circle.setPosition(x, y);

	return circle;
}
