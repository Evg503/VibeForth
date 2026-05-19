// test_simple_graphics.cpp
#include "forth_core.hpp"
#include "forth_graphics.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    try {
        Forth::ForthInterpreter forth;
        ForthGraphics::GraphicsContext graphics;

        ForthGraphics::register_graphics_words(forth, graphics);

        // Создаем окно
        forth.evaluate("640 480 OPEN-WINDOW");

        // Рисуем простую фигуру
        std::string code = R"(
            CLEAR
            
            \ Рисуем солнце (желтый круг)
            255 255 0 COLOR
            320 240 80.0 CIRCLE
            
            \ Рисуем лучи (линии)
            255 255 0 COLOR
            320 240 520 240 3.0 LINE
            320 240 120 240 3.0 LINE
            320 240 320 420 3.0 LINE
            320 240 320 60 3.0 LINE
            
            \ Рисуем траву (зеленый прямоугольник)
            0 255 0 COLOR
            0 400 640 80 RECT
            
            \ Рисуем текст
            255 255 255 COLOR
            280 50 16 TEXT
            
            DISPLAY
        )";

        forth.evaluate(code);

        std::cout << "Window opened. Close it to continue..." << std::endl;

        // Ждем закрытия окна
        while (graphics.is_open()) {
            forth.evaluate("PROCESS-EVENTS");
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::cout << "Window closed. Exiting..." << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}