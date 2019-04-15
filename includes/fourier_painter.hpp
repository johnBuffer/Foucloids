#pragma once

#include <SFML/Graphics.hpp>
#include "complex_wave.hpp"
#include <vector>
#include <list>
#include "harmony_utils.hpp"

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

struct CycloidVertexArray
{
	CycloidVertexArray(sf::PrimitiveType pt, uint32_t size, uint32_t start) :
		va(pt, 0),
		va_size(size),
		offset(start)
	{}

	bool canRead()
	{
		return va.getVertexCount() == va_size;
	}

	void clear()
	{
		offset = 0;
		va.clear();
	}

	void addOrUpdate(uint32_t i, const sf::Vertex& v)
	{
		if (canRead())
		{
			uint32_t index((i - offset) % va_size);
			va[index] = v;
		}
		else
		{
			va.append(v);
		}
	}

	sf::VertexArray va;
	uint32_t va_size;
	uint32_t offset;
};

class FourierPainter
{
public:
	FourierPainter(sf::RenderTarget& target, const std::vector<Point>& signal, const std::vector<double>& dists) :
		m_harmonics_count(0),
		m_target(target),
		m_signal(signal),
		m_dists(dists),
		m_time(0.0),
		m_dt(0.016),
		m_current_point(0),
		m_va(sf::LinesStrip, getPointCount(), 0)
	{
	}

	void setDt(double dt)
	{
		m_dt = dt;
		
		resetTime();
	}

	void addHarmonic()
	{
		const uint32_t hc(getHarmonicsCount());
		if (!hc)
		{
			m_harmonics.emplace_back(m_signal, m_dists, 0);
			m_cycloids.push_back(complexWaveToCycloid(m_harmonics.back()));
			m_inter_va.emplace_back(sf::LinesStrip, getPointCount(), m_current_point);
		}
		else
		{
			int32_t ch(hc);
			m_harmonics.emplace_back(m_signal, m_dists,  ch);
			m_cycloids.push_back(complexWaveToCycloid(m_harmonics.back()));
			m_inter_va.emplace_back(sf::LinesStrip, getPointCount(), m_current_point);

			m_harmonics.emplace_back(m_signal, m_dists, -ch);
			m_cycloids.push_back(complexWaveToCycloid(m_harmonics.back()));
			m_inter_va.emplace_back(sf::LinesStrip, getPointCount(), m_current_point);
		}

		m_va.clear();
		m_va.offset = m_current_point;
	}

	void delHarmonic()
	{
		resetTime();
		const uint32_t hc(getHarmonicsCount());
		if (hc < 2)
		{
			m_harmonics.clear();
			m_cycloids.clear();
			m_inter_va.pop_back();
		}
		else
		{
			m_harmonics.pop_back();
			m_harmonics.pop_back();
			m_cycloids.pop_back();
			m_cycloids.pop_back();
			m_inter_va.pop_back();
			m_inter_va.pop_back();
		}

		m_va.clear();
	}

	void clear()
	{
		m_harmonics.clear();
		m_cycloids.clear();
		m_va.clear();
		m_inter_va.clear();

		resetTime();
	}

	void resetTime()
	{
		m_time = 0.0;
		m_current_point = 0;

		m_va.clear();
		m_va.va_size = getPointCount();
		for (auto& cva : m_inter_va)
		{
			cva.clear();
			cva.va_size = getPointCount();
		}
	}

	void notifySignalChanged()
	{
		const uint32_t hc(getHarmonicsCount());
		clear();
		for (uint32_t i(hc); i--;)
		{
			addHarmonic();
		}

		resetTime();
		update();
	}

	void update()
	{
		uint32_t n_points(Consts::TWO_PI / m_dt);

		float cc(m_cycloids.size());
		uint32_t i(0);
		double x(0.0), y(0.0);
		for (HarmonicCycloid& cycloid : m_cycloids)
		{
			cycloid.x = x;
			cycloid.y = y;

			double xc(cycloid.k * m_time + cycloid.phi);
			x += cycloid.radius * cos(xc);
			y += cycloid.radius * sin(xc);

			float ratio(i / cc);
			CycloidVertexArray& current_cva(m_inter_va[i]);
			current_cva.addOrUpdate(m_current_point, sf::Vertex(sf::Vector2f(x, y), sf::Color(255 * ratio, 128 * ratio, 0)));
			++i;
		}

		m_result = Point(x, y);
		m_va.addOrUpdate(m_current_point, sf::Vertex(sf::Vector2f(x, y), sf::Color(255, 128, 0)));

		advanceTime();
	}

	void draw(const sf::RenderStates& rs) const
	{
		for (const auto& cva : m_inter_va)
		{
			m_target.draw(cva.va, rs);
		}

		const sf::Color arms_color(255, 200, 75);

		sf::VertexArray arms(sf::LinesStrip, m_cycloids.size() + 2);
		arms[0].position = sf::Vector2f(0.0f, 0.0f);
		arms[0].color = arms_color;

		uint32_t i(0);
		for (const HarmonicCycloid& cycloid : m_cycloids)
		{
			float radius(cycloid.radius);
			sf::CircleShape circle = getCircle(radius, cycloid.x, cycloid.y, 1.0f, sf::Color(100, 100, 100));
			sf::CircleShape arms_join = getDisc(4.0f, cycloid.x, cycloid.y, sf::Color::Yellow);

			arms[i+1].position = sf::Vector2f(cycloid.x, cycloid.y);
			arms[i+1].color = arms_color;

			m_target.draw(circle, rs);
			m_target.draw(arms_join, rs);
			++i;
		}

		arms[i+1].position = sf::Vector2f(m_result.x, m_result.y);
		arms[i+1].color = arms_color;

		m_target.draw(arms, rs);
		m_target.draw(m_va.va, rs);

		sf::CircleShape marker = getDisc(4.0f, m_result.x, m_result.y, sf::Color(255, 128, 0));
		m_target.draw(marker, rs);
	}

private:
	uint32_t m_harmonics_count;
	sf::RenderTarget& m_target;

	std::vector<ComplexWave> m_harmonics;
	std::vector<HarmonicCycloid> m_cycloids;
	const std::vector<Point>& m_signal;
	const std::vector<double>& m_dists;

	double m_time;
	double m_dt;
	uint32_t m_current_point;

	CycloidVertexArray m_va;
	std::vector<CycloidVertexArray> m_inter_va;

	Point m_result;

	// Pivate methods
	uint32_t getPointCount() const
	{
		return uint32_t(Consts::TWO_PI / m_dt) - 1U;
	}

	void advanceTime()
	{
		m_time += m_dt;
		++m_current_point;
	}

	uint32_t getHarmonicsCount() const
	{
		std::size_t size(m_harmonics.size());

		if (size < 2)
		{
			return size;
		}

		return (size - 1) / 2 + 1;
	}
};

