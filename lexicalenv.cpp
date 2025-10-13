#include "lexicalenv.h"
#include <algorithm>
#include <utility>


LexicalEnv::LexicalEnv(size_t capacity) 
    : expressions_(capacity, nullptr), capacity_(capacity), full_size_(0) {
}

void LexicalEnv::AddIndexes(size_t index){
    indexes_.insert(index);
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
        if(other.expressions_[i] == nullptr){
        }else{
            expressions_[i] = new std::string(*(other.expressions_[i]));
        }
    }

    indexes_.insert(other.indexes_.begin(), other.indexes_.end());
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
    AddIndexes(index);
}

size_t LexicalEnv::Size() const {
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

LexicalEnvStack::~LexicalEnvStack() {
}

LexicalEnv* LexicalEnvStack::Push(size_t capacity) {
    stack_.emplace_back(capacity);
    return &stack_.back();
}

void LexicalEnvStack::Pop() {
    if (stack_.empty()) {
        throw std::runtime_error("Cannot pop from empty stack");
    }
    stack_.pop_back();
}

size_t LexicalEnvStack::Size() const {
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


void DealWithGlobalLexicalWaitlist(uint32_t tier, uint32_t index, std::string closure_name, std::vector<LexicalEnvStack*> *globallexical_waitlist){
    for (auto it = globallexical_waitlist->begin(); it != globallexical_waitlist->end(); ) {
        auto* waitelement = *it;
        waitelement->Set(tier, index, new std::string(closure_name));

        if(waitelement->IsFull()){
            it = globallexical_waitlist->erase(it);
        }else{
            ++it;
        }
    }
}

void MergeMethod2LexicalMap(uint32_t source_methodoffset, uint32_t target_methodoffset, std::map<uint32_t, std::map<uint32_t,  std::set<uint32_t>>>* method2lexicalmap) {
    // merge instance_initializer lexical to current 
    auto& tmpmethod2lexicalmap = *(method2lexicalmap);

    auto source_lexicalmap = tmpmethod2lexicalmap.find(source_methodoffset);
    if (source_lexicalmap == tmpmethod2lexicalmap.end()) {
        HandleError("#MergeMethod2LexicalMap: source key not found");
        return;
    }

    auto& target_lexicalmap = tmpmethod2lexicalmap[target_methodoffset];

    for (const auto& [tier, source_indexes] : source_lexicalmap->second) {
        auto& target_indexes = target_lexicalmap[tier];
        target_indexes.insert(source_indexes.begin(), source_indexes.end());
    }
}

void PrintInnerMethod2LexicalMap(std::map<uint32_t, std::map<uint32_t,  std::set<uint32_t>>>* method2lexicalmap, uint32_t methodoffset){
    auto outerIt = method2lexicalmap->find(methodoffset);
    if (outerIt == method2lexicalmap->end()) {
        std::cerr << "Method offset not found in the map." << std::endl;
        return;
    }

    const std::map<uint32_t, std::set<uint32_t>>& innerMap = outerIt->second;

    for (const auto& pair : innerMap) {
        uint32_t key = pair.first;
        const std::set<uint32_t>& vec = pair.second;

        std::cout << "Key: " << key << " Values: ";
        for (const auto& value : vec) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
}


uint32_t SearchStartposForCreatePrivateproperty(Inst *inst, std::map<uint32_t, std::map<uint32_t,  std::set<uint32_t>>>* method2lexicalmap, uint32_t methodoffset){
    if(method2lexicalmap->find(methodoffset) != method2lexicalmap->end() ){
        auto outerIt = method2lexicalmap->find(methodoffset);
        if (outerIt == method2lexicalmap->end()) {
            HandleError("#PrintInnerMethod2LexicalMap: Method offset not found in the map1.");
        }

        const std::map<uint32_t, std::set<uint32_t>>& innerMap = outerIt->second;

        auto innerIt = innerMap.find(0);
        if (innerIt == innerMap.end()) {
            HandleError("#PrintInnerMethod2LexicalMap: Method offset not found in the map2.");
        }

        const std::set<uint32_t>& vec = innerIt->second;

        std::vector<uint32_t> sorted(vec.begin(), vec.end());
        std::sort(sorted.begin(), sorted.end());


        for (size_t i = 0; i < sorted.size(); ++i) {
            if (i != sorted[i]) {
                return i;
            }
        }           
    }else{
        HandleError("#SearchStartposForCreatePrivateproperty: not found !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    }

    HandleError("#SearchStartposForCreatePrivateproperty: not found !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

    return -1;
}


void CopyLexicalenvStack(uint32_t methodoffset_, Inst* inst, std::map<uint32_t, LexicalEnvStack*>* method2lexicalenvstack, 
        std::map<panda::compiler::BasicBlock*, LexicalEnvStack*> bb2lexicalenvstack, std::vector<LexicalEnvStack*> *globallexical_waitlist){
            
    if(bb2lexicalenvstack[inst->GetBasicBlock()]->Empty()){
        return;
        //HandleError("#CopyLexicalenvStack: source bb2lexicalenvstack is empty");
    }
    auto wait_method = new LexicalEnvStack(*(bb2lexicalenvstack[inst->GetBasicBlock()]));
    (*method2lexicalenvstack)[methodoffset_] = wait_method;
    globallexical_waitlist->push_back(wait_method);
}