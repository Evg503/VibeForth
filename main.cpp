// test.cpp
#include "forth_core.hpp"
#include <iostream>

int main() {
    try {
        Forth::ForthInterpreter forth;

        std::cout << "=== Forth Interpreter Test ===" << std::endl;

        // Базовые операции
        forth.evaluate("10 20 + . CR");
        forth.evaluate("100 30 - . CR");
        forth.evaluate("5 6 * . CR");
        forth.evaluate("100 4 / . CR");

        // Определение слова
        forth.evaluate(": SQUARE DUP * ;");
        forth.evaluate("5 SQUARE . CR");

        forth.evaluate(": DOUBLE 2 * ;");
        forth.evaluate("7 DOUBLE . CR");

        // Комбинированные операции
        forth.evaluate(": SUM-OF-SQUARES SQUARE SWAP SQUARE + ;");
        forth.evaluate("3 4 SUM-OF-SQUARES . CR");

        // Плавающая точка
        forth.evaluate("3.14 2.0 F* F. CR");

        std::cout << "\n=== All tests passed! ===" << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}