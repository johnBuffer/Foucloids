#pragma once

#include <vector>
#include "point.hpp"

complex polarToComplex(double r, double o)
{
	return complex(r * cos(o), r * sin(o));
}

// Compute C[n]
struct ComplexWave
{
	ComplexWave(const std::vector<Point>& signal, const std::vector<double>& dists, uint32_t k_) :
		k(k_)
	{
		const uint32_t size(signal.size());
		std::vector<complex> processed(size);

		double t(0.0);
		for (uint32_t i(0); i < size; ++i)
		{
			processed[i] = complex(signal[i]) * polarToComplex(1, -(t*k));
			t += dists[i];
		}

		a = computeIntegral(processed, dists);
	}

	complex a;
	uint32_t k;
};
