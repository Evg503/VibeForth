// test_graphics.cpp
#include "forth_core.hpp"
#include "forth_graphics.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    try {
        std::cout << "=== Forth Graphics Test ===" << std::endl;

        Forth::ForthInterpreter forth;
        ForthGraphics::GraphicsContext graphics;

        // Регистрируем графические слова
        ForthGraphics::register_graphics_words(forth, graphics);

        // Создаем окно
        forth.evaluate("800 600 OPEN-WINDOW");

        if (!graphics.is_open()) {
            std::cerr << "Failed to create window!" << std::endl;
            return 1;
        }

        // Рисуем сцену на Forth
        std::string scene_code = R"(
            \ Clear screen with black
            CLEAR
            
            \ Set color to red and draw a circle
            255 0 0 COLOR
            400 300 100.0 CIRCLE
            
            \ Set color to green and draw a rectangle
            0 255 0 COLOR
            100 100 200.0 150.0 RECT
            
            \ Set color to blue and draw a line
            0 0 255 COLOR
            100 500 700 100 2.0 LINE
            
            \ Set color to yellow and draw some text
            255 255 0 COLOR
            350 50 20 TEXT
            
            \ Display everything
            DISPLAY
        )";

        forth.evaluate(scene_code);

        // Анимация - движущийся круг
        std::cout << "Running animation for 5 seconds..." << std::endl;

        for (int frame = 0; frame < 300; frame++) {
            if (!graphics.is_open()) break;

            // Обрабатываем события (закрытие окна)
            forth.evaluate("PROCESS-EVENTS");

            // Очищаем экран
            forth.evaluate("CLEAR");

            // Рисуем статические элементы
            forth.evaluate("255 0 0 COLOR 100 100 50.0 CIRCLE");  // Красный круг
            forth.evaluate("0 255 0 COLOR 500 500 100.0 100.0 RECT"); // Зеленый квадрат

            // Анимированный круг
            double x = 400 + 200 * sin(frame * 0.02);
            double y = 300 + 150 * cos(frame * 0.03);

            std::string animated_circle =
                "0 0 255 COLOR " +
                std::to_string(x) + " " +
                std::to_string(y) + " 30.0 CIRCLE";

            forth.evaluate(animated_circle);

            // Отображаем
            forth.evaluate("DISPLAY");

            // Небольшая задержка для плавности
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

        // Закрываем окно
        forth.evaluate("CLOSE-WINDOW");

        std::cout << "Graphics test completed!" << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}