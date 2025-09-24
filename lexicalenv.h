#ifndef DECOMPILER_LEXICAL_ENV_STACK
#define DECOMPILER_LEXICAL_ENV_STACK

#include "base.h"

class LexicalEnv {
public:
    std::vector<std::string*> expressions_;
    size_t capacity_;
    mutable size_t full_size_;

public:
    explicit LexicalEnv(size_t capacity = 256);
    
    LexicalEnv(const LexicalEnv& other);
    
    LexicalEnv& operator=(const LexicalEnv& other);
    
    LexicalEnv(LexicalEnv&& other) noexcept;
    
    LexicalEnv& operator=(LexicalEnv&& other) noexcept;
    
    std::string *& operator[](size_t index);
    const std::string* operator[](size_t index) const;
    
    bool IsFull();
    std::string* get(size_t index) const;
    void set(size_t index, std::string* expr);
    
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
    
    std::string* get(size_t A, size_t B) const;
    void set(size_t A, size_t B, std::string* expr);
    
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