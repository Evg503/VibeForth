// test_graphics_no_comments.cpp
#include "forth_core.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <cmath>

class SimpleGraphics {
public:
    sf::RenderWindow window;
    sf::Color current_color{sf::Color::White};
    
    SimpleGraphics() : window(sf::VideoMode({800, 600}), "Forth Graphics") {}
    
    void draw_circle(float x, float y, float r) {
        sf::CircleShape circle(r);
        circle.setPosition({x - r, y - r});
        circle.setFillColor(current_color);
        window.draw(circle);
    }
    
    void draw_rectangle(float x, float y, float w, float h) {
        sf::RectangleShape rect(sf::Vector2f(w, h));
        rect.setPosition({x, y});
        rect.setFillColor(current_color);
        window.draw(rect);
    }
    
    void draw_line(float x1, float y1, float x2, float y2, float thickness) {
        float dx = x2 - x1;
        float dy = y2 - y1;
        float length = std::sqrt(dx*dx + dy*dy);
        sf::RectangleShape line(sf::Vector2f(length, thickness));
        float angle = std::atan2(dy, dx);
        line.setPosition({x1, y1});
        line.setRotation(sf::radians(angle));
        line.setFillColor(current_color);
        window.draw(line);
    }
    
    void set_color(int r, int g, int b) {
        current_color = sf::Color(r, g, b);
    }
    
    void clear() {
        window.clear(sf::Color::Black);
    }
    
    void display() {
        window.display();
    }
    
    void handle_events() {
        // ТАК НАДО В SFML 3
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            // Проверка событий, которые не несут в себе данных (например, закрытие окна)
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }
    }
    
    bool is_open() {
        return window.isOpen();
    }
};

int main() {
    try {
        Forth::ForthInterpreter forth;
        SimpleGraphics gfx;
        
        // Регистрируем графические слова
        forth.get_dictionary()["SET-COLOR"] = Forth::Word([&gfx](Forth::ForthInterpreter& f) {
            int b = f.get_data_stack().pop_int();
            int g = f.get_data_stack().pop_int();
            int r = f.get_data_stack().pop_int();
            gfx.set_color(r, g, b);
        });
        
        forth.get_dictionary()["CIRCLE"] = Forth::Word([&gfx](Forth::ForthInterpreter& f) {
            double r = f.get_data_stack().pop_float();
            double y = f.get_data_stack().pop_float();
            double x = f.get_data_stack().pop_float();
            gfx.draw_circle(x, y, r);
        });
        
        forth.get_dictionary()["RECT"] = Forth::Word([&gfx](Forth::ForthInterpreter& f) {
            double h = f.get_data_stack().pop_float();
            double w = f.get_data_stack().pop_float();
            double y = f.get_data_stack().pop_float();
            double x = f.get_data_stack().pop_float();
            gfx.draw_rectangle(x, y, w, h);
        });
        
        forth.get_dictionary()["LINE"] = Forth::Word([&gfx](Forth::ForthInterpreter& f) {
            double thick = f.get_data_stack().pop_float();
            double y2 = f.get_data_stack().pop_float();
            double x2 = f.get_data_stack().pop_float();
            double y1 = f.get_data_stack().pop_float();
            double x1 = f.get_data_stack().pop_float();
            gfx.draw_line(x1, y1, x2, y2, thick);
        });
        
        forth.get_dictionary()["CLEAR"] = Forth::Word([&gfx](Forth::ForthInterpreter& f) {
            gfx.clear();
        });
        
        forth.get_dictionary()["DISPLAY"] = Forth::Word([&gfx](Forth::ForthInterpreter& f) {
            gfx.display();
        });
        
        // Рисуем сцену
        forth.evaluate("CLEAR");
        forth.evaluate("255 0 0 SET-COLOR");
        forth.evaluate("400 300 100.0 CIRCLE");
        forth.evaluate("0 255 0 SET-COLOR");
        forth.evaluate("100 100 200.0 150.0 RECT");
        forth.evaluate("0 0 255 SET-COLOR");
        forth.evaluate("100 500 700 100 2.0 LINE");
        forth.evaluate("DISPLAY");
        
        // Анимация
        float angle = 0;
        while (gfx.is_open()) {
            gfx.handle_events();
            gfx.clear();
            
            // Статические элементы
            forth.evaluate("255 0 0 SET-COLOR 400 300 100.0 CIRCLE");
            forth.evaluate("0 255 0 SET-COLOR 100 100 200.0 150.0 RECT");
            
            // Анимированный круг
            float x = 400 + 200 * sin(angle);
            float y = 300 + 150 * cos(angle * 0.7);
            forth.evaluate("0 0 255 SET-COLOR " + 
                          std::to_string(x) + " " + 
                          std::to_string(y) + " 30.0 CIRCLE");
            
            gfx.display();
            angle += 0.05f;
            sf::sleep(sf::milliseconds(16));
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
