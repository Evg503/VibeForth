// test_leave.cpp
#include "forth_core.hpp"
#include <iostream>

int main() {
    try {
        Forth::ForthInterpreter forth;
        
        std::cout << "=== Testing LEAVE ===" << std::endl;
        
        std::string code = R"(
            : FIND-FIRST ( n -- )
                100 0 DO
                    I DUP * 2* I + 1000 > IF
                        LEAVE
                    THEN
                    I .
                LOOP
                CR ." First number: " I . CR
            ;
            
            1000 FIND-FIRST
        )";
        forth.evaluate(code);
        
        std::cout << "LEAVE test passed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
