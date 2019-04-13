#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <iostream>
#include <dynamic_blur.hpp>
#include "signal_wave.hpp"
#include "complex_wave.hpp"
#include "debug_visu.hpp"

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

	double r = 150.0;
	for (int32_t i(0); i < 1000; ++i)
	{
		double da = 2.0*PI / 1000.0;
		double angle = i * da;
		double x = rand()%100 - 50 + 1.2 * r * cos(angle);
		double y = 1.5 * r * sin(angle);

		signal_x.push_back(x);
		signal_y.push_back(y);
	}

	distances = computeDistances(signal_x, signal_y);

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
					num_terms-=10;
				}
				else if (event.key.code == sf::Keyboard::E)
				{
					num_terms+=10;
				}
				else if (event.key.code == sf::Keyboard::R)
				{
					signal_x.clear();
					signal_y.clear();
				}
			}
		}

		if (clic)
		{
			double vx = current_mouse_pos.x - last_point.x;
			double vy = current_mouse_pos.y - last_point.y;
			double distance(sqrt(vx*vx + vy*vy));

			if (distance > 1.0)
			{
				vx /= distance;
				vy /= distance;
				
				double progress(0.0);
				const double d_dist(1.0);
				while (progress + d_dist < distance)
				{
					progress += d_dist;

					double x = last_point.x + vx * progress - win_width  * 0.5;
					double y = last_point.y + vy * progress - win_height * 0.5;

					signal_x.push_back( x);
					signal_y.push_back(-y);
					distances.push_back(d_dist);
				}

				signal_x.push_back(  current_mouse_pos.x - win_width  * 0.5);
				signal_y.push_back(-(current_mouse_pos.y - win_height * 0.5));
				distances.push_back(distance - progress);

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
				in_va[i].position = sf::Vector2f(signal_x[i], -signal_y[i]);
			}
			in_va[signal_samples].position = sf::Vector2f(signal_x[0], -signal_y[0]);
		}

		uint32_t out_sampling(std::min(signal_samples, out_signal_sampling));

		// Create out_va
		sf::VertexArray x_va = generateInVa(signal_x, distances, win_width);

		terms_x.clear();
		terms_y.clear();
		num_terms = num_terms < 1 ? 1 : num_terms;
		for (uint32_t i(0); i < num_terms; ++i)
		{
			terms_x.emplace_back(signal_x, distances, i);
			terms_y.emplace_back(signal_y, distances, i);
		}

		// Compute signal
		auto out_x = wavesToSignal(terms_x, out_sampling);
		auto out_y = wavesToSignal(terms_y, out_sampling);

		// Create out_va
		sf::VertexArray debug_out_va = generateDebugOutVa(out_x, win_width);
		sf::VertexArray out_va = generateOutVa(out_x, out_y, win_width);

		// Transforms
		sf::Transform tf_in;
		tf_in.translate(win_width * 0.5, win_height * 0.5);
		sf::Transform tf_out;
		tf_out.translate(0, win_height * 0.5);

		// Draw
		window.clear();

		window.draw(in_va, tf_in);
		window.draw(x_va, tf_out);
		window.draw(out_va, tf_in);
		
		window.display();
	}

	return 0;
}