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
		const float c_radius(4.0f);
		sf::CircleShape circle(c_radius);
		circle.setFillColor(sf::Color::Yellow);
		circle.setOrigin(c_radius, c_radius);

		double x(m_x);
		double y(m_y);

		sf::VertexArray arms(sf::LinesStrip, m_waves.size() + 1);
		arms[0].position = sf::Vector2f(x, y);
		arms[0].color = sf::Color(150, 150, 150, 255);

		uint32_t i(0);
		for (const Wave& wave : m_waves)
		{
			const double radius = wave.A;
			const double phase = wave.phase;

			circle.setPosition(x, y);

			x += radius * cos(i * t + phase + base_angle);
			y += radius * sin(i * t + phase + base_angle);

			arms[i+1].position = sf::Vector2f(x, y);
			arms[i+1].color = sf::Color(150, 150, 150, 255);
			target.draw(circle);

			++i;
		}

		circle.setFillColor(sf::Color::Red);
		circle.setPosition(x, y);

		target.draw(arms);
		target.draw(circle);

		return Point(x, y);
	}

private:
	float m_x, m_y;
	const std::vector<Wave>& m_waves;
};