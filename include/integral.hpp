#pragma once

#include <vector>
#include "consts.hpp"
#include "utils.hpp"
#include <ccomplex>

using complex = std::complex<double>;

complex computeIntegral(const std::vector<complex>& signal, const std::vector<double>& distances)
{
	const double fact = Consts::TWO_PI / sum(distances);

	uint32_t i(0);
	complex integral(0.0);
	for (const complex c : signal)
	{
		integral += c *distances[i];
		++i;
	}

	return fact * integral;
}
