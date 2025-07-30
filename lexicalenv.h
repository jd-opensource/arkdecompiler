#ifndef LEXICAL_ENV_STACK_H
#define LEXICAL_ENV_STACK_H

#include <vector>
#include <stdexcept>


class Expression; 

class LexicalEnv {
private:
    std::vector<Expression*> expressions;

public:
    LexicalEnv() = default;
    explicit LexicalEnv(size_t size);
    LexicalEnv(std::initializer_list<Expression*> init);
    
    size_t size() const;
    void push_back(Expression* expr);
    Expression* get(size_t index) const;
    void set(size_t index, Expression* expr);
    Expression* operator[](size_t index) const;
    Expression*& operator[](size_t index);
    void resize(size_t new_size);
    void clear();
};


class LexicalEnvStack {
private:
    std::vector<LexicalEnv> stack;

public:
    LexicalEnvStack() = default;
    
    void push(const LexicalEnv& env);
    void push(LexicalEnv&& env);
    void pop();
    LexicalEnv& top();
    const LexicalEnv& top() const;
    bool empty() const;
    size_t size() const;
    
    Expression* get(size_t depth_from_top, size_t expr_index) const;
    void set(size_t depth_from_top, size_t expr_index, Expression* expr);
    LexicalEnv& get_env(size_t depth_from_top);
    const LexicalEnv& get_env(size_t depth_from_top) const;
    Expression* at(size_t depth_from_top, size_t expr_index) const;
    Expression*& at(size_t depth_from_top, size_t expr_index);
    LexicalEnv& push_new_env(size_t capacity);
    void clear();
    size_t total_expressions() const;
};

#endif // LEXICAL_ENV_STACK_H