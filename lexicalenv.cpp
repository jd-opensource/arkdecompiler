#include "lexicalenv.h"
#include <algorithm>
#include <utility>


LexicalEnv::LexicalEnv(size_t capacity) 
    : expressions_(capacity, nullptr), capacity_(capacity), full_size_(0) {
}

bool LexicalEnv::IsFull() {
    for (size_t i = 0; i < capacity_; ++i) {
        if(expressions_[i] == nullptr){
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
    checkIndex(index);
    return expressions_[index];
}

const std::string* LexicalEnv::operator[](size_t index) const {
    checkIndex(index);
    return expressions_[index];
}

std::string* LexicalEnv::get(size_t index) const {
    checkIndex(index);
    return expressions_[index];
}

void LexicalEnv::set(size_t index, std::string* expr) {
    checkIndex(index);
    expressions_[index] = expr;
    
}

size_t LexicalEnv::capacity() const {
    return capacity_;
}

size_t LexicalEnv::size() const {
    for (size_t i = 0; i < capacity_; ++i) {
        if(expressions_[i] != nullptr){
            full_size_ = i;
        }
    }

    return full_size_;
}


bool LexicalEnv::isValidIndex(size_t index) const {
    return index < capacity_;
}

void LexicalEnv::checkIndex(size_t index) const {
    if (index >= capacity_) {
        throw std::out_of_range("LexicalEnv index out of range");
    }
}

LexicalEnvStack::LexicalEnvStack() {
}

LexicalEnvStack::LexicalEnvStack(const LexicalEnvStack& other) 
    : stack_(other.stack_) {
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

bool LexicalEnvStack::empty() const {
    return stack_.empty();
}

std::string* LexicalEnvStack::get(size_t A, size_t B) const {
    checkIndex(A, B);
    
    size_t actualIndex = stack_.size() - 1 - A;
    return stack_[actualIndex].get(B);
}

void LexicalEnvStack::set(size_t A, size_t B, std::string* expr) {
    checkIndex(A, B);
    
    size_t actualIndex = stack_.size() - 1 - A;
    stack_[actualIndex].set(B, expr);
}

const LexicalEnv& LexicalEnvStack::getLexicalEnv(size_t A) const {
    checkStackIndex(A);
    
    size_t actualIndex = stack_.size() - 1 - A;
    return stack_[actualIndex];
}

LexicalEnv& LexicalEnvStack::getLexicalEnv(size_t A) {
    checkStackIndex(A);
    
    size_t actualIndex = stack_.size() - 1 - A;
    return stack_[actualIndex];
}

const LexicalEnv& LexicalEnvStack::top() const {
    if (stack_.empty()) {
        throw std::runtime_error("Stack is empty");
    }
    return stack_.back();
}

LexicalEnv& LexicalEnvStack::top() {
    if (stack_.empty()) {
        throw std::runtime_error("Stack is empty");
    }
    return stack_.back();
}

void LexicalEnvStack::clear() {
    stack_.clear();
}

void LexicalEnvStack::checkIndex(size_t A, size_t B) const {
    checkStackIndex(A);
    
    size_t actualIndex = stack_.size() - 1 - A;
    if (!stack_[actualIndex].isValidIndex(B)) {
        throw std::out_of_range("LexicalEnv index B out of range");
    }
}

void LexicalEnvStack::checkStackIndex(size_t A) const {
    if (stack_.empty()) {
        throw std::runtime_error("Stack is empty");
    }
    
    if (A >= stack_.size()) {
        throw std::out_of_range("Stack index A out of range");
    }
}