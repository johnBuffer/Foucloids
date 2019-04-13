#pragma once
#include <vector>

constexpr double PI = 3.141592653589793238462;

double sum(const std::vector<double>& in)
{
	double sum_vec = 0.0;
	for (const double& d : in)
	{
		sum_vec += d;
	}

	return sum_vec;
}

double computeIntegral(const std::vector<double>& signal, const std::vector<double>& distances)
{
	const double fact = 2.0 * PI / sum(distances);

	uint32_t i(0);
	double integral(0.0);
	for (const double f : signal)
	{
		integral += f * distances[i];
		++i;
	}

	return fact * integral;
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



