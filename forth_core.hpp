// forth_core.hpp - исправленная версия
#pragma once
#include <vector>
#include <stack>
#include <unordered_map>
#include <functional>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <cctype>
#include <iostream>
#include <cmath>
#include <memory>

namespace Forth {

using Cell = int64_t;
using Float = double;
using Addr = void*;

struct Value {
    enum Type { INT, FLOAT, PTR, WORD };
    Type type;
    union {
        Cell i;
        Float f;
        Addr p;
    };
    std::string word_name;
    
    Value() : type(INT), i(0) {}
    explicit Value(Cell v) : type(INT), i(v) {}
    explicit Value(Float v) : type(FLOAT), f(v) {}
    explicit Value(Addr v) : type(PTR), p(v) {}
};

class DataStack {
private:
    std::vector<Value> stack;
    
public:
    void push(const Value& v) { stack.push_back(v); }
    void push_int(Cell v) { stack.push_back(Value(v)); }
    void push_float(Float v) { stack.push_back(Value(v)); }
    void push_ptr(Addr v) { stack.push_back(Value(v)); }
    
    // Для удобства
    void push(Cell v) { push_int(v); }
    
    Value pop() {
        if (stack.empty()) throw std::runtime_error("Stack underflow");
        Value v = stack.back();
        stack.pop_back();
        return v;
    }
    
    Value top() const {
        if (stack.empty()) throw std::runtime_error("Stack underflow");
        return stack.back();
    }
    
    Cell pop_int() {
        Value v = pop();
        if (v.type != Value::INT) throw std::runtime_error("Not an integer");
        return v.i;
    }
    
    Float pop_float() {
        Value v = pop();
        if (v.type != Value::FLOAT) throw std::runtime_error("Not a float");
        return v.f;
    }
    
    Addr pop_ptr() {
        Value v = pop();
        if (v.type != Value::PTR) throw std::runtime_error("Not a pointer");
        return v.p;
    }
    
    size_t size() const { return stack.size(); }
    bool empty() const { return stack.empty(); }
    void clear() { stack.clear(); }
    
    void print() const {
        std::cout << "Stack: ";
        for (const auto& v : stack) {
            if (v.type == Value::INT) std::cout << v.i << " ";
            else if (v.type == Value::FLOAT) std::cout << v.f << " ";
            else if (v.type == Value::PTR) std::cout << "ptr ";
        }
        std::cout << std::endl;
    }
};

struct LoopState {
    size_t start_position;
    Cell index;
    Cell limit;
    Cell step;
    
    LoopState(size_t pos, Cell idx, Cell lim, Cell st = 1) 
        : start_position(pos), index(idx), limit(lim), step(st) {}
};

class Word {
public:
    using NativeFn = std::function<void(class ForthInterpreter&)>;
    
    enum Type { NATIVE, USER, NONE };
    
private:
    Type type_;
    NativeFn native_func_;
    std::vector<std::string> definition_;
    
public:
    Word() : type_(NONE) {}
    Word(NativeFn fn) : type_(NATIVE), native_func_(fn) {}
    Word(const std::vector<std::string>& def) : type_(USER), definition_(def) {}
    
    bool is_native() const { return type_ == NATIVE; }
    bool is_user() const { return type_ == USER; }
    bool is_none() const { return type_ == NONE; }
    
    NativeFn get_native() const {
        if (type_ != NATIVE) throw std::runtime_error("Not a native word");
        return native_func_;
    }
    
    const std::vector<std::string>& get_definition() const {
        if (type_ != USER) throw std::runtime_error("Not a user word");
        return definition_;
    }
    
    Type get_type() const { return type_; }
};

class ForthInterpreter {
private:
    DataStack data_stack;
    std::stack<Addr> return_stack;
    std::unordered_map<std::string, Word> dictionary;
    std::stack<LoopState> loop_stack;
    std::vector<std::string> current_tokens;
    size_t token_position;
    
    bool compiling = false;
    std::vector<std::string> current_definition;
    std::string current_word_name;
    void* user_data = nullptr;
    
public:
    ForthInterpreter() {
        init_primitive_words();
        init_math_words();
        init_stack_words();
        init_io_words();
        init_control_words();
        init_loop_words();
        init_compare_words();
    }
    
    void evaluate(const std::string& code) {
        auto tokens = tokenize(code);
        current_tokens = tokens;
        token_position = 0;
        interpret(current_tokens);
    }
    
    DataStack& get_data_stack() { return data_stack; }
    void set_user_data(void* data) { user_data = data; }
    void* get_user_data() { return user_data; }
    std::unordered_map<std::string, Word>& get_dictionary() { return dictionary; }
    
private:
    void init_primitive_words() {
        dictionary["DUP"] = Word([](ForthInterpreter& f) {
            f.data_stack.push(f.data_stack.top());
        });
        
        dictionary["DROP"] = Word([](ForthInterpreter& f) {
            f.data_stack.pop();
        });
        
        dictionary["SWAP"] = Word([](ForthInterpreter& f) {
            Value a = f.data_stack.pop();
            Value b = f.data_stack.pop();
            f.data_stack.push(a);
            f.data_stack.push(b);
        });
        
        dictionary["OVER"] = Word([](ForthInterpreter& f) {
            if (f.data_stack.size() < 2) throw std::runtime_error("Stack underflow");
            Value a = f.data_stack.pop();
            Value b = f.data_stack.top();
            f.data_stack.push(a);
            f.data_stack.push(b);
        });
        
        dictionary["ROT"] = Word([](ForthInterpreter& f) {
            if (f.data_stack.size() < 3) throw std::runtime_error("Stack underflow");
            Value a = f.data_stack.pop();
            Value b = f.data_stack.pop();
            Value c = f.data_stack.pop();
            f.data_stack.push(b);
            f.data_stack.push(a);
            f.data_stack.push(c);
        });
        
        dictionary["2DUP"] = Word([](ForthInterpreter& f) {
            if (f.data_stack.size() < 2) throw std::runtime_error("Stack underflow");
            Value a = f.data_stack.pop();
            Value b = f.data_stack.pop();
            f.data_stack.push(b);
            f.data_stack.push(a);
            f.data_stack.push(b);
            f.data_stack.push(a);
        });
        
        dictionary["+"] = Word([](ForthInterpreter& f) {
            Cell b = f.data_stack.pop_int();
            Cell a = f.data_stack.pop_int();
            f.data_stack.push(a + b);
        });
        
        dictionary["-"] = Word([](ForthInterpreter& f) {
            Cell b = f.data_stack.pop_int();
            Cell a = f.data_stack.pop_int();
            f.data_stack.push(a - b);
        });
        
        dictionary["*"] = Word([](ForthInterpreter& f) {
            Cell b = f.data_stack.pop_int();
            Cell a = f.data_stack.pop_int();
            f.data_stack.push(a * b);
        });
        
        dictionary["/"] = Word([](ForthInterpreter& f) {
            Cell b = f.data_stack.pop_int();
            Cell a = f.data_stack.pop_int();
            if (b == 0) throw std::runtime_error("Division by zero");
            f.data_stack.push(a / b);
        });
        
        dictionary["F+"] = Word([](ForthInterpreter& f) {
            Float b = f.data_stack.pop_float();
            Float a = f.data_stack.pop_float();
            f.data_stack.push_float(a + b);
        });
        
        dictionary["F-"] = Word([](ForthInterpreter& f) {
            Float b = f.data_stack.pop_float();
            Float a = f.data_stack.pop_float();
            f.data_stack.push_float(a - b);
        });
        
        dictionary["F*"] = Word([](ForthInterpreter& f) {
            Float b = f.data_stack.pop_float();
            Float a = f.data_stack.pop_float();
            f.data_stack.push_float(a * b);
        });
        
        dictionary["F/"] = Word([](ForthInterpreter& f) {
            Float b = f.data_stack.pop_float();
            Float a = f.data_stack.pop_float();
            if (b == 0.0) throw std::runtime_error("Division by zero");
            f.data_stack.push_float(a / b);
        });
        
        dictionary["NEGATE"] = Word([](ForthInterpreter& f) {
            f.data_stack.push(-f.data_stack.pop_int());
        });
        
        dictionary["ABS"] = Word([](ForthInterpreter& f) {
            Cell a = f.data_stack.pop_int();
            f.data_stack.push(a < 0 ? -a : a);
        });
        
        dictionary["MOD"] = Word([](ForthInterpreter& f) {
            Cell b = f.data_stack.pop_int();
            Cell a = f.data_stack.pop_int();
            if (b == 0) throw std::runtime_error("Division by zero");
            f.data_stack.push(a % b);
        });
    }
    
    void init_math_words() {
        dictionary["MAX"] = Word([](ForthInterpreter& f) {
            Cell b = f.data_stack.pop_int();
            Cell a = f.data_stack.pop_int();
            f.data_stack.push(a > b ? a : b);
        });
        
        dictionary["MIN"] = Word([](ForthInterpreter& f) {
            Cell b = f.data_stack.pop_int();
            Cell a = f.data_stack.pop_int();
            f.data_stack.push(a < b ? a : b);
        });
        
        dictionary["SIN"] = Word([](ForthInterpreter& f) {
            Float a = f.data_stack.pop_float();
            f.data_stack.push_float(std::sin(a));
        });
        
        dictionary["COS"] = Word([](ForthInterpreter& f) {
            Float a = f.data_stack.pop_float();
            f.data_stack.push_float(std::cos(a));
        });
    }
    
    void init_stack_words() {
        dictionary["DEPTH"] = Word([](ForthInterpreter& f) {
            f.data_stack.push((Cell)f.data_stack.size());
        });
        
        dictionary[".S"] = Word([](ForthInterpreter& f) {
            f.data_stack.print();
        });
        
        dictionary[">R"] = Word([](ForthInterpreter& f) {
            Addr a = f.data_stack.pop_ptr();
            f.return_stack.push(a);
        });
        
        dictionary["R>"] = Word([](ForthInterpreter& f) {
            if (f.return_stack.empty()) throw std::runtime_error("Return stack empty");
            Addr a = f.return_stack.top();
            f.return_stack.pop();
            f.data_stack.push_ptr(a);
        });
        
        dictionary["R@"] = Word([](ForthInterpreter& f) {
            if (f.return_stack.empty()) throw std::runtime_error("Return stack empty");
            f.data_stack.push_ptr(f.return_stack.top());
        });
    }
    
    void init_io_words() {
        dictionary["."] = Word([](ForthInterpreter& f) {
            std::cout << f.data_stack.pop_int();
            std::cout.flush();
        });
        
        dictionary["F."] = Word([](ForthInterpreter& f) {
            std::cout << f.data_stack.pop_float();
            std::cout.flush();
        });
        
        dictionary["CR"] = Word([](ForthInterpreter& f) {
            std::cout << std::endl;
        });
        
        dictionary["SPACE"] = Word([](ForthInterpreter& f) {
            std::cout << ' ';
        });
        
        dictionary["EMIT"] = Word([](ForthInterpreter& f) {
            std::cout << (char)f.data_stack.pop_int();
            std::cout.flush();
        });
        
        dictionary["SPACES"] = Word([](ForthInterpreter& f) {
            Cell n = f.data_stack.pop_int();
            for (Cell i = 0; i < n; i++) std::cout << ' ';
        });
        
        dictionary[".R"] = Word([](ForthInterpreter& f) {
            Cell width = f.data_stack.pop_int();
            Cell value = f.data_stack.pop_int();
            std::cout.width(width);
            std::cout << value;
        });
    }
    
    void init_compare_words() {
        dictionary["="] = Word([](ForthInterpreter& f) {
            Cell b = f.data_stack.pop_int();
            Cell a = f.data_stack.pop_int();
            f.data_stack.push_int(a == b ? -1 : 0);  // Исправлено: явный вызов push_int
        });
        
        dictionary["<"] = Word([](ForthInterpreter& f) {
            Cell b = f.data_stack.pop_int();
            Cell a = f.data_stack.pop_int();
            f.data_stack.push_int(a < b ? -1 : 0);   // Исправлено
        });
        
        dictionary[">"] = Word([](ForthInterpreter& f) {
            Cell b = f.data_stack.pop_int();
            Cell a = f.data_stack.pop_int();
            f.data_stack.push_int(a > b ? -1 : 0);   // Исправлено
        });
        
        dictionary["0="] = Word([](ForthInterpreter& f) {
            Cell a = f.data_stack.pop_int();
            f.data_stack.push_int(a == 0 ? -1 : 0);  // Исправлено
        });
        
        dictionary["0<"] = Word([](ForthInterpreter& f) {
            Cell a = f.data_stack.pop_int();
            f.data_stack.push_int(a < 0 ? -1 : 0);   // Исправлено
        });
        
        dictionary["0>"] = Word([](ForthInterpreter& f) {
            Cell a = f.data_stack.pop_int();
            f.data_stack.push_int(a > 0 ? -1 : 0);   // Исправлено
        });
        
        dictionary["AND"] = Word([](ForthInterpreter& f) {
            Cell b = f.data_stack.pop_int();
            Cell a = f.data_stack.pop_int();
            f.data_stack.push_int(a && b ? -1 : 0);
        });
        
        dictionary["OR"] = Word([](ForthInterpreter& f) {
            Cell b = f.data_stack.pop_int();
            Cell a = f.data_stack.pop_int();
            f.data_stack.push_int(a || b ? -1 : 0);
        });
        
        dictionary["NOT"] = Word([](ForthInterpreter& f) {
            Cell a = f.data_stack.pop_int();
            f.data_stack.push_int(a ? 0 : -1);
        });
    }
    
    void init_control_words() {
        dictionary["IF"] = Word([this](ForthInterpreter& f) {
            Cell condition = f.data_stack.pop_int();
            if (!condition) {
                int if_depth = 1;
                while (f.token_position < f.current_tokens.size() && if_depth > 0) {
                    f.token_position++;
                    if (f.token_position >= f.current_tokens.size()) break;
                    if (f.current_tokens[f.token_position] == "IF") if_depth++;
                    else if (f.current_tokens[f.token_position] == "THEN") if_depth--;
                    else if (f.current_tokens[f.token_position] == "ELSE" && if_depth == 1) {
                        // Перепрыгиваем ELSE
                        if_depth = 0;
                    }
                }
            }
            f.token_position++;
        });
        
        dictionary["THEN"] = Word([](ForthInterpreter& f) {
            // Ничего не делаем
        });
        
        dictionary["ELSE"] = Word([this](ForthInterpreter& f) {
            int if_depth = 1;
            while (f.token_position < f.current_tokens.size() && if_depth > 0) {
                f.token_position++;
                if (f.current_tokens[f.token_position] == "IF") if_depth++;
                else if (f.current_tokens[f.token_position] == "THEN") if_depth--;
            }
        });
        
        dictionary["BEGIN"] = Word([this](ForthInterpreter& f) {
            f.return_stack.push(reinterpret_cast<Addr>(f.token_position));
            f.token_position++;
        });
        
        dictionary["UNTIL"] = Word([this](ForthInterpreter& f) {
            Cell condition = f.data_stack.pop_int();
            if (!condition) {
                size_t begin_pos = reinterpret_cast<size_t>(f.return_stack.top());
                f.token_position = begin_pos;
            } else {
                f.return_stack.pop();
                f.token_position++;
            }
        });
        
        dictionary["RECURSE"] = Word([this](ForthInterpreter& f) {
            if (f.current_word_name.empty()) {
                throw std::runtime_error("RECURSE outside of definition");
            }
            auto it = f.dictionary.find(f.current_word_name);
            if (it != f.dictionary.end() && it->second.is_user()) {
                f.interpret(it->second.get_definition());
            }
            f.token_position++;
        });
        
        dictionary["EXIT"] = Word([](ForthInterpreter& f) {
            // Выход из текущего слова
            throw std::runtime_error("EXIT");
        });
    }
    
    void init_loop_words() {
        dictionary["DO"] = Word([this](ForthInterpreter& f) {
            Cell limit = f.data_stack.pop_int();
            Cell index = f.data_stack.pop_int();
            f.loop_stack.emplace(f.token_position + 1, index, limit, 1);
            f.token_position++;
        });
        
        dictionary["LOOP"] = Word([this](ForthInterpreter& f) {
            if (f.loop_stack.empty()) {
                throw std::runtime_error("LOOP without DO");
            }
            
            LoopState& loop = f.loop_stack.top();
            loop.index += loop.step;
            
            if (loop.index < loop.limit) {
                f.token_position = loop.start_position;
            } else {
                f.loop_stack.pop();
                f.token_position++;
            }
        });
        
        dictionary["+LOOP"] = Word([this](ForthInterpreter& f) {
            if (f.loop_stack.empty()) {
                throw std::runtime_error("+LOOP without DO");
            }
            
            Cell step = f.data_stack.pop_int();
            LoopState& loop = f.loop_stack.top();
            loop.index += step;
            
            bool exit_loop;
            if (step > 0) {
                exit_loop = loop.index >= loop.limit;
            } else {
                exit_loop = loop.index <= loop.limit;
            }
            
            if (!exit_loop) {
                f.token_position = loop.start_position;
            } else {
                f.loop_stack.pop();
                f.token_position++;
            }
        });
        
        dictionary["I"] = Word([](ForthInterpreter& f) {
            if (f.loop_stack.empty()) {
                throw std::runtime_error("I used outside of DO loop");
            }
            f.data_stack.push_int(f.loop_stack.top().index);
        });
        
        dictionary["J"] = Word([](ForthInterpreter& f) {
            if (f.loop_stack.size() < 2) {
                throw std::runtime_error("J used without nested loop");
            }
            std::stack<LoopState> temp;
            for (size_t i = 0; i < 2; i++) {
                temp.push(f.loop_stack.top());
                f.loop_stack.pop();
            }
            Cell j_index = temp.top().index;
            f.loop_stack.push(temp.top()); temp.pop();
            f.loop_stack.push(temp.top()); temp.pop();
            f.data_stack.push_int(j_index);
        });
        
        dictionary["LEAVE"] = Word([this](ForthInterpreter& f) {
            if (f.loop_stack.empty()) {
                throw std::runtime_error("LEAVE outside of DO loop");
            }
            f.loop_stack.pop();
            int loop_depth = 1;
            while (f.token_position < f.current_tokens.size() && loop_depth > 0) {
                f.token_position++;
                if (f.current_tokens[f.token_position] == "DO") loop_depth++;
                else if (f.current_tokens[f.token_position] == "LOOP" ||
                         f.current_tokens[f.token_position] == "+LOOP") loop_depth--;
            }
            f.token_position++;
        });
    }
    
    std::vector<std::string> tokenize(const std::string& code) {
        std::vector<std::string> tokens;
        std::string token;
        
        for (size_t i = 0; i < code.length(); i++) {
            char c = code[i];
            
            if (std::isspace(static_cast<unsigned char>(c))) {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
                continue;
            }
            
            if (c == '\\') {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
                while (i < code.length() && code[i] != '\n') i++;
                continue;
            }
            
            token += c;
        }
        
        if (!token.empty()) {
            tokens.push_back(token);
        }
        
        return tokens;
    }
    
    void interpret(const std::vector<std::string>& tokens) {
        size_t saved_position = token_position;
        token_position = 0;
        current_tokens = tokens;
        
        while (token_position < current_tokens.size()) {
            const std::string& token = current_tokens[token_position];
            
            if (token == "(") {
                int depth = 1;
                token_position++;
                while (token_position < current_tokens.size() && depth > 0) {
                    if (current_tokens[token_position] == "(") depth++;
                    else if (current_tokens[token_position] == ")") depth--;
                    token_position++;
                }
                continue;
            }
            
            if (compiling && token != ";") {
                current_definition.push_back(token);
                token_position++;
                continue;
            }
            
            if (token == ":") {
                if (token_position + 1 >= current_tokens.size()) {
                    throw std::runtime_error("Expected word name after :");
                }
                compiling = true;
                token_position++;
                current_word_name = current_tokens[token_position];
                current_definition.clear();
                token_position++;
                continue;
            }
            
            if (token == ";") {
                if (current_definition.empty()) {
                    throw std::runtime_error("Empty word definition");
                }
                compiling = false;
                dictionary[current_word_name] = Word(current_definition);
                token_position++;
                continue;
            }
            
            auto it = dictionary.find(token);
            if (it != dictionary.end()) {
                Word& word = it->second;
                if (word.is_native()) {
                    try {
                        word.get_native()(*this);
                    } catch (const std::runtime_error& e) {
                        if (std::string(e.what()) == "EXIT") {
                            token_position = saved_position;
                            throw;
                        }
                        throw;
                    }
                } else if (word.is_user()) {
                    interpret(word.get_definition());
                }
                token_position++;
            } 
            else if (is_number(token)) {
                data_stack.push_int(std::stoll(token));
                token_position++;
            }
            else if (is_float(token)) {
                data_stack.push_float(std::stod(token));
                token_position++;
            }
            else {
                throw std::runtime_error("Unknown word: " + token);
            }
        }
        
        token_position = saved_position;
    }
    
    bool is_number(const std::string& s) {
        if (s.empty()) return false;
        try {
            size_t pos;
            (void)std::stoll(s, &pos);
            return pos == s.length();
        } catch (...) {
            return false;
        }
    }
    
    bool is_float(const std::string& s) {
        if (s.empty()) return false;
        try {
            size_t pos;
            (void)std::stod(s, &pos);
            return pos == s.length();
        } catch (...) {
            return false;
        }
    }
};

} // namespace Forth
