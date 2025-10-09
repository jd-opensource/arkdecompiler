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
    
    bool IsFull() const;
    std::string* Get(size_t index) const;
    void Set(size_t index, std::string* expr);
    
    size_t size() const;
    
    bool IsValidIndex(size_t index) const;

private:
    void CheckIndex(size_t index) const;
};


class LexicalEnvStack {
private:
    std::vector<LexicalEnv> stack_;

public:
    LexicalEnvStack();
    LexicalEnvStack(const LexicalEnvStack& other);
    
    bool IsFull() const;
    
    LexicalEnvStack& operator=(const LexicalEnvStack& other);
    
    LexicalEnvStack(LexicalEnvStack&& other) noexcept;
    
    LexicalEnvStack& operator=(LexicalEnvStack&& other) noexcept;
    
    ~LexicalEnvStack();
    LexicalEnv* push(size_t capacity);
    
    void pop();
    
    size_t size() const;
    
    bool empty() const;
    
    std::string* Get(size_t A, size_t B) const;
    void Set(size_t A, size_t B, std::string* expr);
    
    const LexicalEnv& GetLexicalEnv(size_t A) const;
    LexicalEnv& GetLexicalEnv(size_t A);
    
    const LexicalEnv& Top() const;
    
    LexicalEnv& Top();
    void Clear();

private:
    void CheckIndex(size_t A, size_t B) const;
    void CheckStackIndex(size_t A) const;
};

#endif // LEXICAL_ENV_STACK_H