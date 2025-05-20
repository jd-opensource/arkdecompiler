#ifndef BYTECODE_OPTIMIZER_CODEGEN_H
#define BYTECODE_OPTIMIZER_CODEGEN_H

#include "ins_create_api.h"
#include "ast.h"


namespace panda::bytecodeopt {

using compiler::BasicBlock;
using compiler::Inst;
using compiler::Opcode;

void DoLda(compiler::Register reg, std::vector<pandasm::Ins> &result);
void DoSta(compiler::Register reg, std::vector<pandasm::Ins> &result);



class AstGen : public compiler::Optimization, public compiler::GraphVisitor {
public:
    explicit AstGen(compiler::Graph *graph, pandasm::Function *function,
        const BytecodeOptIrInterface *iface, pandasm::Program *prog,  es2panda::parser::Program* parser_program, std::string fun_name)
        : compiler::Optimization(graph), function_(function), ir_interface_(iface), program_(prog),  parser_program_(parser_program)
    {
        ArenaVector<es2panda::ir::Expression*> arguments(parser_program->Allocator()->Adapter());

        for (size_t i = 0; i < function->GetParamsNum(); ++i) {
            panda::es2panda::util::StringView tmp_name_view = panda::es2panda::util::StringView(*new std::string("arg"+std::to_string(i)));
            arguments.push_back(parser_program->Allocator()->New<panda::es2panda::ir::Identifier>(tmp_name_view));
        }

        ArenaVector<panda::es2panda::ir::Statement *> func_statements(parser_program->Allocator()->Adapter());
        auto body = parser_program->Allocator()->New<panda::es2panda::ir::BlockStatement>(nullptr, std::move(func_statements));
        panda::es2panda::ir::ScriptFunctionFlags flags_ {panda::es2panda::ir::ScriptFunctionFlags::NONE};
        auto funcNode = parser_program->Allocator()->New<panda::es2panda::ir::ScriptFunction>(nullptr, std::move(arguments), nullptr, body, nullptr, flags_, true, false);
        

        panda::es2panda::util::StringView name_view = panda::es2panda::util::StringView(*new std::string(fun_name));
        auto funname_id = parser_program->Allocator()->New<panda::es2panda::ir::Identifier>(name_view);
                

        funcNode->SetIdent(funname_id);
        auto funcDecl = parser_program->Allocator()->New<panda::es2panda::ir::FunctionDeclaration>(funcNode);

        auto program_block = parser_program->Ast();
        auto program_statements = program_block->Statements();
        program_block->AddStatementAtPos(program_statements.size(), funcDecl);


        this->id2block[0] = body;
    }
    ~AstGen() override = default;
    bool RunImpl() override;
    const char *GetPassName() const override
    {
        return "AstGen";
    }
    std::vector<pandasm::Ins> GetEncodedInstructions() const
    {
        return res_;
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

    static std::string LabelName(uint32_t id)
    {
        return "label_" + std::to_string(id);
    }

    void EmitLabel(const std::string label)
    {
        pandasm::Ins l;
        l.label = label;
        l.set_label = true;
        result_.emplace_back(l);
    }


    void EmitJump(const BasicBlock *bb);

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
    

    template <typename T, typename... Args>
    static T *AllocNode(AstGen * xx, Args &&... args)
    {
        //[[maybe_unused]] auto *xx = static_cast<AstGen *>(visitor);
        auto ret = xx->parser_program_->Allocator()->New<T>(std::forward<Args>(args)...);
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

    uint32_t parseHexFromKey(const std::string& key) {
        std::istringstream iss(key);
        std::string temp;
        std::string hexString;

        while (iss >> temp) {
            if (temp.find("0x") == 0 || temp.find("0X") == 0) {
                hexString = temp;
                break;
            }
        }

        uint32_t hexNumber = 0;
        if (!hexString.empty()) {
            std::istringstream(hexString) >> std::hex >> hexNumber;
        }

        return hexNumber;
    }

    std::optional<panda::pandasm::LiteralArray> findLiteralArrayByOffset(uint32_t offset) {
        for (const auto& [key, value] : this->program_->literalarray_table) {
            if (parseHexFromKey(key) == offset) {
                return value;
            }
        }
        return std::nullopt;
    }


    static panda::es2panda::ir::Identifier* get_identifier(AstGen * enc, compiler::Register reg){
        panda::es2panda::ir::Identifier* identifier;
        if (enc->identifers.find(reg)  != enc->identifers.end()) {
            identifier =  enc->identifers[reg];
        } else {
            std::string* raw_name =  new std::string("v" + std::to_string(reg));

            panda::es2panda::util::StringView reg_name = panda::es2panda::util::StringView( *raw_name);
            //std::cout << raw_name << " reg_name: " << reg_name << std::endl;

            identifier = AllocNode<panda::es2panda::ir::Identifier>(enc, reg_name);
            enc->identifers[reg] = identifier;
            enc->str2identifers[*raw_name ] = identifier;
        }
        return identifier;
    }


    static panda::es2panda::ir::NumberLiteral* get_literal_bynum(AstGen * enc, uint32_t index){
        panda::es2panda::ir::NumberLiteral* literal;
        if (enc->num2literals.find(index)  != enc->num2literals.end()) {
            literal = enc->num2literals[index];
        } else {
            literal = AllocNode<panda::es2panda::ir::NumberLiteral>(enc, index);
            enc->num2literals[index] = literal;
        }
        return literal;
    }

    static panda::es2panda::ir::Identifier* get_identifier_byname(AstGen * enc, std::string* raw_name){
        panda::es2panda::ir::Identifier* identifier;
        if (enc->str2identifers.find(*raw_name)  != enc->str2identifers.end()) {
            identifier = enc->str2identifers[*raw_name];
        } else {
            panda::es2panda::util::StringView name_view = panda::es2panda::util::StringView(*raw_name);
            //std::cout << "@@@ name_view: " << name_view << std::endl;

            identifier = AllocNode<panda::es2panda::ir::Identifier>(enc, name_view);
            enc->str2identifers[*raw_name] = identifier;
        }
        return identifier;
    }

    std::optional<panda::es2panda::ir::Expression*> get_expression_by_register(AstGen * enc, compiler::Register key){
        auto it = enc->reg2expression.find(key);
        if (it != enc->reg2expression.end()) {
            std::cout << "get_expression_by_register: " << std::to_string(key) << std::endl;
            return it->second;  
        }
        handleError("can't find expression in reg2expression: " + std::to_string(key));
        
        return std::nullopt;
    }

    void set_expression_by_register(AstGen * enc, compiler::Register key, panda::es2panda::ir::Expression* value){
        if(value == nullptr){
            handleError("can't set null expression in reg2expression");
        }
        std::cout << "set_expression_by_register: " << std::to_string(key) << std::endl;
        enc->reg2expression[key] = value;
    }

     BasicBlock* lca(BasicBlock* root, BasicBlock* p, BasicBlock* q, std::set<uint32_t>& visited){
        if(!root || root == p || root == q){
            return root;
        }

        if(visited.find(root->GetId()) != visited.end()){
            return nullptr;
        }
        visited.insert(root->GetId());

        BasicBlock* left = nullptr;
        BasicBlock* right = nullptr;

        if(root->GetSuccsBlocks().size() > 0){
            left = lca(root->GetTrueSuccessor(), p, q, visited);
        }

        if(root->GetSuccsBlocks().size() > 1){
            right = lca(root->GetFalseSuccessor(), p, q, visited);
        }      
        
        if(left && right){
            return root;
        }

        return left? left: right;
    }

    void logid2blockkeys(){
        for (const auto& pair : this->id2block) {
            std::cout << pair.first << std::endl;
        }
    }
 
    es2panda::ir::BlockStatement* get_blockstatement_byid(AstGen * enc, BasicBlock *block){
        auto block_id = block->GetId();
        std::cout << "@@ block id start: " << block_id << std::endl;

        // case1: found blockstatment
        if (enc->id2block.find(block_id) != enc->id2block.end()) {
            return enc->id2block[block_id];
        }

        // case2: found unique predecessor with unique successor
        if(block->GetPredsBlocks().size() == 1 && block_id != 0 && block->GetPredecessor(0)->GetSuccsBlocks().size() == 1){
            BasicBlock* ancestor_block = block->GetPredecessor(0);
            enc->id2block[block_id] =  enc->id2block[ancestor_block->GetId()];;
            return enc->id2block[block_id];
        }
        
        // case3:create new statements
        ArenaVector<panda::es2panda::ir::Statement *> statements(enc->parser_program_->Allocator()->Adapter());
        auto new_block_statement = enc->parser_program_->Allocator()->New<panda::es2panda::ir::BlockStatement>(nullptr, std::move(statements));
        enc->id2block[block_id] = new_block_statement;

        if(enc->specialblockid.find(block_id) == enc->specialblockid.end() ){
            BasicBlock* ancestor_block = nullptr;

            if(block->GetPredsBlocks().size() == 2){
                std::set<uint32_t> visited;

                auto left = block->GetPredecessor(0);

                while(left->IsCatchBegin() || left->IsTry()){
                        left = left->GetPredecessor(0);
                }

                auto right = block->GetPredecessor(1);
                while(right->IsCatchBegin()  || right->IsTry()){//|| right->IsCatch() || right->IsCatchEnd()
                    right = right->GetPredecessor(0);
                }
                std::cout << "left: " << std::to_string(left->GetId()) << std::endl;
                std::cout << "right: " << std::to_string(right->GetId()) << std::endl;
                if(left == right){
                    ancestor_block = left;
                }else{
                    ancestor_block = lca(block->GetGraph()->GetStartBlock(), left, right, visited);
                }
            }else{
                enc->handleError("not considered case");
            }
            std::cout << "@ ancestor_block: " <<  std::to_string(ancestor_block->GetId()) <<  std::endl;
            if(enc->id2block.find(ancestor_block->GetId()) != enc->id2block.end()){
                std::cout << "@ d" << std::endl;
                auto ancestor_block_statements = enc->id2block[ancestor_block->GetId()];
                std::cout << "@ e" << std::endl;
                const auto &ancestor_statements = ancestor_block_statements->Statements();
                std::cout << "@ f" << std::endl;
                ancestor_block_statements->AddStatementAtPos(ancestor_statements.size(), new_block_statement);
                std::cout << "@ h" << std::endl;
            }else{
                enc->handleError("can't find block id in id2block for deal");
            }

            return enc->id2block[block_id];
        }else{
            /**
             * add statement in special statements
             * a) if
             * b) try-catch 
             * c) ...
            */

        }
        
        es2panda::ir::BlockStatement* curstatement = enc->id2block[block_id];
        // std::cout << "@ ee" << std::endl;
        // if (block->GetTryId() !=  panda::compiler::INVALID_ID && !block->IsCatch() ) {
        //     std::cout << "@ 1" << std::endl;
        //     auto it = enc->tyrid2block.find(block->GetTryId());
        //     if (it == enc->tyrid2block.end()) {
        //         enc->handleError("get_block by_try_id error: " + std::to_string(block->GetTryId()));
        //     }
        //     std::cout << "@ 2" << std::endl;
        //     auto father_block = enc->tyrid2block[block->GetTryId()];
        //     std::cout << "@ 3" << std::endl;
        //     const auto &statements = father_block->Statements();
        //     std::cout << "@ 4" << std::endl;
        //     father_block->AddStatementAtPos(statements.size(), curstatement);
        //     std::cout << "@ 5" << std::endl;
        // }
        std::cout << "@@ block id end: " << block_id << std::endl;
        return curstatement;
    }


    void handleError(const std::string& errorMessage) {
        std::cerr << "Error: " << errorMessage << std::endl;
        std::exit(EXIT_FAILURE); 
    }


#include "compiler/optimizer/ir/visitor.inc"

private:
    void AppendCatchBlock(uint32_t type_id, const compiler::BasicBlock *try_begin, const compiler::BasicBlock *try_end,
                          const compiler::BasicBlock *catch_begin, const compiler::BasicBlock *catch_end = nullptr);
    void VisitTryBegin(const compiler::BasicBlock *bb);

public:
    pandasm::Function *function_;
    const BytecodeOptIrInterface *ir_interface_;
    pandasm::Program *program_;

    std::vector<pandasm::Ins> res_;
    std::vector<pandasm::Function::CatchBlock> catch_blocks_;

    uint32_t first_block_id_;



    

    bool success_ {true};
    std::vector<pandasm::Ins> result_;
   
    std::string* acc_global_str = NULL;

    panda::es2panda::ir::Expression* acc = NULL;

    panda::es2panda::ir::Expression* thisptr= NULL;

    es2panda::parser::Program* parser_program_;


    std::map<uint32_t, es2panda::ir::BlockStatement*> id2block;

    std::set<uint32_t> specialblockid;

   
    std::map<compiler::Register, panda::es2panda::ir::Identifier*> identifers;

    std::map<std::string, panda::es2panda::ir::Identifier*> str2identifers;
    std::map<uint32_t, panda::es2panda::ir::NumberLiteral*> num2literals;

    std::map<compiler::Register, panda::es2panda::ir::Expression*> reg2expression;

    std::map<uint32_t, es2panda::ir::BlockStatement*> tyrid2block;
    std::map<uint32_t, panda::es2panda::ir::TryStatement*> tyridtrystatement;
    std::map<uint32_t, panda::es2panda::ir::CatchClause*> tyrid2catchclause;

    panda::es2panda::ir::Identifier* DEFINEFUNC = AllocNode<panda::es2panda::ir::Identifier>(this, "DEFINEFUNC");

    panda::es2panda::ir::Identifier* constant_undefined = AllocNode<panda::es2panda::ir::Identifier>(this, "undefined");
    panda::es2panda::ir::Identifier* constant_nan = AllocNode<panda::es2panda::ir::Identifier>(this, "NaN");
    panda::es2panda::ir::Identifier* constant_infinity = AllocNode<panda::es2panda::ir::Identifier>(this, "infinity");
    panda::es2panda::ir::BooleanLiteral* constant_true = AllocNode<panda::es2panda::ir::BooleanLiteral>(this, true);
    panda::es2panda::ir::BooleanLiteral* constant_false = AllocNode<panda::es2panda::ir::BooleanLiteral>(this, false);
    panda::es2panda::ir::NullLiteral* constant_null = AllocNode<panda::es2panda::ir::NullLiteral>(this);

    panda::es2panda::ir::NumberLiteral* constant_one = AllocNode<panda::es2panda::ir::NumberLiteral>(this, 1);
    panda::es2panda::ir::NumberLiteral* constant_zero = AllocNode<panda::es2panda::ir::NumberLiteral>(this, 0);


};




}  // namespace panda::bytecodeopt

#endif  // BYTECODE_OPTIMIZER_CODEGEN_H
