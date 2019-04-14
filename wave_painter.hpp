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

	void draw(double t, sf::RenderTarget& target)
	{
		double x(m_x);
		double y(m_y);

		uint32_t i(0);
		for (const ComplexWave& wave : m_waves)
		{
			double radius(wave.a.real());
			double phase(0.0);

			sf::CircleShape circle(radius);
			circle.setOutlineColor(sf::Color::Green);
			circle.setFillColor(sf::Color(0, 0, 0, 0));
			circle.setOutlineThickness(2.0f);
			circle.setOrigin(radius, radius);
			circle.setPosition(x, y);

			x += radius * cos((i + 1) * t);
			y += radius * sin((i + 1) * t);

			target.draw(circle);

			++i;
		}
	}

private:
	float m_x, m_y;
	const std::vector<ComplexWave>& m_waves;
};