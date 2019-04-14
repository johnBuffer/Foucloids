#pragma once

#include "utils.hpp"
#include <SFML/Graphics.hpp>


sf::VertexArray generateInVa(const std::vector<double>& signal, const std::vector<double>& distances, double win_width, const sf::Color& color = sf::Color::Red)
{
	// Create out_va
	double signal_samples = signal.size();
	sf::VertexArray va(sf::LinesStrip, signal.size());
	double x_x = 0.0;
	double x_fact = win_width / sum(distances);
	for (uint32_t i(0); i < signal_samples; ++i)
	{
		x_x += distances[i];
		va[i].position = sf::Vector2f(x_x * x_fact, signal[i]);
		va[i].color = color;
	}

	return va;
}

sf::VertexArray generateDebugOutVa(const std::vector<double>& out_signal, double win_width, const sf::Color& color = sf::Color::Green)
{
	const uint32_t out_sampling(out_signal.size());
	sf::VertexArray va(sf::LinesStrip, out_sampling);
	double out_x_fact = win_width / double(out_sampling);
	for (uint32_t i(0); i < out_sampling; ++i)
	{
		double y = out_signal[i];

		va[i].position = sf::Vector2f(i*out_x_fact, y);
		va[i].color = color;
	}

	return va;
}

sf::VertexArray generateOutVa(const std::vector<double>& out_x, const std::vector<double>& out_y, uint32_t win_width)
{
	const uint32_t out_sampling = out_x.size();
	sf::VertexArray out_va(sf::LinesStrip, out_sampling);
	if (out_sampling)
	{
		double out_x_fact = win_width / double(out_sampling);
		for (uint32_t i(0); i < out_sampling; ++i)
		{
			double x = out_x[i];
			double y = out_y[i];

			out_va[i].position = sf::Vector2f(x, -y);
			out_va[i].color = sf::Color::Green;
		}

		out_va.append(sf::Vertex());
		out_va[out_sampling].position = sf::Vector2f(out_x[0], -out_y[0]);
		out_va[out_sampling].color = sf::Color::Green;
	}

	return out_va;
}

sf::VertexArray plot(const std::vector<double>& ys, double width, sf::Color color)
{
	uint32_t size(ys.size());
	sf::VertexArray va(sf::LinesStrip, size);
	double fact = width / double(size);
	uint32_t i(0);
	for (const double& y : ys)
	{
		va[i].position = sf::Vector2f(i*fact, y);
		va[i].color = color;
		++i;
	}

	return va;
}