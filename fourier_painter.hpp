#pragma once

#include <SFML/Graphics.hpp>
#include "complex_wave.hpp"
#include <vector>

struct HarmonicCycloid
{
	HarmonicCycloid(double radius_, double phi_, double k_, double x_ = 0.0, double y_ = 0.0) :
		radius(radius_),
		phi(phi_),
		k(k_),
		x(x_),
		y(y_)
	{}

	double radius;
	double phi;
	double k;
	double x, y;
};

HarmonicCycloid complexWaveToCycloid(const ComplexWave& w)
{
	const complex& c(w.a);
	double radius(std::abs(c));
	double phi(std::arg(c));

	return HarmonicCycloid(radius, phi, w.k);
}

class FourierPainter
{
public:
	FourierPainter(sf::RenderTarget& target, const std::vector<Point>& signal, const std::vector<double>& dists) :
		m_harmonics_count(0),
		m_target(target),
		m_signal(signal),
		m_dists(dists),
		m_va(sf::LinesStrip, 0)
	{

	}

	void addHarmonic()
	{
		const uint32_t hc(getHarmonicsCount());
		if (!hc)
		{
			m_harmonics.emplace_back(m_signal, m_dists, 0);
			m_cycloids.push_back(complexWaveToCycloid(m_harmonics.back()));
		}
		else
		{
			int32_t ch(hc);
			m_harmonics.emplace_back(m_signal, m_dists,  ch);
			m_cycloids.push_back(complexWaveToCycloid(m_harmonics.back()));
			m_harmonics.emplace_back(m_signal, m_dists, -ch);
			m_cycloids.push_back(complexWaveToCycloid(m_harmonics.back()));
		}

		m_va.clear();
	}

	void delHarmonic()
	{
		const uint32_t hc(getHarmonicsCount());
		if (hc < 2)
		{
			m_harmonics.clear();
			m_cycloids.clear();
		}
		else
		{
			m_harmonics.pop_back();
			m_harmonics.pop_back();
			m_cycloids.pop_back();
			m_cycloids.pop_back();
		}

		m_va.clear();
	}

	void clear()
	{
		m_harmonics.clear();
		m_cycloids.clear();
		m_va.clear();
	}

	void notifySignalChanged()
	{
		const uint32_t hc(getHarmonicsCount());
		clear();
		for (uint32_t i(hc); i--;)
		{
			addHarmonic();
		}
	}

	void update(double time)
	{
		double x(0.0), y(0.0);
		for (HarmonicCycloid& cycloid : m_cycloids)
		{
			cycloid.x = x;
			cycloid.y = y;

			double xc(cycloid.k * time + cycloid.phi);
			x += cycloid.radius * cos(xc);
			y += cycloid.radius * sin(xc);
		}

		sf::Vertex v(sf::Vector2f(x, y), sf::Color::Red, sf::Vector2f());
		m_va.append(v);
	}

	void draw(const sf::RenderStates& rs) const
	{
		for (const HarmonicCycloid& cycloid : m_cycloids)
		{
			float radius(cycloid.radius);
			sf::CircleShape circle(radius);
			circle.setOutlineColor(sf::Color::Green);
			circle.setFillColor(sf::Color(0, 0, 0, 0));
			circle.setOutlineThickness(2.0f);
			circle.setOrigin(radius, radius);
			
			circle.setPosition(cycloid.x, cycloid.y);

			m_target.draw(circle, rs);
		}

		m_target.draw(m_va, rs);
	}

private:
	uint32_t m_harmonics_count;
	sf::RenderTarget& m_target;

	std::vector<ComplexWave> m_harmonics;
	std::vector<HarmonicCycloid> m_cycloids;
	const std::vector<Point>& m_signal;
	const std::vector<double>& m_dists;

	sf::VertexArray m_va;

	Point m_result;

	uint32_t getHarmonicsCount() const
	{
		std::size_t size(m_harmonics.size());

		if (size < 2)
		{
			return size;
		}

		return (size - 1) / 2;
	}
};

