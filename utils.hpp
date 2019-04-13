#pragma once
#include <vector>

constexpr double PI = 3.141592653589793238462;

double computeIntegral(const std::vector<double>& signal, const std::vector<double>& distances)
{
	double sum = 0.0;

	double fact = 0.0;
	for (const double& d : distances)
	{
		fact += d;
	}

	fact = 2.0*PI / fact;

	uint32_t i(0);
	for (double f : signal)
	{
		sum += f * fact * distances[i++];
	}

	return sum;
}

