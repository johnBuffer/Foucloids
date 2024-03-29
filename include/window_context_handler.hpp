#pragma once
#include <SFML/Graphics.hpp>
#include "viewport_handler.hpp"
#include "event_manager.hpp"
#include "utils.hpp"


class WindowContextHandler;


class RenderContext
{
public:
    explicit
    RenderContext(sf::RenderWindow& window)
        : m_window(window)
        , m_viewport_handler(toVector2f(window.getSize()))
    {
    }

    void setFocus(sf::Vector2f focus)
    {
        m_viewport_handler.setFocus(focus);
    }

    void setZoom(float zoom)
    {
        m_viewport_handler.setZoom(zoom);
    }

    void setView(sf::Vector2f focus, float zoom)
    {
        m_viewport_handler.setView(focus, zoom);
    }
    
    void registerCallbacks(sfev::EventManager& event_manager, bool use_viewport_callbacks)
    {
        event_manager.addEventCallback(sf::Event::MouseMoved, [&](sfev::CstEv) {
            m_viewport_handler.setMousePosition(event_manager.getFloatMousePosition());
        });
        if (use_viewport_callbacks) {
            event_manager.addMousePressedCallback(sf::Mouse::Left, [&](sfev::CstEv) {
                m_viewport_handler.click(event_manager.getFloatMousePosition());
            });
            event_manager.addMouseReleasedCallback(sf::Mouse::Left, [&](sfev::CstEv) {
                m_viewport_handler.unclick();
            });
            event_manager.addEventCallback(sf::Event::MouseWheelScrolled, [&](sfev::CstEv e) {
                m_viewport_handler.wheelZoom(e.mouseWheelScroll.delta);
            });
        }
    }
    
    void drawDirect(const sf::Drawable& drawable)
    {
        m_window.draw(drawable);
    }
    
    void draw(const sf::Drawable& drawable, sf::RenderStates render_states = {})
    {
        const sf::Transform drawable_transform = render_states.transform;
        render_states.transform = m_viewport_handler.getTransform();
        render_states.transform.combine(drawable_transform);
        m_window.draw(drawable, render_states);
    }
    
    void clear(sf::Color color = sf::Color::Black)
    {
        m_window.clear(color);
    }
    
    void display()
    {
        m_window.display();
    }
    
private:
    sf::RenderWindow& m_window;
    ViewportHandler m_viewport_handler;
    
    friend class WindowContextHandler;
};


class WindowContextHandler
{
public:
    WindowContextHandler(const std::string& window_name,
                         sf::Vector2u window_size,
                         int32_t window_style = sf::Style::Default,
                         bool use_viewport_callbacks = true)
        : m_settings(0, 0, 4)
        , m_window(sf::VideoMode(window_size.x, window_size.y), window_name, window_style, m_settings)
        , m_event_manager(m_window, true)
        , m_render_context(m_window)
    {
        m_window.setFramerateLimit(60);
        m_window.setKeyRepeatEnabled(false);
        registerDefaultCallbacks(use_viewport_callbacks);
    }

    void registerDefaultCallbacks(bool use_viewport_callbacks)
    {
        m_event_manager.addEventCallback(sf::Event::Closed, [&](sfev::CstEv) { m_window.close(); });
        m_event_manager.addKeyPressedCallback(sf::Keyboard::Escape, [&](sfev::CstEv) { m_window.close(); });
        m_render_context.registerCallbacks(m_event_manager, use_viewport_callbacks);
    }
    
    void processEvents()
    {
        m_event_manager.processEvents();
    }
    
    bool isRunning() const
    {
        return m_window.isOpen();
    }
    
    bool run()
    {
        m_event_manager.processEvents();
        return isRunning();
    }

    sfev::EventManager& getEventManager()
    {
        return m_event_manager;
    }

    RenderContext& getRenderContext()
    {
        return m_render_context;
    }
    
    sf::Vector2f getWorldMousePosition() const
    {
        return m_render_context.m_viewport_handler.getMouseWorldPosition();
    }

    sf::RenderWindow& getWindow()
    {
        return m_window;
    }
    
private:
    sf::ContextSettings m_settings;
    sf::RenderWindow    m_window;
    sfev::EventManager  m_event_manager;
    RenderContext       m_render_context;
};
