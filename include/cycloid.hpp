#pragma once
#include <stdint.h>
#include "complex_coef.hpp"

class Cycloid
{
public:
	Cycloid(double radius, double phi, int32_t k, double x = 0.0, double y = 0.0) :
		m_radius(radius),
		m_phi(phi),
		m_k(k),
		m_position(x, y)
	{}

	template<typename T=double>
	T radius() const
	{
		return static_cast<T>(m_radius);
	}

	double phi() const
	{
		return m_phi;
	}

	int32_t k() const
	{
		return m_k;
	}

	const Point& position() const
	{
		return m_position;
	}

	void position(const Point& position)
	{
		m_position = position;
	}

	Point getPoint(double t) const
	{
		return Point(m_radius * cos(t), m_radius * sin(t));
	}

private:
	double m_radius;
	double m_phi;
	int32_t m_k;
	Point m_position;
};

Cycloid complexWaveToCycloid(const ComplexCoef& w)
{
	const complex& c(w.a);
	double radius(std::abs(c));
	double phi(std::arg(c));

	return Cycloid(radius, phi, w.k);
}

struct CycloidVertexArray
{
	CycloidVertexArray(sf::PrimitiveType pt, uint32_t size, uint32_t start) :
		va(pt, 0),
		va_size(size),
		offset(start),
		ratio(0.0f)
	{}

	bool canRead()
	{
		return va.getVertexCount() == va_size;
	}

	void clear(uint32_t offset_, uint32_t size)
	{
		offset = offset_;
		va_size = size;
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
	float ratio;
};

struct GraphicalCycloid
{
	GraphicalCycloid() :
		cycloid(0.0, 0.0, 0),
		cva(sf::LinesStrip, 0, 0)
	{}

	GraphicalCycloid(const Signal2D& signal, int32_t k, uint32_t va_size, uint32_t start) :
		cycloid(complexWaveToCycloid(ComplexCoef(signal, k))),
		cva(sf::LinesStrip, va_size, start)
	{}

	Cycloid cycloid;
	CycloidVertexArray cva;
};

