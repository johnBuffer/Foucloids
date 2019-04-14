#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <iostream>
#include <event_manager.hpp>
#include "signal_wave.hpp"
#include "complex_wave.hpp"
#include "debug_visu.hpp"
#include "wave_painter.hpp"

int main()
{
	const uint32_t win_width = 1600;
	const uint32_t win_height = 900;

	sf::RenderWindow window(sf::VideoMode(win_width, win_height), "Octave", sf::Style::Default);
	window.setVerticalSyncEnabled(true);
	sfev::EventManager event_manager(window);
	const Point window_offset(win_width*0.5f, win_height*0.5f);

	// Signal inputs
	std::vector<double> signal_x;
	std::vector<double> signal_y;
	std::vector<double> distances;

	// Terms init
	int32_t num_terms(0);
	std::vector<Wave> terms_x;
	std::vector<Wave> terms_y;

	// Painter objects
	WavePainter painter_x(terms_x, win_width*0.5f, win_height*0.25f);
	WavePainter painter_y(terms_y, win_width*0.25f, win_height*0.5f);
	sf::VertexArray arms(sf::LineStrip, 3);
	sf::CircleShape marker(3.0);
	marker.setOrigin(3.0, 3.0);
	marker.setFillColor(sf::Color::Red);
	sf::VertexArray painter_va(sf::LinesStrip, 0);

	// The Holy Time
	double t(0.0);

	// Events handling
	bool clic = false;
	sf::Vector2i last_point(0, 0), current_mouse_pos(0, 0);
	event_manager.addEventCallback(sf::Event::Closed, [&](const sf::Event&) {window.close(); });
	event_manager.addMousePressedCallback(sf::Mouse::Left, [&](const sf::Event&) {clic = true; last_point = current_mouse_pos; });
	event_manager.addMouseReleasedCallback(sf::Mouse::Left, [&](const sf::Event&) {clic = false; 
		const Point p0(current_mouse_pos.x, current_mouse_pos.y);
		const Point p1(p0 - window_offset);
		const Point p2(signal_x[0], signal_y[0]);
		join(Point(p1.x, -p1.y), p2, signal_x, signal_y, distances, 1.0); 
	});
	event_manager.addKeyReleasedCallback(sf::Keyboard::A, [&](const sf::Event&) {num_terms -= 1; painter_va.clear(); });
	event_manager.addKeyReleasedCallback(sf::Keyboard::E, [&](const sf::Event&) {num_terms += 1; painter_va.clear(); });
	event_manager.addKeyReleasedCallback(sf::Keyboard::R, [&](const sf::Event&) {signal_x.clear(), signal_y.clear(), distances.clear(), painter_va.clear(); });

	while (window.isOpen())
	{
		t += 0.016;
		current_mouse_pos = sf::Mouse::getPosition(window);
		event_manager.processEvents();

		if (clic)
		{
			double vx = current_mouse_pos.x - last_point.x;
			double vy = current_mouse_pos.y - last_point.y;
			double distance(sqrt(vx*vx + vy*vy));

			if (distance > 1.0)
			{
				double x1 = last_point.x - win_width * 0.5;
				double y1 = last_point.y - win_height * 0.5;

				double x2 = current_mouse_pos.x - win_width * 0.5;
				double y2 = current_mouse_pos.y - win_height * 0.5;

				join(Point(x1, -y1), Point(x2, -y2), signal_x, signal_y, distances, 1.0);
	
				last_point = current_mouse_pos;
			}
		}

		uint32_t signal_samples = signal_x.size();
		sf::VertexArray in_va(sf::LinesStrip, signal_samples + 1);
		if (signal_samples)
		{
			for (uint32_t i(0); i< signal_samples; ++i)
			{
				in_va[i].position = sf::Vector2f(signal_x[i], -signal_y[i]);
				in_va[i].color = sf::Color::Green;
			}
			in_va[signal_samples].position = sf::Vector2f(signal_x[0], -signal_y[0]);
			in_va[signal_samples].color = sf::Color::Green;
		}

		terms_x.clear();
		terms_y.clear();
		num_terms = num_terms < 0 ? 0 : num_terms;
		for (uint32_t i(0); i < num_terms; ++i)
		{
			terms_x.emplace_back(signal_x, distances, i);
			terms_y.emplace_back(signal_y, distances, i);
		}

		// Transforms
		sf::Transform tf_in;
		tf_in.translate(win_width * 0.5, win_height * 0.5);
		
		// Draw
		window.clear();

		window.draw(in_va, tf_in);

		Point px = painter_x.draw(t, 0.0, window);
		Point py = painter_y.draw(t, -PI*0.5f, window);
		arms[0].position = sf::Vector2f(px.x, px.y);
		arms[1].position = sf::Vector2f(px.x, py.y);
		arms[2].position = sf::Vector2f(py.x, py.y);
		window.draw(arms);

		marker.setPosition(px.x, py.y);
		window.draw(marker);

		sf::Vertex vertex;
		vertex.color = sf::Color::Red;
		vertex.position = sf::Vector2f(px.x, py.y);
		painter_va.append(vertex);

		window.draw(painter_va);

		window.display();
	}

	return 0;
}