#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <iostream>
#include "fourier_painter.hpp"
#include <event_manager.hpp>
#include "transition.hpp"
#include <sstream>
#include "window_context_handler.hpp"


int main()
{
	// Initialize window
	const uint32_t win_width(1600);
	const uint32_t win_height(900);

	WindowContextHandler app("Foucloids", { win_width, win_height }, sf::Style::Default, false);
	const Point window_offset(win_width * 0.5f, win_height * 0.5f);
	Signal2D signal;

	FourierPainter painter(signal);
	Transition<float> zoom(1.0f);
	Transition<Point> focus(0.0f, 0.0f);

	bool slow(false);
	painter.setDt(0.016);
	bool draw_signal(true);

	bool clic = false;
	sf::Vector2i last_point(0, 0), current_mouse_pos(0, 0);

	sf::Font font;
	font.loadFromFile("res/font.ttf");
	sf::Text text;
	text.setFont(font);
	text.setFillColor(sf::Color::White);
	text.setCharacterSize(18);

	// Event intialization
	app.getEventManager().addMousePressedCallback(sf::Mouse::Left, [&](const sf::Event&) {clic = true; last_point = current_mouse_pos; });
	app.getEventManager().addMouseReleasedCallback(sf::Mouse::Left, [&](const sf::Event&) {clic = false; signal.close(1.0);});
	app.getEventManager().addKeyReleasedCallback(sf::Keyboard::A, [&](const sf::Event&) {painter.addHarmonic(); });
	app.getEventManager().addKeyReleasedCallback(sf::Keyboard::E, [&](const sf::Event&) {painter.delHarmonic(); });
	app.getEventManager().addKeyReleasedCallback(sf::Keyboard::R, [&](const sf::Event&) {signal.clear(); painter.clear(); });
	app.getEventManager().addKeyReleasedCallback(sf::Keyboard::S, [&](const sf::Event&) {draw_signal =!draw_signal; });
	app.getEventManager().addKeyReleasedCallback(sf::Keyboard::Q, [&](const sf::Event&) {painter.draw_harmonics = !painter.draw_harmonics; });
	app.getEventManager().addKeyReleasedCallback(sf::Keyboard::D, [&](const sf::Event&) {painter.draw_arms = !painter.draw_arms; });
	app.getEventManager().addKeyReleasedCallback(sf::Keyboard::Space, [&](const sf::Event&) {slow = !slow; if (slow) {
		painter.setDt(0.0008);
		zoom = 10.0f;
		zoom.setSpeed(0.25f);
		focus.setSpeed(10.0f);
	} else { 
		painter.setDt(0.016);
		zoom = 1.0f;
		//zoom.setSpeed(1.0f);
		focus = Point(0.0f, 0.0f);
		focus.setSpeed(1.0f);
	}});

	while (app.run()) {
		painter.update();

		if (clic) {
			current_mouse_pos = sf::Mouse::getPosition(app.getWindow());
			Point p2(current_mouse_pos.x, current_mouse_pos.y);
			signal.addPointInterpolate(p2 - window_offset, 1.0, 1.0);
			painter.notifySignalChanged();
		}

		// Transforms
		RenderContext& render_context = app.getRenderContext();

		const Point current_focus(focus);
		render_context.setView(sf::Vector2f(to<float>(current_focus.x), to<float>(current_focus.y)), zoom);

		if (slow) {
			focus = painter.getResult();
		}

		// Draw
		render_context.clear();

		if (draw_signal) {
			render_context.draw(generateVertexArray(signal));
		}

		painter.draw(render_context, slow);

		text.setString("Harmonics: " + toString(painter.getHarmonics()) + "\n\nA: add harmonic\nE: remove harmonic\nS: show input signal\nMouse + clic: Edit input signal\nR: clear signal\nQ: show harmonics\nD: show cycloids\nSpace: zoom");
		text.setPosition(10.0f, 10.0f);
		render_context.drawDirect(text);

		render_context.display();
	}

	return 0;
}
