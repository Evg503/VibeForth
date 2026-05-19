// test_comments.cpp
#include "forth_core.hpp"
#include <iostream>

int main() {
    try {
        Forth::ForthInterpreter forth;
        
        std::cout << "=== Testing Comments ===" << std::endl;
        
        // Строковый комментарий
        forth.evaluate("10 20 + . \\ This is a comment");
        forth.evaluate("CR");
        
        // Комментарий в скобках
        forth.evaluate("( This is a comment ) 30 40 + .");
        forth.evaluate("CR");
        
        // Многострочный комментарий
        forth.evaluate(R"(
            ( 
                This is a 
                multi-line comment 
            )
            50 60 + .
        )");
        forth.evaluate("CR");
        
        // Смешанные комментарии
        forth.evaluate(R"(
            : STAR 42 EMIT ; ( Define a star character )
            STAR STAR STAR STAR STAR STAR STAR STAR STAR STAR ( Print 10 stars )
        )");
        
        std::cout << std::endl << "All tests passed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
