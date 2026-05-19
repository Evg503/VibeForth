// forth_core.hpp - исправленная версия Word
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
#include <memory>  // для std::unique_ptr

namespace Forth {

    // Базовые типы данных
    using Cell = int64_t;
    using Float = double;
    using Addr = void*;

    // Универсальное значение
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

    // Стек данных
    class DataStack {
    private:
        std::vector<Value> stack;

    public:
        void push(const Value& v) {
            stack.push_back(v);
        }

        void push(Cell v) {
            stack.push_back(Value(v));
        }

        void push(Float v) {
            stack.push_back(Value(v));
        }

        void push(Addr v) {
            stack.push_back(Value(v));
        }

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

        size_t size() const {
            return stack.size();
        }

        bool empty() const {
            return stack.empty();
        }

        void clear() {
            stack.clear();
        }
    };

    // Исправленная версия Word
    class Word {
    public:
        using NativeFn = std::function<void(class ForthInterpreter&)>;

        enum Type { NATIVE, USER, NONE };

    private:
        Type type_;
        NativeFn native_func_;
        std::vector<std::string> definition_;

    public:
        // Конструктор по умолчанию (теперь есть!)
        Word() : type_(NONE) {}

        // Конструктор для нативного слова
        Word(NativeFn fn) : type_(NATIVE), native_func_(fn) {}

        // Конструктор для пользовательского слова
        Word(const std::vector<std::string>& def) : type_(USER), definition_(def) {}

        // Проверка типа
        bool is_native() const { return type_ == NATIVE; }
        bool is_user() const { return type_ == USER; }
        bool is_none() const { return type_ == NONE; }

        // Получение функций
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

    // Интерпретатор Forth
    class ForthInterpreter {
    private:
        DataStack data_stack;
        std::stack<Addr> return_stack;
        std::unordered_map<std::string, Word> dictionary;
        std::unordered_map<std::string, Addr> extern_functions;

        bool compiling = false;
        std::vector<std::string> current_definition;
        std::string current_word_name;
        void* user_data = nullptr;

    public:
        ForthInterpreter() {
            init_primitive_words();
            init_math_words();
            init_stack_words();
            init_memory_words();
            init_control_words();
            init_io_words();
        }

        void register_extern(const std::string& name, Addr func_ptr) {
            extern_functions[name] = func_ptr;
        }

        void evaluate(const std::string& code) {
            auto tokens = tokenize(code);
            interpret(tokens);
        }

        DataStack& get_data_stack() { return data_stack; }
        void set_user_data(void* data) { user_data = data; }
        void* get_user_data() { return user_data; }

    private:
        void init_primitive_words() {
            // DUP
            dictionary["DUP"] = Word([](ForthInterpreter& f) {
                Value v = f.data_stack.top();
                f.data_stack.push(v);
                });

            // DROP
            dictionary["DROP"] = Word([](ForthInterpreter& f) {
                f.data_stack.pop();
                });

            // SWAP
            dictionary["SWAP"] = Word([](ForthInterpreter& f) {
                Value a = f.data_stack.pop();
                Value b = f.data_stack.pop();
                f.data_stack.push(a);
                f.data_stack.push(b);
                });

            // OVER
            dictionary["OVER"] = Word([](ForthInterpreter& f) {
                if (f.data_stack.size() < 2) throw std::runtime_error("Stack underflow");
                Value a = f.data_stack.pop();
                Value b = f.data_stack.top();
                f.data_stack.push(a);
                f.data_stack.push(b);
                });

            // ROT
            dictionary["ROT"] = Word([](ForthInterpreter& f) {
                if (f.data_stack.size() < 3) throw std::runtime_error("Stack underflow");
                Value a = f.data_stack.pop();
                Value b = f.data_stack.pop();
                Value c = f.data_stack.pop();
                f.data_stack.push(b);
                f.data_stack.push(a);
                f.data_stack.push(c);
                });

            // Арифметика
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

            // Плавающая точка
            dictionary["F+"] = Word([](ForthInterpreter& f) {
                Float b = f.data_stack.pop_float();
                Float a = f.data_stack.pop_float();
                f.data_stack.push(a + b);
                });

            dictionary["F-"] = Word([](ForthInterpreter& f) {
                Float b = f.data_stack.pop_float();
                Float a = f.data_stack.pop_float();
                f.data_stack.push(a - b);
                });

            dictionary["F*"] = Word([](ForthInterpreter& f) {
                Float b = f.data_stack.pop_float();
                Float a = f.data_stack.pop_float();
                f.data_stack.push(a * b);
                });

            dictionary["F/"] = Word([](ForthInterpreter& f) {
                Float b = f.data_stack.pop_float();
                Float a = f.data_stack.pop_float();
                if (b == 0.0) throw std::runtime_error("Division by zero");
                f.data_stack.push(a / b);
                });

            // Преобразования
            dictionary["INT>FLOAT"] = Word([](ForthInterpreter& f) {
                Cell i = f.data_stack.pop_int();
                f.data_stack.push((Float)i);
                });

            dictionary["FLOAT>INT"] = Word([](ForthInterpreter& f) {
                Float fl = f.data_stack.pop_float();
                f.data_stack.push((Cell)fl);
                });

            // Работа с памятью
            dictionary["@"] = Word([](ForthInterpreter& f) {
                Addr addr = f.data_stack.pop_ptr();
                f.data_stack.push(*static_cast<Cell*>(addr));
                });

            dictionary["!"] = Word([](ForthInterpreter& f) {
                Addr addr = f.data_stack.pop_ptr();
                Cell value = f.data_stack.pop_int();
                *static_cast<Cell*>(addr) = value;
                });
        }

        void init_math_words() {
            dictionary["NEGATE"] = Word([](ForthInterpreter& f) {
                Cell a = f.data_stack.pop_int();
                f.data_stack.push(-a);
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

        void init_stack_words() {
            dictionary["DEPTH"] = Word([](ForthInterpreter& f) {
                f.data_stack.push((Cell)f.data_stack.size());
                });

            dictionary[".S"] = Word([](ForthInterpreter& f) {
                std::cout << "Stack depth: " << f.data_stack.size() << std::endl;
                });

            dictionary[">R"] = Word([](ForthInterpreter& f) {
                Addr a = f.data_stack.pop_ptr();
                f.return_stack.push(a);
                });

            dictionary["R>"] = Word([](ForthInterpreter& f) {
                if (f.return_stack.empty()) throw std::runtime_error("Return stack empty");
                Addr a = f.return_stack.top();
                f.return_stack.pop();
                f.data_stack.push(a);
                });
        }

        void init_memory_words() {
            dictionary["ALLOC"] = Word([](ForthInterpreter& f) {
                Cell size = f.data_stack.pop_int();
                if (size <= 0) throw std::runtime_error("Invalid size");
                Addr ptr = malloc(size);
                if (!ptr) throw std::runtime_error("Allocation failed");
                f.data_stack.push(ptr);
                });

            dictionary["FREE"] = Word([](ForthInterpreter& f) {
                Addr ptr = f.data_stack.pop_ptr();
                if (ptr) free(ptr);
                });
        }

        void init_control_words() {
            dictionary["EXIT"] = Word([](ForthInterpreter& f) {
                throw std::runtime_error("EXIT");
                });
        }

        void init_io_words() {
            dictionary["."] = Word([](ForthInterpreter& f) {
                Cell value = f.data_stack.pop_int();
                std::cout << value;
                });

            dictionary["F."] = Word([](ForthInterpreter& f) {
                Float value = f.data_stack.pop_float();
                std::cout << value;
                });

            dictionary["CR"] = Word([](ForthInterpreter& f) {
                std::cout << std::endl;
                });

            dictionary["SPACE"] = Word([](ForthInterpreter& f) {
                std::cout << ' ';
                });
        }

        std::vector<std::string> tokenize(const std::string& code) {
            std::vector<std::string> tokens;
            std::string token;

            for (char c : code) {
                if (std::isspace(static_cast<unsigned char>(c))) {
                    if (!token.empty()) {
                        tokens.push_back(token);
                        token.clear();
                    }
                }
                else {
                    token += c;
                }
            }

            if (!token.empty())
                tokens.push_back(token);

            return tokens;
        }

        void interpret(const std::vector<std::string>& tokens) {
            for (size_t i = 0; i < tokens.size(); i++) {
                const std::string& token = tokens[i];

                if (compiling && token != ";") {
                    current_definition.push_back(token);
                    continue;
                }

                if (token == ":") {
                    if (i + 1 >= tokens.size()) {
                        throw std::runtime_error("Expected word name");
                    }
                    compiling = true;
                    current_word_name = tokens[++i];
                    current_definition.clear();
                    continue;
                }

                if (token == ";") {
                    if (current_definition.empty()) {
                        throw std::runtime_error("Empty definition");
                    }
                    compiling = false;
                    dictionary[current_word_name] = Word(current_definition);
                    continue;
                }

                auto it = dictionary.find(token);
                if (it != dictionary.end()) {
                    Word& word = it->second;
                    if (word.is_native()) {
                        try {
                            word.get_native()(*this);
                        }
                        catch (const std::runtime_error& e) {
                            if (std::string(e.what()) == "EXIT") {
                                return;
                            }
                            throw;
                        }
                    }
                    else if (word.is_user()) {
                        interpret(word.get_definition());
                    }
                }
                else if (is_number(token)) {
                    data_stack.push(std::stoll(token));
                }
                else if (is_float(token)) {
                    data_stack.push(std::stod(token));
                }
                else {
                    throw std::runtime_error("Unknown word: " + token);
                }
            }
        }

        bool is_number(const std::string& s) {
            if (s.empty()) return false;
            try {
                size_t pos;
                std::stoll(s, &pos);
                return pos == s.length();
            }
            catch (...) {
                return false;
            }
        }

        bool is_float(const std::string& s) {
            if (s.empty()) return false;
            try {
                size_t pos;
                std::stod(s, &pos);
                return pos == s.length();
            }
            catch (...) {
                return false;
            }
        }
    };

} // namespace Forth