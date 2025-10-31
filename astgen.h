#ifndef DECOMPILER_ASTGEN
#define DECOMPILER_ASTGEN

#include "base.h"
#include "lexicalenv.h"
#include "algos.h"
#include "loopconstruction.h"

namespace panda::bytecodeopt {

using compiler::BasicBlock;
using compiler::Inst;
using compiler::Opcode;

void DoLda(compiler::Register reg, std::vector<pandasm::Ins> &result);
void DoSta(compiler::Register reg, std::vector<pandasm::Ins> &result);

class AstGen : public compiler::Optimization, public compiler::GraphVisitor {
public:
    explicit AstGen(compiler::Graph *graph, pandasm::Function *function, 
        const BytecodeOptIrInterface *iface, pandasm::Program *prog,  es2panda::parser::Program* parser_program, 
        uint32_t methodoffset, std::map<uint32_t, LexicalEnvStack*>* method2lexicalenvstack,
        std::map<uint32_t, LexicalEnvStack*>* method2sendablelexicalenvstack, 
        std::map<uint32_t, std::string*> *patchvarspace,
        std::map<size_t, std::vector<std::string>> index2namespaces, std::vector<std::string> localnamespaces,
        std::map<uint32_t, std::set<uint32_t>> *class2memberfuns, 
        std::map<uint32_t, panda::es2panda::ir::ScriptFunction *> *method2scriptfunast, 
        std::map<uint32_t, panda::es2panda::ir::ClassDeclaration *>* ctor2classdeclast, std::set<uint32_t> *memberfuncs, 
        std::map<uint32_t, panda::es2panda::ir::Expression*> *class2father, 
        std::map<uint32_t, std::map<uint32_t,  std::set<size_t>>>* method2lexicalmap,
        std::vector<LexicalEnvStack*> *globallexical_waitlist,
        std::vector<LexicalEnvStack*> *globalsendablelexical_waitlist,
        std::map<std::string, std::string> *raw2newname,
        std::map<std::string, uint32_t> *methodname2offset,
        std::string fun_name)
        : compiler::Optimization(graph), function_(function), ir_interface_(iface), program_(prog), methodoffset_(methodoffset),
        method2lexicalenvstack_(method2lexicalenvstack), method2sendablelexicalenvstack_(method2sendablelexicalenvstack), 
        patchvarspace_(patchvarspace), parser_program_(parser_program), 
        index2namespaces_(index2namespaces), localnamespaces_(localnamespaces), class2memberfuns_(class2memberfuns),
        method2scriptfunast_(method2scriptfunast), ctor2classdeclast_(ctor2classdeclast), memberfuncs_(memberfuncs), class2father_(class2father),
        method2lexicalmap_(method2lexicalmap), globallexical_waitlist_(globallexical_waitlist), globalsendablelexical_waitlist_(globalsendablelexical_waitlist), 
        raw2newname_(raw2newname), methodname2offset_(methodname2offset), fun_name_(fun_name)
    {

        this->closure_count = 0;
        this->privatevar_count = 0;

        ArenaVector<es2panda::ir::Expression*> arguments(parser_program->Allocator()->Adapter());

        if(this->method2lexicalenvstack_->find(methodoffset) != this->method2lexicalenvstack_->end()){
            //std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX found lexicalenvstack " << std::endl;
            //auto x = (*this->method2lexicalenvstack_)[methodoffset];
            //std::cout << "lexicalenvstack size: " << x->Size() << std::endl;
        }else{
            //std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX not found lexicalenvstack " << std::endl;
            (*this->method2lexicalenvstack_)[methodoffset] = new LexicalEnvStack();
        }

        if(this->method2sendablelexicalenvstack_->find(methodoffset) != this->method2sendablelexicalenvstack_->end()){
            std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX found lexicalenvstack " << std::endl;
            auto x = (*this->method2sendablelexicalenvstack_)[methodoffset];
            std::cout << "sendablelexicalenvstack size: " << x->Size() << std::endl;
        }else{
            //std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX not found lexicalenvstack " << std::endl;
            (*this->method2sendablelexicalenvstack_)[methodoffset] = new LexicalEnvStack();
        }


        this->bb2lexicalenvstack_[graph->GetStartBlock()] = (*this->method2lexicalenvstack_)[methodoffset];
        this->bb2sendablelexicalenvstack_[graph->GetStartBlock()] = (*this->method2sendablelexicalenvstack_)[methodoffset];
        
        for (size_t i = 0; i < function->GetParamsNum(); ++i) {
            if(i <= 2){
                continue;
            }
            std::string* argname = nullptr;
            if(this->memberfuncs_->find(methodoffset) != this->memberfuncs_->end() && i == 2 ){
                argname = new std::string("this");
            }else{
                argname = new std::string("arg"+std::to_string(i-3));
            }
            panda::es2panda::util::StringView tmp_name_view = panda::es2panda::util::StringView(*argname);
            arguments.push_back(parser_program->Allocator()->New<panda::es2panda::ir::Identifier>(tmp_name_view));
        }

        ArenaVector<panda::es2panda::ir::Statement *> func_statements(parser_program->Allocator()->Adapter());
        auto body = parser_program->Allocator()->New<panda::es2panda::ir::BlockStatement>(nullptr, std::move(func_statements));
        panda::es2panda::ir::ScriptFunctionFlags flags_ {panda::es2panda::ir::ScriptFunctionFlags::NONE};
        auto funcNode = parser_program->Allocator()->New<panda::es2panda::ir::ScriptFunction>(nullptr, std::move(arguments), nullptr, body, nullptr, flags_, true, false);
        
        
        (*this->method2scriptfunast_)[methodoffset] = funcNode;

        auto newfunname = this->RemovePrefixOfFunc(fun_name);
        (*this->raw2newname_)[fun_name] = newfunname;
        panda::es2panda::util::StringView name_view = panda::es2panda::util::StringView(*new std::string(newfunname));
        auto funname_id = AllocNode<panda::es2panda::ir::Identifier>(this, name_view);
                

        funcNode->SetIdent(funname_id);

        this->id2block[0] = body;

        //this->lcaFinder = std::make_unique<LCAFinder>(graph);

    }
    ~AstGen() override = default;
    bool RunImpl() override;
    const char *GetPassName() const override
    {
        return "AstGen";
    }

    void Reserve(size_t res_size = 0)
    {
        if (res_size > 0) {
            result_.reserve(res_size);
        }
    }

    bool GetStatus() const
    {
        return success_;
    }

    const std::vector<pandasm::Ins> &GetResult() const
    {
        return result_;
    }

    std::vector<pandasm::Ins> &&GetResult()
    {
        return std::move(result_);
    }

    const ArenaVector<BasicBlock *> &GetBlocksToVisit() const override
    {
        return GetGraph()->GetBlocksRPO();
    }
    static void VisitSpillFill(GraphVisitor *visitor, Inst *inst);
    static void VisitConstant(GraphVisitor *visitor, Inst *inst);
    static void VisitCatchPhi(GraphVisitor *visitor, Inst *inst);

    static void VisitIf(GraphVisitor *v, Inst *inst_base);
    static void VisitIfImm(GraphVisitor *v, Inst *inst_base);
    static void IfImmZero(GraphVisitor *v, Inst *inst_base);
    static void VisitIntrinsic(GraphVisitor *visitor, Inst *inst_base);
    static void VisitLoadString(GraphVisitor *v, Inst *inst_base);
    static void VisitReturn(GraphVisitor *v, Inst *inst_base);

    static void VisitCastValueToAnyType(GraphVisitor *v, Inst *inst_base);

    static void VisitEcma(GraphVisitor *v, Inst *inst_base);
    static void IfEcma(GraphVisitor *v, compiler::IfInst *inst);

    static void VisitPhi(GraphVisitor* v, Inst* inst_base);
    static void VisitTry(GraphVisitor* v, Inst* inst_base);
    static void VisitSaveState(GraphVisitor* v, Inst* inst_base);
    static void VisitParameter(GraphVisitor* v, Inst* inst_base);
    
    BasicBlock* FindNearestVisitedPred(const std::vector<BasicBlock*>& visited, BasicBlock* block);

    template <typename T, typename... Args>
    static T *AllocNode(AstGen * astgen, Args &&... args)
    {
        auto ret = astgen->parser_program_->Allocator()->New<T>(std::forward<Args>(args)...);
        if (ret == nullptr) {
            std::cout << "Unsuccessful allocation during parsing" << std::endl;;
        }
        return ret;
    }

    void VisitDefault(Inst *inst) override
    {
        LOG(ERROR, BYTECODE_OPTIMIZER) << "Opcode " << compiler::GetOpcodeString(inst->GetOpcode())
                                       << " not yet implemented in codegen";
        success_ = false;
    }

    std::string RemovePrefixOfFunc(const std::string& input) {
        if(this->raw2newname_->find(input) != this->raw2newname_->end()){
            return (*this->raw2newname_)[input];
        }

        auto coarsename = RemoveArgumentsOfFunc(input);

        size_t hashPos = coarsename.find_last_of('#');
        std::string result;
        if (hashPos != std::string::npos) {
            result = coarsename.substr(hashPos + 1);
        } else {
            result = coarsename;
        }
        
        if(result == "" || result.rfind("^", 0) == 0){
            result =  "func_" + std::to_string(count++);
        }

        if (result.rfind("#~@0>#", 0) == 0){
            result = input.substr(6);
            if(result == "" || result.rfind("^", 0) == 0){
                result = "func_" + std::to_string(count++);
            }
        }

        (*this->raw2newname_)[input] = result;

        return result;
        
    }

    std::optional<panda::pandasm::LiteralArray> FindLiteralArrayByOffset(uint32_t offset) {
        for (const auto& [key, value] : this->program_->literalarray_table) {
            if (ParseHexFromKey(key) == offset) {
                return value;
            }
        }
        return std::nullopt;
    }


    panda::es2panda::ir::NumberLiteral* GetLiteralByNum(uint32_t index){
        panda::es2panda::ir::NumberLiteral* literal;
        if (this->num2literals.find(index)  != this->num2literals.end()) {
            literal = this->num2literals[index];
        } else {
            literal = AllocNode<panda::es2panda::ir::NumberLiteral>(this, index);
            this->num2literals[index] = literal;
        }
        return literal;
    }

    panda::es2panda::ir::Identifier* GetIdentifierByReg(compiler::Register reg){
        panda::es2panda::ir::Identifier* identifier;
        if (this->identifers.find(reg)  != this->identifers.end()) {
            identifier =  this->identifers[reg];
        } else {
            std::string* raw_name =  new std::string("v" + std::to_string(reg));

            panda::es2panda::util::StringView reg_name = panda::es2panda::util::StringView( *raw_name);
            identifier = AllocNode<panda::es2panda::ir::Identifier>(this, reg_name);
            this->identifers[reg] = identifier;
            this->str2identifers[*raw_name ] = identifier;
        }
        return identifier;
    }

    panda::es2panda::ir::Identifier* GetIdentifierByName(std::string raw_name){
        panda::es2panda::ir::Identifier* identifier;
        if (this->str2identifers.find(raw_name)  != this->str2identifers.end()) {
            identifier = this->str2identifers[raw_name];
        } else {
            panda::es2panda::util::StringView name_view = panda::es2panda::util::StringView(*new std::string(raw_name));
            identifier = AllocNode<panda::es2panda::ir::Identifier>(this, name_view);
            this->str2identifers[raw_name] = identifier;
        }
        return identifier;
    }


    panda::es2panda::ir::Identifier* GetIdentifierByName(std::string* raw_name){
        return this->GetIdentifierByName(*raw_name);
    }

    void HandleNewCreatedExpression(Inst* inst, es2panda::ir::Expression* expression){
        if(inst->HasUsers()){
            this->SetExpressionByRegister(inst, inst->GetDstReg(), expression);
        }else{
            if(expression->IsCallExpression()){
                auto callstatement = AllocNode<es2panda::ir::ExpressionStatement>(this, expression);
                this->AddInstAst2BlockStatemntByInst(inst, callstatement);
            }

        }
    }

    std::optional<panda::es2panda::ir::Expression*> GetExpressionByAcc(Inst* inst){
        return this->GetExpressionByRegIndex(inst, inst->GetInputsCount() - 2);
    }

    std::optional<panda::es2panda::ir::Expression*> GetExpressionByRegIndex(Inst* inst, uint32_t index){
        auto id = inst->GetInput(index).GetInst()->GetId();
        
        auto it = this->id2expression.find(id);
        if (it != this->id2expression.end()) {
            std::cout << "#GetExpressionByRegister: " << std::to_string(id) << std::endl;
            return it->second;  
        }
        

        // @@@###@@@TODO temp test suport for phi
        //return this->GetIdentifierByName(new std::string(std::to_string(index))); 
        
        //HandleError("can't find expression in reg2expression: " + std::to_string(id));
        return std::nullopt;
    }

    void SetExpressionById(Inst* inst, uint32_t id, panda::es2panda::ir::Expression* value){
        if(value == nullptr){
            HandleError("#SetExpressionByRegister: can't set null expression in reg2expression");
        }
        this->id2expression[id] = value;
    }


    std::optional<panda::es2panda::ir::Expression*> GetExpressionByRegister(Inst* inst, compiler::Register key){
        if(key == compiler::ACC_REG_ID){
            if(this->bb2acc2expression[inst->GetBasicBlock()] != nullptr){
                return this->bb2acc2expression[inst->GetBasicBlock()];
            }else{
                HandleError("#GetExpressionByRegister: acc point to null");
            }
        }

        auto it = this->reg2expression.find(key);
        if (it != this->reg2expression.end()) {
            std::cout << "#GetExpressionByRegister: " << std::to_string(key) << std::endl;
            return it->second;  
        }

        HandleError("can't find expression in reg2expression: " + std::to_string(key));
        
        return std::nullopt;
    }


    void SetExpressionByRegister(Inst* inst, compiler::Register key, panda::es2panda::ir::Expression* value){
        /**
            std::map<compiler::Register, panda::es2panda::ir::Expression*> reg2expression;
            std::map<compiler::BasicBlock*, panda::es2panda::ir::Expression*> bb2acc2expression;
         * 
        */

        if(value == nullptr){
            HandleError("#SetExpressionByRegister: can't set null expression in reg2expression");
        }
        this->SetExpressionById(inst, inst->GetId(), value);

        std::cout << "#SetExpressionByRegister: " << std::to_string(key) << std::endl;
        
        if(inst->IsAccWrite()){
            this->bb2acc2expression[inst->GetBasicBlock()] = value;
        }

        this->reg2expression[key] = value;
    }

    void Logid2BlockKeys(){
        std::cout << "id2block keys: ";
        for (const auto& pair : this->id2block) {
            std::cout << pair.first << ", ";
        }
        std::cout << std::endl;
    }

    void LogSpecialBlockId(){
        std::cout << "specialblockid: ";
        for (auto it = this->specialblockid.begin(); it != this->specialblockid.end(); ++it) {
            std::cout << *it;
            if (std::next(it) != this->specialblockid.end()) {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
    }
 
    void AddInstAst2BlockStatemntByInst(Inst *inst, es2panda::ir::Statement *statement){
        BasicBlock* block = inst->GetBasicBlock();
        this->AddInstAst2BlockStatemntByBlock(block, statement);

        if(block->IsLoopValid() && block->IsLoopHeader() && inst->GetOpcode()!= Opcode::If   && inst->GetOpcode()!= Opcode::IfImm ){
            auto headerblockstatements = this->whileheader2redundant[block];
            const auto &statements = headerblockstatements->Statements();
            headerblockstatements->AddStatementAtPos(statements.size(), statement);
        }
    }

    void AddInstAst2BlockStatemntByBlock(BasicBlock* block, es2panda::ir::Statement *statement){
        es2panda::ir::BlockStatement* block_statements = this->GetBlockStatementById(block);
        const auto &statements = block_statements->Statements();
        block_statements->AddStatementAtPos(statements.size(), statement);
    }

    es2panda::ir::BlockStatement* GetBlockStatementById(BasicBlock *block){
        auto block_id = block->GetId();
        std::cout << "[*] GetBlockStatementById bbid: " << block_id << ", ";

        // case1: found blockstatment
        if (this->id2block.find(block_id) != this->id2block.end()) {
            std::cout << "@@ case 1" << std::endl;
            return this->id2block[block_id];
        }
        
        // case2: found loop
        if(block->IsLoopValid() && block->IsLoopHeader() ){
            std::cout << "@@ case 2" << std::endl;
            JudgeLoopType(block, this->loop2type, this->loop2exit, this->backedge2dowhileloop);

            //////////////////////////////////////////////////////////////////////////////////////
            ArenaVector<panda::es2panda::ir::Statement *> statements(this->parser_program_->Allocator()->Adapter());
            auto new_block_statement = AllocNode<es2panda::ir::BlockStatement>(this, nullptr, std::move(statements));
            
            this->id2block[block_id] = new_block_statement;
            return this->id2block[block_id];
        }

        ///////////////////////////////////////////////////////////////////////////////////////////

        // case3: found unique predecessor with unique successor
        if(block->GetPredsBlocks().size() == 1 && block_id != 0 && block->GetPredecessor(0)->GetSuccsBlocks().size() == 1){
            std::cout << "@@ case 3" << std::endl;
            BasicBlock* ancestor_block = block->GetPredecessor(0);
            this->id2block[block_id] =  this->id2block[ancestor_block->GetId()];;
            return this->id2block[block_id];
        }
        
        // case4:create new statements
        std::cout << "@@ case 4" << std::endl;
        ArenaVector<panda::es2panda::ir::Statement *> statements(this->parser_program_->Allocator()->Adapter());
        auto new_block_statement = AllocNode<es2panda::ir::BlockStatement>(this, nullptr, std::move(statements));

        this->id2block[block_id] = new_block_statement;

        LogSpecialBlockId();

        // nested if-else
        if(this->specialblockid.find(block_id) == this->specialblockid.end() ){
            BasicBlock* ancestor_block = nullptr;

            ancestor_block = block->GetDominator();

            if(ancestor_block == nullptr){
                HandleError("GetBlockStatementById# find ancestor is nullptr");
            }
            std::cout << "@ ancestor_block: " <<  std::to_string(ancestor_block->GetId()) <<  std::endl;

            auto ancestor_block_statements = this->GetBlockStatementById(ancestor_block);
            this->id2block[block_id] =  ancestor_block_statements;

            this->AddInstAst2BlockStatemntByBlock(ancestor_block, new_block_statement);
            

            return this->id2block[block_id];
        }else{
            /**
             * add statement in special statements
             * a) if
             * b) try-catch 
             * c) ...
            */

        }
        
        es2panda::ir::BlockStatement* curstatement = this->id2block[block_id];
        return curstatement;
    }

#include "compiler/optimizer/ir/visitor.inc"

private:
    void VisitTryBegin(const compiler::BasicBlock *bb);

public:
    pandasm::Function *function_;
    const BytecodeOptIrInterface *ir_interface_;
    pandasm::Program *program_;

    uint32_t methodoffset_;
    uint32_t closure_count;

    uint32_t privatevar_count;

    std::vector<pandasm::Ins> res_;
    std::vector<pandasm::Function::CatchBlock> catch_blocks_;

    std::map<uint32_t, LexicalEnvStack*>* method2lexicalenvstack_;
    std::map<uint32_t, LexicalEnvStack*>* method2sendablelexicalenvstack_;

    std::map<uint32_t, std::string*> *patchvarspace_;

    es2panda::parser::Program* parser_program_;

    std::map<size_t, std::vector<std::string>> index2namespaces_;
    std::vector<std::string> localnamespaces_;
    std::map<uint32_t, std::set<uint32_t>> *class2memberfuns_;
    std::map<uint32_t, panda::es2panda::ir::ScriptFunction *> *method2scriptfunast_;
    std::map<uint32_t, panda::es2panda::ir::ClassDeclaration *>* ctor2classdeclast_;

    std::set<uint32_t> *memberfuncs_;

    std::map<uint32_t, panda::es2panda::ir::Expression*> *class2father_;

    std::map<uint32_t, std::map<uint32_t,  std::set<size_t>>>* method2lexicalmap_;

    std::vector<LexicalEnvStack*> *globallexical_waitlist_;
    std::vector<LexicalEnvStack*> *globalsendablelexical_waitlist_;

    std::map<std::string, std::string> *raw2newname_;

    std::map<std::string, uint32_t> *methodname2offset_;

    std::string fun_name_;

    
    ///////////////////////////////////////////////////////////////////////////////////////
    uint32_t current_constructor_offset;

    std::unique_ptr<LCAFinder> lcaFinder;

    bool success_ {true};
    std::vector<pandasm::Ins> result_;
   

    std::string* acc_global_str = NULL;

    panda::es2panda::ir::Expression* acc = NULL;

    panda::es2panda::ir::Expression* thisptr= NULL;

    std::map<compiler::Loop *, uint32_t> loop2type; // 0-while, 1-dowhile
    std::map<compiler::Loop *, BasicBlock*> loop2exit; 
    std::map<compiler::BasicBlock*, compiler::Loop *> backedge2dowhileloop;

    std::map<compiler::BasicBlock*, es2panda::ir::BlockStatement*> whileheader2redundant;

    std::map<uint32_t, es2panda::ir::BlockStatement*> id2block;

    std::set<uint32_t> specialblockid;

    std::map<compiler::Register, panda::es2panda::ir::Identifier*> identifers;
    

    std::map<std::string, panda::es2panda::ir::Identifier*> str2identifers;
    std::map<uint32_t, panda::es2panda::ir::NumberLiteral*> num2literals;

    std::map<compiler::Register, panda::es2panda::ir::Expression*> reg2expression;
    std::map<compiler::BasicBlock*, panda::es2panda::ir::Expression*> bb2acc2expression;

    LexicalEnv* acc_lexicalenv = NULL;

    std::map<compiler::BasicBlock*, LexicalEnvStack*> bb2lexicalenvstack_;

    std::map<compiler::BasicBlock*, LexicalEnvStack*> bb2sendablelexicalenvstack_;

    std::map<uint32_t, panda::es2panda::ir::Expression*> id2expression;
    
    std::map<uint32_t, es2panda::ir::BlockStatement*> tyrid2block;
    std::map<uint32_t, panda::es2panda::ir::TryStatement*> tyridtrystatement;
    std::map<uint32_t, panda::es2panda::ir::CatchClause*> tyrid2catchclause;

    std::set<panda::es2panda::ir::Expression*> not_add_assgin_for_stlexvar; // class and instance_initializer

    panda::es2panda::ir::Identifier* constant_undefined = AllocNode<panda::es2panda::ir::Identifier>(this, "undefined");
    panda::es2panda::ir::Identifier* constant_hole = AllocNode<panda::es2panda::ir::Identifier>(this, "hole");

    panda::es2panda::ir::Identifier* constant_nan = AllocNode<panda::es2panda::ir::Identifier>(this, "NaN");
    panda::es2panda::ir::Identifier* constant_infinity = AllocNode<panda::es2panda::ir::Identifier>(this, "infinity");
    panda::es2panda::ir::Identifier* constant_catcherror = AllocNode<panda::es2panda::ir::Identifier>(this, "error");
    panda::es2panda::ir::Identifier* constant_symbol = AllocNode<panda::es2panda::ir::Identifier>(this, "Symbol");
    panda::es2panda::ir::Identifier* constant_this = AllocNode<panda::es2panda::ir::Identifier>(this, "this");
    panda::es2panda::ir::Identifier* constant_global = AllocNode<panda::es2panda::ir::Identifier>(this, "global");
    panda::es2panda::ir::Identifier* constant_newtarget = AllocNode<panda::es2panda::ir::Identifier>(this, "new.target");
    panda::es2panda::ir::Identifier* constant_restargs = AllocNode<panda::es2panda::ir::Identifier>(this, "args");

    panda::es2panda::ir::BooleanLiteral* constant_true = AllocNode<panda::es2panda::ir::BooleanLiteral>(this, true);
    panda::es2panda::ir::BooleanLiteral* constant_false = AllocNode<panda::es2panda::ir::BooleanLiteral>(this, false);
    panda::es2panda::ir::NullLiteral* constant_null = AllocNode<panda::es2panda::ir::NullLiteral>(this);

    panda::es2panda::ir::NumberLiteral* constant_one = AllocNode<panda::es2panda::ir::NumberLiteral>(this, 1);
    panda::es2panda::ir::NumberLiteral* constant_zero = AllocNode<panda::es2panda::ir::NumberLiteral>(this, 0);


};

}

#endif
