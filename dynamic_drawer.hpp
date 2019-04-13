#pragma once

#include <SFML/Graphics.hpp>
#include "signal_wave.hpp"

class DynamicDrawer
{
public:
	DynamicDrawer(const std::vector<Wave>& waves, float x, float y) :
		m_x(x),
		m_y(y),
		m_waves(waves)
	{}

	double draw(double t, sf::RenderTarget& target)
	{
		double x(m_x);
		double y(m_y);

		uint32_t i(0);
		for (const Wave wave : m_waves)
		{
			const float radius(wave.a);
			sf::CircleShape circle(radius);
			circle.setOrigin(radius, radius);
			circle.setPosition(x, y);

			x += radius * cos(i * t);
			y += radius * sin(i * t);

			++i;
		}
	}

private:
	float m_x, m_y;
	const std::vector<Wave>& m_waves;
};