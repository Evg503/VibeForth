// test_loops.cpp
#include "forth_core.hpp"
#include <iostream>

int main() {
    try {
        Forth::ForthInterpreter forth;
        
        std::cout << "=== Testing DO/LOOP Cycles ===" << std::endl;
        
        // Простой цикл от 0 до 9
        std::string code1 = R"(
            : TEST1
                10 0 DO
                    I .
                LOOP
            ;
            TEST1 CR
        )";
        forth.evaluate(code1);
        
        // Цикл с шагом 2
        std::string code2 = R"(
            : TEST2
                10 0 DO
                    I .
                    2 +LOOP
            ;
            TEST2 CR
        )";
        forth.evaluate(code2);
        
        // Сумма чисел от 1 до 10
        std::string code3 = R"(
            : SUM ( n -- sum )
                0 SWAP
                1+ 1 DO
                    I +
                LOOP
            ;
            10 SUM . CR
        )";
        forth.evaluate(code3);
        
        // Таблица умножения
        std::string code4 = R"(
            : MULT-TABLE
                10 1 DO
                    10 1 DO
                        I J * 4 .R
                    LOOP
                    CR
                LOOP
            ;
            MULT-TABLE
        )";
        forth.evaluate(code4);
        
        // Поиск простых чисел
        std::string code5 = R"(
            : IS-PRIME ( n -- flag )
                DUP 2 < IF DROP 0 EXIT THEN
                2
                BEGIN
                    2DUP MOD 0=
                WHILE
                    DROP 0 EXIT
                REPEAT
                1+ DUP * ROT > UNTIL
                DROP 1
            ;
            
            : PRIMES ( n -- )
                2 DO
                    I IS-PRIME IF I . SPACE THEN
                LOOP
            ;
            
            50 PRIMES CR
        )";
        forth.evaluate(code5);
        
        // Факториал через цикл
        std::string code6 = R"(
            : FACTORIAL ( n -- n! )
                1 SWAP
                1+ 1 DO
                    I *
                LOOP
            ;
            
            1 FACTORIAL . SPACE
            2 FACTORIAL . SPACE
            3 FACTORIAL . SPACE
            4 FACTORIAL . SPACE
            5 FACTORIAL . SPACE
            6 FACTORIAL . CR
        )";
        forth.evaluate(code6);
        
        std::cout << "\nAll cycle tests passed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}