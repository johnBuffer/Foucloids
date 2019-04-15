#pragma once

#include <vector>
#include "point.hpp"
#include "integral.hpp"

complex polarToComplex(double r, double o)
{
	return complex(r * cos(o), r * sin(o));
}

// Compute C[n]
struct ComplexWave
{
	ComplexWave(const std::vector<Point>& signal, const std::vector<double>& dists, int32_t k_) :
		k(k_)
	{
		const int32_t size(signal.size());
		std::vector<complex> processed(size);

		const double scale_factor = Consts::TWO_PI / sum(dists);

		double t(0.0);
		for (uint32_t i(0); i < size; ++i)
		{
			processed[i] = complex(signal[i]) * polarToComplex(1.0, -((t*scale_factor - Consts::PI)*k));
			t += dists[i];
		}

		double front_coef = Consts::INV_TWO_PI;

		a = front_coef * computeIntegral(processed, dists);
	}

	complex a;
	int32_t k;
};
