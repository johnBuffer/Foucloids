#pragma once
#include <vector>
#include <numeric>
#include <complex>
#include <sstream>
#include "point.hpp"
#include "signal2D.hpp"
#include "consts.hpp"


using complex = std::complex<double>;


template<typename U, typename T>
U to(const T& v)
{
    return static_cast<U>(v);
}

template<typename T>
sf::Vector2f toVector2f(sf::Vector2<T> v)
{
    return {to<float>(v.x), to<float>(v.y)};
}

double sum(const std::vector<double>& in)
{
	return std::accumulate(in.begin(), in.end(), 0.0);
}

sf::Vector2f toV2f(const Point& p)
{
	return sf::Vector2f(float(p.x), float(p.y));
}

sf::CircleShape getDisc(float radius, const Point& p, const sf::Color& color)
{
	sf::CircleShape circle(radius);
	circle.setFillColor(color);
	circle.setOrigin(radius, radius);
	circle.setPosition(toV2f(p));

	return circle;
}

sf::CircleShape getCircle(float radius, const Point& p, float thickness, const sf::Color& color)
{
	sf::CircleShape circle = getDisc(radius, p, sf::Color(0, 0, 0, 0));
	circle.setPointCount(uint32_t(radius) * 16);
	circle.setOutlineThickness(thickness);
	circle.setOutlineColor(color);

	return circle;
}

template<typename T>
std::string toString(T d, int decimals = 2)
{
	std::string result;
	std::stringstream sx;
	sx << std::fixed << d;
	result = sx.str();

	size_t pos = result.find('.');
	if (pos == std::string::npos) {
		return result;
	} else if (!decimals) {
		return result.substr(0, pos);
	} else {
		if (pos + decimals + 1 < result.size()) {
			return result.substr(0, pos + decimals + 1);
		}
	}

	return result;
}

const sf::VertexArray generateVertexArray(const Signal2D& signal)
{
	const auto& points(signal.points());
	const std::size_t signal_samples(points.size());
	sf::VertexArray va(sf::LinesStrip, signal_samples + 1);
	if (signal_samples)
	{
		for (uint32_t i(0); i < signal_samples; ++i)
		{
			const Point& p(points[i]);

			va[i].position = sf::Vector2f(float(p.x), float(p.y));
		}
		va[signal_samples].position = toV2f(points[0]);
	}

	return va;
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

