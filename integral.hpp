#pragma once

#include <vector>
#include "consts.hpp"
#include <ccomplex>

using complex = std::complex<double>;

double computeIntegral(const std::vector<double>& signal, const std::vector<double>& distances)
{
	const double fact = Consts::TWO_PI / sum(distances);

	uint32_t i(0);
	double integral(0.0);
	for (const double f : signal)
	{
		integral += f * distances[i];
		++i;
	}

	return fact * integral;
}

complex computeIntegral(const std::vector<complex>& signal, const std::vector<double>& distances)
{
	const double fact = Consts::TWO_PI / sum(distances);

	uint32_t i(0);
	complex integral(0.0);
	for (const complex c : signal)
	{
		integral += c * distances[i];
		++i;
	}

	return fact * integral;
}
