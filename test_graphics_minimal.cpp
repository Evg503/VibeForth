// test_graphics_minimal.cpp
#include "forth_core.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>

// Простой графический контекст
class SimpleGraphics {
public:
    sf::RenderWindow window;

    SimpleGraphics() : window(sf::VideoMode(800, 600), "Forth Graphics") {}

    void draw_circle(float x, float y, float r) {
        sf::CircleShape circle(r);
        circle.setPosition(x - r, y - r);
        circle.setFillColor(sf::Color::White);
        window.draw(circle);
    }

    void display() {
        window.display();
    }

    void clear() {
        window.clear(sf::Color::Black);
    }

    bool is_open() {
        return window.isOpen();
    }

    void handle_events() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }
};

int main() {
    try {
        SimpleGraphics gfx;

        // Простой анимированный круг
        float angle = 0;

        while (gfx.is_open()) {
            gfx.handle_events();
            gfx.clear();

            // Движущийся круг
            float x = 400 + 200 * sin(angle);
            float y = 300;
            gfx.draw_circle(x, y, 30);

            gfx.display();

            angle += 0.05f;
            sf::sleep(sf::milliseconds(16));
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}