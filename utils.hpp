#pragma once
#include <vector>

constexpr double PI = 3.141592653589793238462;

double computeIntegral(const std::vector<double>& signal, const std::vector<double>& distances)
{
	double sum = 0.0;

	uint32_t i(0);
	for (double f : signal)
	{
		sum += f * distances[i];
		++i;
	}

	return sum;
}

