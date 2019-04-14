#pragma once

#include <SFML/Graphics.hpp>
#include "signal_wave.hpp"
#include "point.hpp"

class WavePainter
{
public:
	WavePainter(const std::vector<Wave>& waves, float x, float y) :
		m_x(x),
		m_y(y),
		m_waves(waves)
	{}

	Point draw(double t, double base_angle, sf::RenderTarget& target)
	{
		double x(m_x);
		double y(m_y);

		uint32_t i(0);
		for (const Wave& wave : m_waves)
		{
			const double radius = wave.A;
			const double phase = wave.phase;

			sf::CircleShape circle(radius);
			circle.setOutlineColor(sf::Color::Green);
			circle.setFillColor(sf::Color(0, 0, 0, 0));
			circle.setOutlineThickness(2.0f);
			circle.setOrigin(radius, radius);
			circle.setPosition(x, y);

			x += radius * cos(i * t + phase + base_angle);
			y += radius * sin(i * t + phase + base_angle);

			target.draw(circle);

			++i;
		}

		return Point(x, y);
	}

private:
	float m_x, m_y;
	const std::vector<Wave>& m_waves;
};