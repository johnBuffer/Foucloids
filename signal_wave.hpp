#pragma once
#include "utils.hpp"

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
			b = 0;
		}
		else
		{
			double sample_width = 2.0 * PI / double(in.size());
			std::vector<double> processed_in_a;
			std::vector<double> processed_in_b;

			uint32_t i(0);
			for (double f : in)
			{
				double t = i * sample_width;
				double val_a = f * cos(k*t);
				double val_b = f * cos(k*t + 0.5 * PI);

				processed_in_a.push_back(val_a);
				processed_in_b.push_back(val_b);

				++i;
			}

			double front_coef = 1.0 / PI;

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
		double sign_space = 2.0*PI / double(sampling);
		for (uint32_t i(0); i < sampling; ++i)
		{
			out[i] += wv.a * cos(i * sign_space * wv.k);
			out[i] += wv.b * cos(i * sign_space * wv.k + PI * 0.5);
		}
	}

	return out;
}

