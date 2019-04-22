#pragma once

#include <SFML/Graphics.hpp>
#include "complex_coef.hpp"
#include <vector>
#include "cycloid.hpp"
#include <iostream>

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
		const uint32_t point_count(getPointCount());
		m_va.clear(m_current_point, point_count);
		for (GraphicalCycloid& coef : m_coefs)
		{
			coef.cva.clear(m_current_point, point_count);
		}
	}

	void addHarmonic()
	{
		const std::size_t hc(getHarmonicsCount());
		if (!hc) {
			addCoef(0);
		} else {
			int32_t ch((int32_t)hc);
			addCoef( ch);
			addCoef(-ch);
		}
		std::sort(m_coefs.begin(), m_coefs.end(), [&](const GraphicalCycloid& c1, const GraphicalCycloid& c2) {return c1.cycloid.radius() > c2.cycloid.radius(); });
		update();
		m_va.clear(m_current_point, getPointCount());
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
		m_va.clear(m_current_point, getPointCount());
		for (GraphicalCycloid& coef : m_coefs)
		{
			coef.cva.clear(m_current_point, getPointCount());
		}
	}

	std::size_t getHarmonics() const
	{
		return getHarmonicsCount();
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
		for (GraphicalCycloid& coef : m_coefs)
		{
			Cycloid& cycloid(coef.cycloid);
			double xc(cycloid.k() * m_time + cycloid.phi());
			float ratio(i / float(cc));
			
			cycloid.position(pt);
			pt += cycloid.getPoint(xc);

			CycloidVertexArray& current_cva(coef.cva);
			current_cva.addOrUpdate(m_current_point, sf::Vertex(toV2f(pt), sf::Color(uint8_t(255.0f * ratio), uint8_t(128.0f * ratio), 0)));
			current_cva.ratio = ratio;
			++i;
		}

		m_va.addOrUpdate(m_current_point, sf::Vertex(toV2f(pt), sf::Color(255, 128, 0)));
		m_result = pt;

		advanceTime();
	}

	void draw(const sf::RenderStates& rs, bool zoom) const
	{
		const sf::Color arms_color(255, 200, 75);
		sf::VertexArray arms(sf::LinesStrip, m_coefs.size() + 2);
		arms[0].position = sf::Vector2f(0.0f, 0.0f);
		arms[0].color = arms_color;

		uint32_t i(0);
		for (const GraphicalCycloid& coef : m_coefs)
		{
			const Cycloid& cycloid(coef.cycloid);

			arms[i+1].position = toV2f(cycloid.position());
			arms[i+1].color = arms_color;

			if (draw_arms) {
				float r = zoom ? 0.0f : 1.0f;
				m_target.draw(getCircle(cycloid.radius<float>(), cycloid.position(), r + 2.0f / float(i + 1), sf::Color(100, 100, 100)), rs);
				m_target.draw(getDisc((7.0f + r) / float(i + 1), cycloid.position(), sf::Color::Yellow), rs);
			}

			if (draw_harmonics && coef.cva.ratio > 0.01f) {
				m_target.draw(coef.cva.va, rs);
			}

			++i;
		}

		arms[i+1].position = toV2f(m_result);
		arms[i+1].color = arms_color;

		if (draw_arms) {
			m_target.draw(arms, rs);
			m_target.draw(getDisc(2.0f / float(log(i+1)), m_result, sf::Color(255, 128, 0)), rs);
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

	std::vector<GraphicalCycloid> m_coefs;
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
			if (m_coefs[i].cycloid.k() == k) {
				m_coefs.erase(m_coefs.begin() + i);
				return;
			}
		}
	}
};

