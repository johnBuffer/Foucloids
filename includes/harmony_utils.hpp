#pragma once

#include <SFML/Graphics.hpp>
#include "utils.hpp"
#include "complex_wave.hpp"

struct HarmonyText
{
	HarmonyText(const ComplexWave& wave, const sf::Font& font) :
		HarmonyText(std::abs(wave.a), wave.k, std::abs(wave.k), font)
	{}

	HarmonyText(float amplitude, float frequence, float phase, const sf::Font& font) :
		a(amplitude),
		f(frequence),
		p(phase),
		m_removed(false)
	{
		text.setString(round(a, 2) + " cos(" + round(f, 0) + "*t + " + round(p, 2) + ")");
		text.setFont(font);
		text.setCharacterSize(20);
		text.setFillColor(sf::Color(255, 128, 0));
	}

	void setState(float x, float y, float tx, float ty)
	{
		m_x = x;
		m_y = y;

		m_init_x = x;
		m_init_y = y;
		
		m_tx = tx;
		m_ty = ty;

		text.setPosition(m_x, m_y);
	}

	void remove()
	{
		m_tx = m_init_x;
		m_ty = m_init_y;

		m_removed = true;
	}

	bool done() const
	{
		return  m_removed && std::abs(m_tx - m_x) < 0.1f;
	}

	void update(float dt)
	{
		m_x += (m_tx - m_x) * 0.1f * dt / 0.016f;
		m_y += (m_ty - m_y) * 0.1f * dt / 0.016f;

		text.setPosition(m_x, m_y);
	}

	void draw(sf::RenderTarget& target) const
	{
		target.draw(text);
	}

	float a;
	float f;
	float p;

private:
	float m_tx, m_ty;
	float m_init_x, m_init_y;

	bool m_removed;

	float m_x, m_y;
	sf::Text text;
};