#pragma once
#include <vector>

constexpr double PI = 3.141592653589793238462;

double computeIntegral(const std::vector<double>& signal)
{
	double sum = 0.0;

	double sample_width = 2.0 * PI / double(signal.size());

	uint32_t i(0);
	for (double f : signal)
	{
		sum += f * sample_width;
		++i;
	}

	return sum;
}

double sign(double d)
{
	return d < 0.0 ? -1.0 : 1.0;
}

