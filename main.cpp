#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <iostream>
#include <dynamic_blur.hpp>
#include "signal_wave.hpp"
#include "complex_wave.hpp"
#include "fourier_painter.hpp"
#include <event_manager.hpp>

int main()
{
	const uint32_t win_width = 1000;
	const uint32_t win_height = 800;

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	sf::RenderWindow window(sf::VideoMode(win_width, win_height), "Octave", sf::Style::Default, settings);
	sfev::EventManager event_manager(window);
	const Point window_offset(win_width*0.5f, win_height*0.5f);
	window.setVerticalSyncEnabled(false);
	window.setFramerateLimit(60);

	std::vector<Point> signal;
	std::vector<double> distances;

	FourierPainter painter(window, signal, distances);

	bool clic = false;
	sf::Vector2i last_point(0, 0), current_mouse_pos(0, 0);

	// Event intialization
	event_manager.addEventCallback(sf::Event::Closed, [&](const sf::Event&) {window.close(); });
	event_manager.addMousePressedCallback(sf::Mouse::Left, [&](const sf::Event&) {clic = true; last_point = current_mouse_pos; });
	event_manager.addMouseReleasedCallback(sf::Mouse::Left, [&](const sf::Event&) {clic = false;
		const Point p0(current_mouse_pos.x, current_mouse_pos.y);
		const Point p1(p0 - window_offset);
		const Point& p2(signal.front());
		join(Point(p1.x, p1.y), p2, signal, distances, 1.0);
		painter.notifySignalChanged();
	});
	event_manager.addKeyReleasedCallback(sf::Keyboard::A, [&](const sf::Event&) {painter.delHarmonic(); });
	event_manager.addKeyReleasedCallback(sf::Keyboard::E, [&](const sf::Event&) {painter.addHarmonic(); });
	event_manager.addKeyReleasedCallback(sf::Keyboard::R, [&](const sf::Event&) {signal.clear(), distances.clear(); painter.clear(); });

	// Time initialization
	double t(0.0);

	while (window.isOpen())
	{
		t += 0.016;
		
		painter.update(t);

		current_mouse_pos = sf::Mouse::getPosition(window);
		event_manager.processEvents();

		if (clic)
		{
			Point p1(last_point.x, last_point.y);
			Point p2(current_mouse_pos.x, current_mouse_pos.y);
			
			Point v(p2 - p1);
			double dist(distance(v));

			if (dist > 1.0)
			{
				double x1 = last_point.x - win_width * 0.5;
				double y1 = last_point.y - win_height * 0.5;

				double x2 = current_mouse_pos.x - win_width * 0.5;
				double y2 = current_mouse_pos.y - win_height * 0.5;

				join(Point(x1, y1), Point(x2, y2), signal, distances, 1.0);
	
				last_point = current_mouse_pos;
			}

			painter.notifySignalChanged();
		}

		uint32_t signal_samples = signal.size();
		sf::VertexArray in_va(sf::LinesStrip, signal_samples + 1);
		if (signal_samples)
		{
			for (uint32_t i(0); i< signal_samples; ++i)
			{
				const Point& p(signal[i]);
				in_va[i].position = sf::Vector2f(p.x, p.y);
			}
			in_va[signal_samples].position = sf::Vector2f(signal[0].x, signal[0].y);
		}

		// Transforms
		sf::Transform tf_in;
		tf_in.translate(win_width * 0.5, win_height * 0.5);

		// Draw
		window.clear();

		window.draw(in_va, tf_in);
		painter.draw(tf_in);
		
		window.display();
	}

	return 0;
}