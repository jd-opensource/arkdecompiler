#include "lexicalenv.h"
#include <algorithm>
#include <utility>


LexicalEnv::LexicalEnv(size_t capacity) 
    : expressions_(capacity, nullptr), capacity_(capacity), full_size_(0) {
}

bool LexicalEnv::IsFull() const {
    for (size_t i = 0; i < capacity_; ++i) {
        if(expressions_[i] == nullptr){
            //HandleError("#LexicalEnv::IsFull : " + std::to_string(i));
            return false;
        }
    }
    return true;
}

LexicalEnv::LexicalEnv(const LexicalEnv& other) 
    : expressions_(other.capacity_), capacity_(other.capacity_), full_size_(other.full_size_){
    
    for (size_t i = 0; i < capacity_; ++i) {
        //std::cout << "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" << std::endl;
        if(other.expressions_[i] == nullptr){
            //std::cout << "null" << std::endl;
        }else{
            //std::cout << "not null" << std::endl;
            expressions_[i] = new std::string(*(other.expressions_[i]));
        }
        
        //std::cout << "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb" << std::endl;
    }
}

LexicalEnv& LexicalEnv::operator=(const LexicalEnv& other) {
    if (this != &other) {
        expressions_.clear();
        capacity_ = other.capacity_;
        expressions_.resize(capacity_);
        full_size_ = other.full_size_;
        
        for (size_t i = 0; i < capacity_; ++i) {
            expressions_[i] = new std::string(*other.expressions_[i]);
        }
    }
    return *this;
}

LexicalEnv::LexicalEnv(LexicalEnv&& other) noexcept 
    : expressions_(std::move(other.expressions_)), 
      capacity_(other.capacity_) {
    
    other.capacity_ = 0;
}

LexicalEnv& LexicalEnv::operator=(LexicalEnv&& other) noexcept {
    if (this != &other) {
        expressions_ = std::move(other.expressions_);
        capacity_ = other.capacity_;
        other.capacity_ = 0;
    }
    return *this;
}

std::string*& LexicalEnv::operator[](size_t index) {
    CheckIndex(index);
    return expressions_[index];
}

const std::string* LexicalEnv::operator[](size_t index) const {
    CheckIndex(index);
    return expressions_[index];
}

std::string* LexicalEnv::Get(size_t index) const {
    CheckIndex(index);
    return expressions_[index];
}

void LexicalEnv::Set(size_t index, std::string* expr) {
    CheckIndex(index);
    expressions_[index] = expr;
    
}


size_t LexicalEnv::size() const {
    for (size_t i = 0; i < capacity_; ++i) {
        if(expressions_[i] != nullptr){
            full_size_ = i;
        }
    }

    return full_size_;
}


bool LexicalEnv::IsValidIndex(size_t index) const {
    return index < capacity_;
}

void LexicalEnv::CheckIndex(size_t index) const {
    if (index >= capacity_) {
        throw std::out_of_range("LexicalEnv index out of range");
    }
}

LexicalEnvStack::LexicalEnvStack() {
}

LexicalEnvStack::LexicalEnvStack(const LexicalEnvStack& other) 
    : stack_(other.stack_) {
}

bool LexicalEnvStack::IsFull() const {
    for(auto const &lexicalenv : stack_){
        if(!lexicalenv.IsFull()){
            return false;
        }
    }
    return true;
}

LexicalEnvStack& LexicalEnvStack::operator=(const LexicalEnvStack& other) {
    if (this != &other) {
        stack_ = other.stack_;
    }
    return *this;
}

LexicalEnvStack::LexicalEnvStack(LexicalEnvStack&& other) noexcept 
    : stack_(std::move(other.stack_)) {
}

LexicalEnvStack& LexicalEnvStack::operator=(LexicalEnvStack&& other) noexcept {
    if (this != &other) {
        stack_ = std::move(other.stack_);
    }
    return *this;
}

LexicalEnvStack::~LexicalEnvStack() {
}

LexicalEnv* LexicalEnvStack::push(size_t capacity) {
    stack_.emplace_back(capacity);
    return &stack_.back();
}

void LexicalEnvStack::pop() {
    if (stack_.empty()) {
        throw std::runtime_error("Cannot pop from empty stack");
    }
    stack_.pop_back();
}

size_t LexicalEnvStack::size() const {
    return stack_.size();
}

bool LexicalEnvStack::Empty() const {
    return stack_.empty();
}

std::string* LexicalEnvStack::Get(size_t A, size_t B) const {
    CheckIndex(A, B);
    
    size_t actualIndex = stack_.size() - 1 - A;
    return stack_[actualIndex].Get(B);
}

void LexicalEnvStack::Set(size_t A, size_t B, std::string* expr) {
    CheckIndex(A, B);
    
    size_t actualIndex = stack_.size() - 1 - A;
    stack_[actualIndex].Set(B, expr);
}

const LexicalEnv& LexicalEnvStack::GetLexicalEnv(size_t A) const {
    CheckStackIndex(A);
    
    size_t actualIndex = stack_.size() - 1 - A;
    return stack_[actualIndex];
}

LexicalEnv& LexicalEnvStack::GetLexicalEnv(size_t A) {
    CheckStackIndex(A);
    
    size_t actualIndex = stack_.size() - 1 - A;
    return stack_[actualIndex];
}

const LexicalEnv& LexicalEnvStack::Top() const {
    if (stack_.empty()) {
        throw std::runtime_error("Stack is empty");
    }
    return stack_.back();
}

LexicalEnv& LexicalEnvStack::Top() {
    if (stack_.empty()) {
        throw std::runtime_error("Stack is empty");
    }
    return stack_.back();
}

void LexicalEnvStack::Clear() {
    stack_.clear();
}

void LexicalEnvStack::CheckIndex(size_t A, size_t B) const {
    CheckStackIndex(A);
    
    size_t actualIndex = stack_.size() - 1 - A;
    if (!stack_[actualIndex].IsValidIndex(B)) {
        throw std::out_of_range("LexicalEnv index B out of range");
    }
}

void LexicalEnvStack::CheckStackIndex(size_t A) const {
    if (stack_.empty()) {
        throw std::runtime_error("Stack is empty");
    }
    
    if (A >= stack_.size()) {
        throw std::out_of_range("Stack index A out of range");
    }
}