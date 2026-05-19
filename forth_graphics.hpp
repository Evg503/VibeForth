// forth_graphics.hpp
#pragma once
#include "forth_core.hpp"
#include <SFML/Graphics.hpp>

class ForthGraphics {
private:
    sf::RenderWindow* window = nullptr;
    Forth::ForthInterpreter& forth;

public:
    ForthGraphics(Forth::ForthInterpreter& f) : forth(f) {
        register_graphics_words();
    }

private:
    void register_graphics_words() {
        // Создание окна
        forth.register_extern("OPEN-WINDOW",
            reinterpret_cast<void*>(+[](Forth::ForthInterpreter& f) {
                auto height = f.data_stack.pop_int();
                auto width = f.data_stack.pop_int();
                auto* graphics = static_cast<ForthGraphics*>(f.get_user_data());
                graphics->window = new sf::RenderWindow(
                    sf::VideoMode(width, height),
                    "Forth Graphics"
                );
                }));

        // Очистка окна
        forth.register_extern("CLEAR",
            reinterpret_cast<void*>(+[](Forth::ForthInterpreter& f) {
                auto* graphics = static_cast<ForthGraphics*>(f.get_user_data());
                if (graphics->window)
                    graphics->window->clear(sf::Color::Black);
                }));

        // Отображение
        forth.register_extern("DISPLAY",
            reinterpret_cast<void*>(+[](Forth::ForthInterpreter& f) {
                auto* graphics = static_cast<ForthGraphics*>(f.get_user_data());
                if (graphics->window)
                    graphics->window->display();
                }));

        // Рисование круга
        forth.register_extern("CIRCLE",
            reinterpret_cast<void*>(+[](Forth::ForthInterpreter& f) {
                auto radius = f.data_stack.pop_float();
                auto y = f.data_stack.pop_float();
                auto x = f.data_stack.pop_float();

                auto* graphics = static_cast<ForthGraphics*>(f.get_user_data());
                if (graphics->window) {
                    sf::CircleShape circle(radius);
                    circle.setPosition(x - radius, y - radius);
                    circle.setFillColor(sf::Color::White);
                    graphics->window->draw(circle);
                }
                }));

        // Рисование прямоугольника
        forth.register_extern("RECT",
            reinterpret_cast<void*>(+[](Forth::ForthInterpreter& f) {
                auto height = f.data_stack.pop_float();
                auto width = f.data_stack.pop_float();
                auto y = f.data_stack.pop_float();
                auto x = f.data_stack.pop_float();

                auto* graphics = static_cast<ForthGraphics*>(f.get_user_data());
                if (graphics->window) {
                    sf::RectangleShape rect(sf::Vector2f(width, height));
                    rect.setPosition(x, y);
                    rect.setFillColor(sf::Color::White);
                    graphics->window->draw(rect);
                }
                }));

        // Установка цвета
        forth.register_extern("COLOR",
            reinterpret_cast<void*>(+[](Forth::ForthInterpreter& f) {
                auto b = f.data_stack.pop_int();
                auto g = f.data_stack.pop_int();
                auto r = f.data_stack.pop_int();
                // сохранение текущего цвета
                auto* graphics = static_cast<ForthGraphics*>(f.get_user_data());
                graphics->current_color = sf::Color(r, g, b);
                }));
    }

    sf::Color current_color{ sf::Color::White };
};

// Вспомогательный метод для ForthInterpreter
// Нужно добавить в класс ForthInterpreter:
/*
private:
    void* user_data = nullptr;
public:
    void set_user_data(void* data) { user_data = data; }
    void* get_user_data() { return user_data; }
*/