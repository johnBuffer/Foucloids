#pragma once

#include "point.hpp"
#include <vector>

class Signal2D
{
public:
	Signal2D() :
		m_distance_tot(0.0)
	{

	}

	const std::vector<double>& distances() const
	{
		return m_distances;
	}

	const std::vector<Point>& points() const
	{
		return m_points;
	}

	
	const bool empty() const
	{
		return m_points.empty();
	}

	void clear()
	{
		m_points.clear();
		m_distances.clear();
		m_distance_tot = 0.0;
	}

	void addPointInterpolate(const Point pt, double step, double threshold)
	{
		if (!m_points.empty() && threshold)
		{
			if (distance(m_points.back(), pt) < threshold)
				return;
		}

		if (step)
		{
			fill(pt, step);
		}

		addPoint(pt);
	}

	void addPoint(const Point pt)
	{
		m_points.push_back(pt);
		updatLastDistances();
	}

	void fill(const Point p2, double step)
	{
		if (m_points.empty())
			return;

		const Point p1(m_points.back());
		Point v(p2 - p1);
		double vx = v.x;
		double vy = v.y;
		double dist(distance(v));

		if (dist > step)
		{
			vx /= dist;
			vy /= dist;
		}

		double progress(0.0);
		while (progress + step < dist)
		{
			progress += step;

			double x = p1.x + vx * progress;
			double y = p1.y + vy * progress;

			addPoint(Point(x, y));
		}
	}

	void close(double step)
	{
		addPointInterpolate(m_points.front(), step, 1.0);
	}

private:
	std::vector<Point>  m_points;
	std::vector<double> m_distances;

	double m_distance_tot;

	void updatLastDistances()
	{
		std::size_t size(m_points.size());
		if (size > 1)
		{
			const Point& old_last(m_points[size - 2]);
			const Point& last(m_points.back());

			m_distances.back() = distance(last, old_last);
			m_distances.push_back(distance(m_points.front(), last));
		}
		else
		{
			m_distances.push_back(0.0);
		}
	}
};
