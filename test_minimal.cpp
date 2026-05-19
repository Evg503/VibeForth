// test_minimal.cpp
#include "forth_core.hpp"
#include <iostream>

int main() {
    try {
        Forth::ForthInterpreter forth;

        std::cout << "=== Minimal Forth Test ===" << std::endl;

        // Тестируем базовые операции
        forth.evaluate("10 20 + . CR");
        forth.evaluate(": SQUARE DUP * ;");
        forth.evaluate("5 SQUARE . CR");
        forth.evaluate("3.14 2.0 F+ F. CR");

        std::cout << "All basic tests passed!" << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}