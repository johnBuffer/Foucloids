#pragma once

#include <vector>
#include "point.hpp"
#include "signal2D.hpp"
#include "utils.hpp"


complex polarToComplex(double r, double o)
{
	return complex(r * cos(o), r * sin(o));
}

// Compute C[n]
struct ComplexCoef
{
	ComplexCoef(const Signal2D& signal, int32_t k_) :
		k(k_)
	{
		const std::vector<Point>& signal_points(signal.points());
		const std::vector<double>& signal_dists(signal.distances());
		const std::size_t size(signal_points.size());
		std::vector<complex> processed(size);

		const double scale_factor = Consts::TWO_PI / sum(signal_dists);

		double t(0.0);
		for (std::size_t i(0); i < size; ++i) {
			processed[i] = complex(signal_points[i]) * polarToComplex(1.0, -((t*scale_factor - Consts::PI)*k));
			t += signal_dists[i];
		}

		double front_coef = Consts::INV_TWO_PI;

		a = front_coef * computeIntegral(processed, signal_dists);
	}

	complex a;
	int32_t k;
};
