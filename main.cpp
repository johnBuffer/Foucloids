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
	//window.setFramerateLimit(60);

	std::vector<double> signal_x;
	std::vector<double> signal_y;
	std::vector<double> distances;

	int32_t num_terms(1);

	std::vector<Wave> terms_x;
	std::vector<Wave> terms_y;

	uint32_t out_signal_sampling(1024);

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
			float vx = last_point.x - current_mouse_pos.x;
			float vy = last_point.y - current_mouse_pos.y;

			double distance(sqrt(vx*vx + vy*vy));
			if (distance > 1.0)
			{
				signal_x.push_back(current_mouse_pos.x - win_width * 0.5f);
				signal_y.push_back(current_mouse_pos.y - win_height * 0.5f);
				distances.push_back(distance);

				last_point = current_mouse_pos;
			}
		}

		uint32_t signal_samples = signal_x.size();
		std::cout << signal_samples << std::endl;
		sf::VertexArray in_va(sf::LinesStrip, signal_samples + 1);
		if (signal_samples)
		{
			for (uint32_t i(0); i< signal_samples; ++i)
			{
				in_va[i].position = sf::Vector2f(signal_x[i], signal_y[i]);
			}
			in_va[signal_samples].position = sf::Vector2f(signal_x[0], signal_y[0]);
		}

		// Create out_va
		sf::VertexArray x_va(sf::LinesStrip, signal_samples);
		double x_x = 0.0;
		for (uint32_t i(0); i < signal_samples; ++i)
		{
			x_x += distances[i];
			x_va[i].position = sf::Vector2f(x_x, signal_x[i]*0.5);
			x_va[i].color = sf::Color::Red;
		}

		terms_x.clear();
		terms_y.clear();
		num_terms = num_terms < 1 ? 1 : num_terms;
		for (uint32_t i(0); i < num_terms; ++i)
		{
			terms_x.emplace_back(signal_x, distances, i);
			terms_y.emplace_back(signal_y, distances, i);
		}

		// Compute signal
		auto out_x = wavesToSignal(terms_x, out_signal_sampling);
		auto out_y = wavesToSignal(terms_y, out_signal_sampling);

		// Create out_va
		sf::VertexArray out_va(sf::LinesStrip, out_signal_sampling);
		for (uint32_t i(0); i<out_signal_sampling; ++i)
		{
			double x = out_x[i];
			double y = out_y[i];

			out_va[i].position = sf::Vector2f(y * 0.5, x * 0.5);
			out_va[i].color = sf::Color::Green;
		}

		sf::Transform tf_in;
		tf_in.translate(win_width * 0.5, win_height * 0.5);

		sf::Transform tf_out;
		tf_out.translate(0, win_height * 0.5);


		window.clear();

		window.draw(in_va, tf_in);
		window.draw(out_va, tf_in);
		
		window.display();
	}

	return 0;
}