#ifndef LEXICAL_ENV_STACK_H
#define LEXICAL_ENV_STACK_H

#include <vector>
#include <memory>
#include <stdexcept>
#include "ast.h"


class LexicalEnv {
private:
    std::vector<panda::es2panda::ir::Expression*> expressions_;
    size_t capacity_;

public:
    explicit LexicalEnv(size_t capacity = 256);
    
    LexicalEnv(const LexicalEnv& other);
    
    LexicalEnv& operator=(const LexicalEnv& other);
    
    LexicalEnv(LexicalEnv&& other) noexcept;
    
    LexicalEnv& operator=(LexicalEnv&& other) noexcept;
    
    panda::es2panda::ir::Expression*& operator[](size_t index);
    const panda::es2panda::ir::Expression* operator[](size_t index) const;
    
    panda::es2panda::ir::Expression* get(size_t index) const;
    void set(size_t index, panda::es2panda::ir::Expression* expr);
    
    size_t capacity() const;
    size_t size() const;
    
    bool isValidIndex(size_t index) const;

private:
    void checkIndex(size_t index) const;
};


class LexicalEnvStack {
private:
    std::vector<LexicalEnv> stack_;

public:
    LexicalEnvStack();
    LexicalEnvStack(const LexicalEnvStack& other);
    
    LexicalEnvStack& operator=(const LexicalEnvStack& other);
    
    LexicalEnvStack(LexicalEnvStack&& other) noexcept;
    
    LexicalEnvStack& operator=(LexicalEnvStack&& other) noexcept;
    
    ~LexicalEnvStack();
    LexicalEnv* push(size_t capacity);
    
    void pop();
    
    size_t size() const;
    
    bool empty() const;
    
    panda::es2panda::ir::Expression* get(size_t A, size_t B) const;
    void set(size_t A, size_t B, panda::es2panda::ir::Expression* expr);
    
    const LexicalEnv& getLexicalEnv(size_t A) const;
    LexicalEnv& getLexicalEnv(size_t A);
    
    const LexicalEnv& top() const;
    
    LexicalEnv& top();
    void clear();

private:
    void checkIndex(size_t A, size_t B) const;
    void checkStackIndex(size_t A) const;
};

#endif // LEXICAL_ENV_STACK_H