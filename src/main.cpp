#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <iostream>
#include "fourier_painter.hpp"
#include <event_manager.hpp>
#include <dynamic_blur.hpp>
#include "transition.hpp"

int main()
{
	// Initialize window
	const uint32_t win_width(1600);
	const uint32_t win_height(900);

	sf::RenderWindow window(sf::VideoMode(win_width, win_height), "Foucloids", sf::Style::Default);
	sfev::EventManager event_manager(window);
	const Point window_offset(win_width*0.5f, win_height*0.5f);
	window.setVerticalSyncEnabled(true);

	sf::RenderTexture main_renderer;
	main_renderer.create(win_width, win_height);

	Blur blur(win_width, win_height, 0.5f);

	Signal2D signal;

	FourierPainter painter(main_renderer, signal);

	Transition<float> zoom(1.0f);
	Transition<Point> focus(0.0f, 0.0f);

	bool slow(false);
	painter.setDt(0.016);
	bool draw_signal(true);

	bool clic = false;
	sf::Vector2i last_point(0, 0), current_mouse_pos(0, 0);

	// Event intialization
	event_manager.addEventCallback(sf::Event::Closed, [&](const sf::Event&) {window.close(); });
	event_manager.addMousePressedCallback(sf::Mouse::Left, [&](const sf::Event&) {clic = true; last_point = current_mouse_pos; });
	event_manager.addMouseReleasedCallback(sf::Mouse::Left, [&](const sf::Event&) {clic = false; signal.close(1.0);});
	event_manager.addKeyReleasedCallback(sf::Keyboard::A, [&](const sf::Event&) {painter.delHarmonic(); });
	event_manager.addKeyReleasedCallback(sf::Keyboard::E, [&](const sf::Event&) {painter.addHarmonic();});
	event_manager.addKeyReleasedCallback(sf::Keyboard::R, [&](const sf::Event&) {signal.clear(); painter.clear(); });
	event_manager.addKeyReleasedCallback(sf::Keyboard::S, [&](const sf::Event&) {draw_signal =!draw_signal; });
	event_manager.addKeyReleasedCallback(sf::Keyboard::Q, [&](const sf::Event&) {painter.draw_harmonics = !painter.draw_harmonics; });
	event_manager.addKeyReleasedCallback(sf::Keyboard::D, [&](const sf::Event&) {painter.draw_arms = !painter.draw_arms; });
	event_manager.addKeyReleasedCallback(sf::Keyboard::Space, [&](const sf::Event&) {slow = !slow; if (slow) { painter.setDt(0.0008); } else { painter.setDt(0.016); }});

	while (window.isOpen())
	{	
		painter.update();
		zoom.update();
		focus.update();

		current_mouse_pos = sf::Mouse::getPosition(window);
		event_manager.processEvents();

		if (clic)
		{
			Point p2(current_mouse_pos.x, current_mouse_pos.y);
			signal.addPointInterpolate(p2 - window_offset, 1.0, 1.0);
			painter.notifySignalChanged();
		}

		// Transforms
		sf::Transform tf_in;
		tf_in.translate(win_width * 0.5, win_height * 0.5);
		tf_in.scale(zoom, zoom);
		tf_in.translate(-toV2f(focus));
		if (slow)
		{
			zoom = 30.0f;
			zoom.speed(0.125f);
			focus = painter.getResult();
			focus.speed(5.0f);
		}
		else
		{
			zoom = 1.0f;
			zoom.speed(4.0f);
			focus = Point(0.0f, 0.0f);
			focus.speed(1.0f);
		}

		// Draw
		window.clear();
		main_renderer.clear();

		if (draw_signal)
			main_renderer.draw(generateVertexArray(signal), tf_in);

		painter.draw(tf_in);
		main_renderer.display();
		window.draw(sf::Sprite(main_renderer.getTexture()));
		window.draw(sf::Sprite(blur.apply(main_renderer.getTexture(), 1)), sf::BlendAdd);

		window.display();
	}

	return 0;
}