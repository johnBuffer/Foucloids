#pragma once

#include <SFML/Graphics.hpp>
#include "complex_wave.hpp"
#include <vector>


struct HarmonicCycloid
{
	HarmonicCycloid(double radius_, double phi_, int32_t k_, double x = 0.0, double y = 0.0) :
		radius(radius_),
		phi(phi_),
		k(k_),
		pos(x, y)
	{}

	double radius;
	double phi;
	int32_t k;
	Point pos;
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

	void clear(uint32_t offset_)
	{
		offset = offset_;
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

struct ComplexCoef
{
	ComplexCoef() :
		cycloid(0.0, 0.0, 0),
		cva(sf::LinesStrip, 0, 0)
	{}

	ComplexCoef(const Signal2D& signal, int32_t k, uint32_t va_size, uint32_t start) :
		cycloid(complexWaveToCycloid(ComplexWave(signal, k))),
		cva(sf::LinesStrip, va_size, start)
	{}

	HarmonicCycloid cycloid;
	CycloidVertexArray cva;
};

class FourierPainter
{
public:
	FourierPainter(sf::RenderTarget& target, const Signal2D& signal) :
		m_harmonics_count(0),
		m_target(target),
		m_signal(signal),
		draw_harmonics(true),
		draw_arms(true),
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
		const std::size_t hc(getHarmonicsCount());
		if (!hc)
		{
			addCoef(0);
		} else {
			int32_t ch((int32_t)hc);
			addCoef( ch);
			addCoef(-ch);
		}
		std::sort(m_coefs.begin(), m_coefs.end(), [&](const ComplexCoef& c1, const ComplexCoef& c2) {return c1.cycloid.radius > c2.cycloid.radius; });
		update();
		m_va.clear(m_current_point);
	}

	void delHarmonic()
	{
		const std::size_t hc(getHarmonicsCount());
		if (hc < 2) {
			m_coefs.clear();
		} else {
			int32_t ch((int32_t)(hc - 1));
			remove( ch);
			remove(-ch);
		}
	}

	void clear()
	{
		m_coefs.clear();
		resetTime();
	}

	void resetTime()
	{
		m_time = 0.0;
		m_current_point = 0;
		m_va.clear(m_current_point);
		for (ComplexCoef& coef : m_coefs)
		{
			coef.cva.clear(m_current_point);
		}
	}

	void notifySignalChanged()
	{
		clear();
	}

	void update()
	{
		uint32_t n_points(static_cast<uint32_t>(Consts::TWO_PI / m_dt));
		std::size_t cc(m_coefs.size());
		uint32_t i(0);
		Point pt(0.0, 0.0);
		for (ComplexCoef& coef : m_coefs)
		{
			HarmonicCycloid& cycloid(coef.cycloid);
			double xc(cycloid.k * m_time + cycloid.phi);
			
			cycloid.pos = pt;
			pt += Point(cycloid.radius * cos(xc), cycloid.radius * sin(xc));

			float ratio(i / float(cc));
			CycloidVertexArray& current_cva(coef.cva);
			current_cva.addOrUpdate(m_current_point, sf::Vertex(toV2f(pt), sf::Color(uint8_t(255.0f * ratio), uint8_t(128.0f * ratio), 0)));
			++i;
		}

		m_va.addOrUpdate(m_current_point, sf::Vertex(toV2f(pt), sf::Color(255, 128, 0)));
		m_result = pt;

		advanceTime();
	}

	void draw(const sf::RenderStates& rs) const
	{
		const sf::Color arms_color(255, 200, 75);
		sf::VertexArray arms(sf::LinesStrip, m_coefs.size() + 2);
		arms[0].position = sf::Vector2f(0.0f, 0.0f);
		arms[0].color = arms_color;

		uint32_t i(0);
		for (const ComplexCoef& coef : m_coefs)
		{
			const HarmonicCycloid& cycloid(coef.cycloid);
			double radius(cycloid.radius);

			arms[i+1].position = toV2f(cycloid.pos);
			arms[i+1].color = arms_color;

			if (draw_arms) {
				m_target.draw(getCircle(float(radius), cycloid.pos, 2.0f, sf::Color(100, 100, 100)), rs);
				m_target.draw(getDisc(4.0f, cycloid.pos, sf::Color::Yellow), rs);
			}

			if (draw_harmonics) {
				m_target.draw(coef.cva.va, rs);
			}

			++i;
		}

		arms[i+1].position = toV2f(m_result);
		arms[i+1].color = arms_color;

		if (draw_arms)
		{
			m_target.draw(arms, rs);
			m_target.draw(getDisc(4.0f, m_result, sf::Color(255, 128, 0)), rs);
		}

		m_target.draw(m_va.va, rs);
	}

	const Point& getResult() const
	{
		return m_result;
	}

	const double getTime() const
	{
		return m_time;
	}

	bool draw_harmonics, draw_arms;

private:
	uint32_t m_harmonics_count;
	sf::RenderTarget& m_target;

	std::vector<ComplexCoef> m_coefs;
	const Signal2D& m_signal;

	double m_time;
	double m_dt;
	uint32_t m_current_point;

	CycloidVertexArray m_va;

	Point m_result;

	// Pivate methods
	uint32_t getPointCount() const
	{
		return uint32_t(Consts::TWO_PI / m_dt);
	}

	void advanceTime()
	{
		m_time += m_dt;
		++m_current_point;
	}

	std::size_t getHarmonicsCount() const
	{
		std::size_t size(m_coefs.size());
		if (size < 2) {
			return size;
		}
		return (size - 1) / 2 + 1;
	}

	void addCoef(int32_t c)
	{
		m_coefs.emplace_back(m_signal, c, getPointCount(), m_current_point);
	}

	void remove(int32_t k)
	{
		for (uint32_t i(0); i < m_coefs.size(); ++i) {
			if (m_coefs[i].cycloid.k == k) {
				m_coefs.erase(m_coefs.begin() + i);
				return;
			}
		}
	}
};

