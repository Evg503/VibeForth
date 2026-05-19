// test_nested_loops.cpp
#include "forth_core.hpp"
#include <iostream>

int main() {
    try {
        Forth::ForthInterpreter forth;
        
        std::cout << "=== Testing Nested Loops ===" << std::endl;
        
        // Звездочки с вложенными циклами
        std::string stars = R"(
            : STARS ( n -- )
                0 DO 42 EMIT LOOP
            ;
            
            : TRIANGLE ( n -- )
                1+ 1 DO
                    I STARS CR
                LOOP
            ;
            
            5 TRIANGLE
        )";
        forth.evaluate(stars);
        
        // Матрица 5x5
        std::string matrix = R"(
            : MATRIX
                5 0 DO
                    5 0 DO
                         I J * 4 .R
                    LOOP
                    CR
                LOOP
            ;
            
            MATRIX
        )";
        forth.evaluate(matrix);
        
        std::cout << "\nNested loops test passed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}