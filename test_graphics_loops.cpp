// test_graphics_loops.cpp
#include "forth_core.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

class GraphicsContext {
public:
    sf::RenderWindow window;
    sf::Color current_color{255, 255, 255};
    
    GraphicsContext() : window(sf::VideoMode(800, 600), "Forth Graphics with Loops") {}
    
    void clear() { window.clear(sf::Color::Black); }
    void display() { window.display(); }
    void set_color(int r, int g, int b) { current_color = sf::Color(r, g, b); }
    
    void draw_circle(float x, float y, float r) {
        sf::CircleShape circle(r);
        circle.setPosition(x - r, y - r);
        circle.setFillColor(current_color);
        window.draw(circle);
    }
    
    void draw_rectangle(float x, float y, float w, float h) {
        sf::RectangleShape rect(sf::Vector2f(w, h));
        rect.setPosition(x, y);
        rect.setFillColor(current_color);
        window.draw(rect);
    }
    
    void draw_line(float x1, float y1, float x2, float y2, float thickness) {
        float dx = x2 - x1, dy = y2 - y1;
        float len = std::sqrt(dx*dx + dy*dy);
        sf::RectangleShape line(sf::Vector2f(len, thickness));
        float angle = std::atan2(dy, dx) * 180 / 3.14159f;
        line.setPosition(x1, y1);
        line.setRotation(angle);
        line.setFillColor(current_color);
        window.draw(line);
    }
    
    void handle_events() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }
    
    bool is_open() { return window.isOpen(); }
};

int main() {
    try {
        Forth::ForthInterpreter forth;
        GraphicsContext gfx;
        
        // Регистрируем графические слова
        forth.get_dictionary()["CLEAR"] = Forth::Word([&gfx](Forth::ForthInterpreter& f) {
            gfx.clear();
        });
        
        forth.get_dictionary()["DISPLAY"] = Forth::Word([&gfx](Forth::ForthInterpreter& f) {
            gfx.display();
        });
        
        forth.get_dictionary()["COLOR"] = Forth::Word([&gfx](Forth::ForthInterpreter& f) {
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
        
        // Рисуем спираль с помощью цикла
        std::string spiral_code = R"(
            CLEAR
            
            : SPIRAL
                360 0 DO
                    I 3 MOD 0= IF
                        255 0 0 COLOR
                    ELSE I 3 MOD 1= IF
                        0 255 0 COLOR
                    ELSE
                        0 0 255 COLOR
                    THEN THEN
                    
                    I 2 * I COS 200 * 400 + 
                    I 2 * I SIN 200 * 300 + 
                    10 I 100 / 5 + LINE
                LOOP
            ;
            
            SPIRAL
            DISPLAY
        )";
        
        forth.evaluate(spiral_code);
        
        std::cout << "Window opened. Close it to exit..." << std::endl;
        
        while (gfx.is_open()) {
            gfx.handle_events();
            sf::sleep(sf::milliseconds(16));
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}