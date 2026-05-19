// forth_graphics.hpp
#pragma once
#include "forth_core.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <memory>
#include <cmath>

// Пространство имен для графики
namespace ForthGraphics {

    // Класс графического контекста
    class GraphicsContext {
    private:
        std::unique_ptr<sf::RenderWindow> window;
        sf::Color current_color{ sf::Color::White };
        bool window_open = false;
        std::vector<std::unique_ptr<sf::Drawable>> drawables;

    public:
        GraphicsContext() = default;
        ~GraphicsContext() = default;

        bool create_window(unsigned int width, unsigned int height, const std::string& title) {
            window = std::make_unique<sf::RenderWindow>(
                sf::VideoMode(width, height),
                title,
                sf::Style::Close | sf::Style::Titlebar
            );
            window_open = true;
            return true;
        }

        bool is_open() const {
            return window_open && window && window->isOpen();
        }

        void close() {
            if (window) {
                window->close();
                window_open = false;
            }
        }

        void clear() {
            if (window) {
                window->clear(current_color);
            }
        }

        void display() {
            if (window) {
                window->display();
            }
        }

        void set_color(uint8_t r, uint8_t g, uint8_t b) {
            current_color = sf::Color(r, g, b);
        }

        void draw_circle(float x, float y, float radius) {
            if (!window) return;

            auto circle = std::make_unique<sf::CircleShape>(radius);
            circle->setPosition(x - radius, y - radius);
            circle->setFillColor(current_color);
            window->draw(*circle);
            drawables.push_back(std::move(circle));
        }

        void draw_rectangle(float x, float y, float width, float height) {
            if (!window) return;

            auto rect = std::make_unique<sf::RectangleShape>(sf::Vector2f(width, height));
            rect->setPosition(x, y);
            rect->setFillColor(current_color);
            window->draw(*rect);
            drawables.push_back(std::move(rect));
        }

        void draw_line(float x1, float y1, float x2, float y2, float thickness = 1.0f) {
            if (!window) return;

            float dx = x2 - x1;
            float dy = y2 - y1;
            float length = std::sqrt(dx * dx + dy * dy);

            auto line = std::make_unique<sf::RectangleShape>(sf::Vector2f(length, thickness));
            float angle = std::atan2(dy, dx) * 180 / 3.14159265f;
            line->setPosition(x1, y1);
            line->setRotation(angle);
            line->setFillColor(current_color);
            window->draw(*line);
            drawables.push_back(std::move(line));
        }

        void handle_events() {
            if (!window) return;

            sf::Event event;
            while (window->pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window->close();
                    window_open = false;
                }
            }
        }
    };

    // Функция регистрации графических слов
    inline void register_graphics_words(Forth::ForthInterpreter& forth, GraphicsContext& graphics) {
        using namespace Forth;

        // Сохраняем указатель на графический контекст
        forth.set_user_data(&graphics);

        // Создание окна
        dictionary["OPEN-WINDOW"] = Word([&graphics](ForthInterpreter& f) {
            int height = f.get_data_stack().pop_int();
            int width = f.get_data_stack().pop_int();
            graphics.create_window(width, height, "Forth Graphics");
            });

        // Закрытие окна
        dictionary["CLOSE-WINDOW"] = Word([&graphics](ForthInterpreter& f) {
            graphics.close();
            });

        // Очистка экрана
        dictionary["CLEAR"] = Word([&graphics](ForthInterpreter& f) {
            graphics.clear();
            });

        // Отображение
        dictionary["DISPLAY"] = Word([&graphics](ForthInterpreter& f) {
            graphics.display();
            });

        // Установка цвета
        dictionary["COLOR"] = Word([&graphics](ForthInterpreter& f) {
            int b = f.get_data_stack().pop_int();
            int g = f.get_data_stack().pop_int();
            int r = f.get_data_stack().pop_int();
            graphics.set_color(r, g, b);
            });

        // Рисование круга
        dictionary["CIRCLE"] = Word([&graphics](ForthInterpreter& f) {
            double radius = f.get_data_stack().pop_float();
            double y = f.get_data_stack().pop_float();
            double x = f.get_data_stack().pop_float();
            graphics.draw_circle(x, y, radius);
            });

        // Рисование прямоугольника
        dictionary["RECT"] = Word([&graphics](ForthInterpreter& f) {
            double height = f.get_data_stack().pop_float();
            double width = f.get_data_stack().pop_float();
            double y = f.get_data_stack().pop_float();
            double x = f.get_data_stack().pop_float();
            graphics.draw_rectangle(x, y, width, height);
            });

        // Рисование линии
        dictionary["LINE"] = Word([&graphics](ForthInterpreter& f) {
            double thickness = f.get_data_stack().pop_float();
            double y2 = f.get_data_stack().pop_float();
            double x2 = f.get_data_stack().pop_float();
            double y1 = f.get_data_stack().pop_float();
            double x1 = f.get_data_stack().pop_float();
            graphics.draw_line(x1, y1, x2, y2, thickness);
            });

        // Обработка событий
        dictionary["PROCESS-EVENTS"] = Word([&graphics](ForthInterpreter& f) {
            graphics.handle_events();
            });

        // Проверка открытости окна
        dictionary["WINDOW-OPEN?"] = Word([&graphics](ForthInterpreter& f) {
            f.get_data_stack().push(graphics.is_open() ? 1 : 0);
            });
    }

} // namespace ForthGraphics