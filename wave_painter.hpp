#pragma once

#include <SFML/Graphics.hpp>
#include "complex_wave.hpp"
#include <complex>

class WavePainter
{
public:
	WavePainter(const std::vector<ComplexWave>& waves, float x, float y) :
		m_x(x),
		m_y(y),
		m_waves(waves)
	{}

	Point draw(double t, sf::RenderTarget& target)
	{
		double x(m_x);
		double y(m_y);

		uint32_t i(0);
		for (uint32_t i(0); i<m_waves.size(); ++i)
		{
			const ComplexWave& w1(m_waves[i]);

			const double radius(std::abs(w1.a));
			const double phi(std::arg(w1.a) + Consts::PI);

			sf::CircleShape circle(radius);
			circle.setOutlineColor(sf::Color::Green);
			circle.setFillColor(sf::Color(0, 0, 0, 0));
			circle.setOutlineThickness(2.0f);
			circle.setOrigin(radius, radius);
			circle.setPosition(x, y);

			x += radius * cos(w1.k * t + phi);
			y += radius * sin(w1.k * t + phi);

			target.draw(circle);
		}

		return Point(x, y);
	}

private:
	float m_x, m_y;
	const std::vector<ComplexWave>& m_waves;
};