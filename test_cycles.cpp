// test_cycles.cpp
#include "forth_core.hpp"
#include <iostream>

int main() {
    try {
        Forth::ForthInterpreter forth;
        
        std::cout << "=== Testing Cycles ===" << std::endl;
        
        // Простой цикл
        forth.evaluate("10 0 DO I . LOOP CR");
        
        // Цикл с шагом
        forth.evaluate("10 0 DO I . 2 +LOOP CR");
        
        // Вложенные циклы
        forth.evaluate(R"(
            5 0 DO
                5 0 DO
                    I J * .
                LOOP
                CR
            LOOP
        )");
        
        // Сумма
        forth.evaluate(": SUM 0 SWAP 1+ 1 DO I + LOOP ; 10 SUM . CR");
        
        // Факториал
        forth.evaluate(": FACT 1 SWAP 1+ 1 DO I * LOOP ; 5 FACT . CR");
        
        std::cout << "\nAll tests passed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}