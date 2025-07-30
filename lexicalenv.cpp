#include "lexicalenv.h"


LexicalEnv::LexicalEnv(size_t size) : expressions(size, nullptr) {}
LexicalEnv::LexicalEnv(std::initializer_list<Expression*> init) : expressions(init) {}

size_t LexicalEnv::size() const {
    return expressions.size();
}

void LexicalEnv::push_back(Expression* expr) {
    expressions.push_back(expr);
}

Expression* LexicalEnv::get(size_t index) const {
    if (index >= expressions.size()) {
        throw std::out_of_range("LexicalEnv index out of range");
    }
    return expressions[index];
}

void LexicalEnv::set(size_t index, Expression* expr) {
    if (index >= expressions.size()) {
        throw std::out_of_range("LexicalEnv index out of range");
    }
    expressions[index] = expr;
}

Expression* LexicalEnv::operator[](size_t index) const {
    return get(index);
}

Expression*& LexicalEnv::operator[](size_t index) {
    if (index >= expressions.size()) {
        throw std::out_of_range("LexicalEnv index out of range");
    }
    return expressions[index];
}

void LexicalEnv::resize(size_t new_size) {
    expressions.resize(new_size, nullptr);
}

void LexicalEnv::clear() {
    expressions.clear();
}

void LexicalEnvStack::push(const LexicalEnv& env) {
    stack.push_back(env);
}

void LexicalEnvStack::push(LexicalEnv&& env) {
    stack.push_back(std::move(env));
}

void LexicalEnvStack::pop() {
    if (stack.empty()) {
        throw std::runtime_error("Cannot pop from empty stack");
    }
    stack.pop_back();
}

LexicalEnv& LexicalEnvStack::top() {
    if (stack.empty()) {
        throw std::runtime_error("Stack is empty");
    }
    return stack.back();
}

const LexicalEnv& LexicalEnvStack::top() const {
    if (stack.empty()) {
        throw std::runtime_error("Stack is empty");
    }
    return stack.back();
}

bool LexicalEnvStack::empty() const {
    return stack.empty();
}

size_t LexicalEnvStack::size() const {
    return stack.size();
}

Expression* LexicalEnvStack::get(size_t depth_from_top, size_t expr_index) const {
    if (depth_from_top >= stack.size()) {
        throw std::out_of_range("Stack depth out of range");
    }
    
    size_t actual_index = stack.size() - 1 - depth_from_top;
    return stack[actual_index].get(expr_index);
}

void LexicalEnvStack::set(size_t depth_from_top, size_t expr_index, Expression* expr) {
    if (depth_from_top >= stack.size()) {
        throw std::out_of_range("Stack depth out of range");
    }
    
    size_t actual_index = stack.size() - 1 - depth_from_top;
    stack[actual_index].set(expr_index, expr);
}

LexicalEnv& LexicalEnvStack::get_env(size_t depth_from_top) {
    if (depth_from_top >= stack.size()) {
        throw std::out_of_range("Stack depth out of range");
    }
    
    size_t actual_index = stack.size() - 1 - depth_from_top;
    return stack[actual_index];
}

const LexicalEnv& LexicalEnvStack::get_env(size_t depth_from_top) const {
    if (depth_from_top >= stack.size()) {
        throw std::out_of_range("Stack depth out of range");
    }
    
    size_t actual_index = stack.size() - 1 - depth_from_top;
    return stack[actual_index];
}

Expression* LexicalEnvStack::at(size_t depth_from_top, size_t expr_index) const {
    return get(depth_from_top, expr_index);
}

Expression*& LexicalEnvStack::at(size_t depth_from_top, size_t expr_index) {
    if (depth_from_top >= stack.size()) {
        throw std::out_of_range("Stack depth out of range");
    }
    
    size_t actual_index = stack.size() - 1 - depth_from_top;
    return stack[actual_index][expr_index];
}

LexicalEnv& LexicalEnvStack::push_new_env(size_t capacity) {
    LexicalEnv new_env(capacity);
    stack.push_back(std::move(new_env));
    return stack.back();
}

void LexicalEnvStack::clear() {
    stack.clear();
}

size_t LexicalEnvStack::total_expressions() const {
    size_t total = 0;
    for (const auto& env : stack) {
        total += env.size();
    }
    return total;
}