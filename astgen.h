#ifndef BYTECODE_OPTIMIZER_CODEGEN_H
#define BYTECODE_OPTIMIZER_CODEGEN_H

#include "assembler/annotation.h"
#include "assembler/assembly-function.h"
#include "assembler/assembly-ins.h"
#include "ins_create_api.h"
#include "ir_interface.h"
#include "compiler/optimizer/pass.h"
#include "compiler/optimizer/ir/basicblock.h"
#include "compiler/optimizer/ir/graph.h"
#include "compiler/optimizer/ir/graph_visitor.h"
#include "utils/logger.h"


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
        const BytecodeOptIrInterface *iface, pandasm::Program *prog,  es2panda::parser::Program* programast, uint32_t first_block_id)
        : compiler::Optimization(graph), function_(function), ir_interface_(iface), program_(prog),  first_block_id_(first_block_id), programast_(programast)
    {
        std::cout << "enter:: " << first_block_id << std::endl;
        this->id2block[first_block_id] = programast->Ast();
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

    void EncodeSpillFillData(const compiler::SpillFillData &sf);

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
        auto ret = xx->programast_->Allocator()->New<T>(std::forward<Args>(args)...);
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
            return it->second;  
        }
        handleError("can't find expression in reg2expression");
        return std::nullopt;
    }

    void set_expression_by_register(AstGen * enc, compiler::Register key, panda::es2panda::ir::Expression* value){
        if(value == nullptr){
            handleError("can't set null expression in reg2expression");
        }
        std::cout << "set_expression_by_register: " << std::to_string(key) << std::endl;
        enc->reg2expression[key] = value;
    }

    es2panda::ir::BlockStatement* get_blockstatement_byid(AstGen * enc, uint32_t block_id){
        std::cout << "@@@@@@@@@@@@@@@@@@@@: " << block_id << std::endl;
        if (enc->id2block.find(block_id) == enc->id2block.end()) {
            ArenaVector<panda::es2panda::ir::Statement *> statements(enc->programast_->Allocator()->Adapter());
            auto block = enc->programast_->Allocator()->New<panda::es2panda::ir::BlockStatement>(nullptr, std::move(statements));
            enc->id2block[block_id] = block;
        }
        return enc->id2block[block_id];
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

    es2panda::parser::Program* programast_;


    std::map<uint32_t, es2panda::ir::BlockStatement*> id2block;

   
    std::map<compiler::Register, panda::es2panda::ir::Identifier*> identifers;

    std::map<std::string, panda::es2panda::ir::Identifier*> str2identifers;
    std::map<uint32_t, panda::es2panda::ir::NumberLiteral*> num2literals;

    std::map<compiler::Register, panda::es2panda::ir::Expression*> reg2expression;

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
