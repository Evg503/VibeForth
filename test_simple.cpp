// test_simple.cpp
#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "Testing SFML directly..." << std::endl;

    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Test");

    // Рисуем простые фигуры
    sf::CircleShape circle(50.0f);
    circle.setFillColor(sf::Color::Red);
    circle.setPosition(375, 250);

    sf::RectangleShape rectangle(sf::Vector2f(100, 100));
    rectangle.setFillColor(sf::Color::Green);
    rectangle.setPosition(100, 100);

    sf::RectangleShape line(sf::Vector2f(200, 3));
    line.setFillColor(sf::Color::Blue);
    line.setPosition(300, 400);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);
        window.draw(circle);
        window.draw(rectangle);
        window.draw(line);
        window.display();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}