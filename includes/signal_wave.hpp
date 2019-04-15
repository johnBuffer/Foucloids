#pragma once
#include "utils.hpp"
#include "integral.hpp"

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
			a = 1.0 / (Consts::TWO_PI) * computeIntegral(in, dists);
			b = 0;
		} else {
			std::vector<double> processed_in_a;
			std::vector<double> processed_in_b;

			const double scale_factor = Consts::TWO_PI / sum(dists);

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

			double front_coef = Consts::INV_PI;

			a = front_coef * computeIntegral(processed_in_a, dists);
			b = front_coef * computeIntegral(processed_in_b, dists);
		}
	}

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
		double sign_space = Consts::TWO_PI / double(sampling + 1);
		for (uint32_t i(0); i < sampling; ++i)
		{
			out[i] += wv.a * cos(i * sign_space * wv.k);
			out[i] += wv.b * sin(i * sign_space * wv.k);
		}
	}

	return out;
}

