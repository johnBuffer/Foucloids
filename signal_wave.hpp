#pragma once
#include "utils.hpp"
#include <cmath>

struct Wave
{
	Wave() :
		k(0),
		a(0.0f),
		b(0.0f)
	{}

	Wave(const std::vector<double>& in, const std::vector<double>& dists, uint32_t k_) :
		k(k_)
	{
		if (!k)
		{
			a = 1.0 / (2.0 * PI) * computeIntegral(in, dists);
			b = 0.0;

			A = a;
			phase = b;

		} else {
			std::vector<double> processed_in_a;
			std::vector<double> processed_in_b;

			const double scale_factor = 2.0*PI / sum(dists);

			double x(0.0);
			uint32_t i(0);
			for (double f : in)
			{
				double t = x;
				double val_a = f * cos(k*t);
				double val_b = f * sin(k*t);

				processed_in_a.push_back(val_a);
				processed_in_b.push_back(val_b);

				x += dists[i] * scale_factor;
				++i;
			}

			double front_coef = 1.0 / PI;

			a = front_coef * computeIntegral(processed_in_a, dists);
			b = front_coef * computeIntegral(processed_in_b, dists);

			A = sign(a) * sqrt(a*a + b*b);
			phase = atan(-b / a);
		}
	}

	double A, phase;

	double a;
	double b;
	uint32_t k;
};

std::vector<double> wavesToSignal(const std::vector<Wave>& waves, uint32_t sampling)
{
	std::vector<double> out(sampling);
	for (double& f : out) { f = 0.0f; }

	for (const Wave& wv : waves)
	{
		const double ampl = sign(wv.a) * std::sqrt(wv.a*wv.a + wv.b*wv.b);
		const double phi = std::atan(-wv.b / wv.a);

		double sign_space = 2.0*PI / double(sampling);
		for (uint32_t i(0); i < sampling; ++i)
		{
			const double x = i * sign_space;
			const double cb = ampl * cos(x * wv.k + phi);
			out[i] += cb;
		}
	}

	return out;
}

