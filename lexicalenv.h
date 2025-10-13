#ifndef DECOMPILER_LEXICAL_ENV_STACK
#define DECOMPILER_LEXICAL_ENV_STACK

#include "base.h"

using panda::compiler::BasicBlock;
using panda::compiler::Inst;
using panda::compiler::Opcode;

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
    
    size_t Size() const;
    
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
    LexicalEnv* Push(size_t capacity);
    
    void Pop();
    
    size_t Size() const;
    
    bool Empty() const;
    
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

void DealWithGlobalLexicalWaitlist(uint32_t tier, uint32_t index, std::string closure_name, std::vector<LexicalEnvStack*> *globallexical_waitlist);

void MergeMethod2LexicalMap(uint32_t source_methodoffset, uint32_t target_methodoffset, std::map<uint32_t, std::map<uint32_t,  std::set<uint32_t>>>* method2lexicalmap);

void PrintInnerMethod2LexicalMap(std::map<uint32_t, std::map<uint32_t,  std::set<uint32_t>>>* method2lexicalmap, uint32_t methodoffset);

uint32_t SearchStartposForCreatePrivateproperty(Inst *inst, std::map<uint32_t, std::map<uint32_t,  std::set<uint32_t>>>* method2lexicalmap, uint32_t methodoffset);


void CopyLexicalenvStack(uint32_t methodoffset_, Inst* inst, std::map<uint32_t, LexicalEnvStack*>* method2lexicalenvstack, 
        std::map<panda::compiler::BasicBlock*, LexicalEnvStack*> bb2lexicalenvstack, std::vector<LexicalEnvStack*> *globallexical_waitlist);

#endif // LEXICAL_ENV_STACK_H