#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <complex>
#include <iostream>
#include <dynamic_blur.hpp>
#include "signal_wave.hpp"
#include "complex_wave.hpp"

int main()
{
	const uint32_t win_width = 1000;
	const uint32_t win_height = 500;

	sf::RenderWindow window(sf::VideoMode(1000, 500), "Sign", sf::Style::Default);
	window.setVerticalSyncEnabled(true);

	std::vector<double> signal_x(win_width);
	for (uint32_t i(0); i < win_width; ++i)
	{
		signal_x[i] = rand() % 25 - 12 + 200*cos(i * 4*PI / double(win_width));
	}

	int32_t num_terms(1);

	std::vector<Wave> terms_x;

	uint32_t out_signal_sampling(win_width);

	bool clic = false;
	sf::Vector2i last_point(0, 0);

	while (window.isOpen())
	{
		sf::Vector2i current_mouse_pos = sf::Mouse::getPosition(window);
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			else if (event.type == sf::Event::MouseButtonPressed)
			{
				clic = true;
				last_point = current_mouse_pos;
			}
			else if (event.type == sf::Event::MouseButtonReleased)
			{
				clic = false;
			}
			else if (event.type == sf::Event::KeyReleased)
			{
				if (event.key.code == sf::Keyboard::A)
				{
					num_terms--;
				}
				else if (event.key.code == sf::Keyboard::E)
				{
					num_terms++;
				}
			}
		}

		if (clic)
		{
			if (current_mouse_pos.x > 0 && current_mouse_pos.x < win_width)
			{
				double vx = current_mouse_pos.x - last_point.x;
				double vy = current_mouse_pos.y - last_point.y;
				double dx = sign(vx);

				for (uint32_t i(last_point.x); i != current_mouse_pos.x; i += dx)
				{
					double ratio = (double(i) - last_point.x) / vx;
					signal_x[i] = last_point.y + vy * ratio - win_height / 2.0;
				}
			}
		}

		last_point = current_mouse_pos;

		uint32_t signal_samples = signal_x.size();
		//std::cout << signal_samples << std::endl;
		sf::VertexArray in_va(sf::LinesStrip, signal_samples);
		if (signal_samples)
		{
			for (uint32_t i(0); i< signal_samples; ++i)
			{
				in_va[i].position = sf::Vector2f(i, signal_x[i]);
			}
		}

		terms_x.clear();
		num_terms = num_terms < 1 ? 1 : num_terms;
		for (uint32_t i(0); i < num_terms; ++i)
		{
			terms_x.emplace_back(signal_x, i);
		}

		// Compute signal
		auto out_x = wavesToSignal(terms_x, out_signal_sampling);

		// Create out_va
		sf::VertexArray out_va(sf::LinesStrip, out_signal_sampling);
		for (uint32_t i(0); i<out_signal_sampling; ++i)
		{
			double x = out_x[i];

			out_va[i].position = sf::Vector2f(i, x);
			out_va[i].color = sf::Color::Green;
		}

		sf::Transform tf;
		tf.translate(0.0f, win_height*0.5f);

		window.clear();

		window.draw(in_va, tf);

		window.draw(out_va, tf);
		
		window.display();
	}

	return 0;
}