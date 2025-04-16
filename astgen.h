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

#include "../ets_frontend/es2panda/es2panda.h"
#include "../ets_frontend/es2panda/parser/program/program.h"

/////////////////////////////////////////////////////
#include "../ets_frontend/es2panda/ir/base/catchClause.h"
#include "../ets_frontend/es2panda/ir/base/classStaticBlock.h"
#include "../ets_frontend/es2panda/ir/base/decorator.h"
#include "../ets_frontend/es2panda/ir/base/scriptFunction.h"

#include "../ets_frontend/es2panda/ir/expressions/assignmentExpression.h"
#include "../ets_frontend/es2panda/ir/expressions/binaryExpression.h"
#include "../ets_frontend/es2panda/ir/expressions/callExpression.h"
#include "../ets_frontend/es2panda/ir/expressions/classExpression.h"
#include "../ets_frontend/es2panda/ir/expressions/functionExpression.h"
#include "../ets_frontend/es2panda/ir/expressions/memberExpression.h"
#include "../ets_frontend/es2panda/ir/expressions/objectExpression.h"
#include "../ets_frontend/es2panda/ir/expressions/sequenceExpression.h"
#include "../ets_frontend/es2panda/ir/expressions/templateLiteral.h"
#include "../ets_frontend/es2panda/ir/expressions/thisExpression.h"
#include "../ets_frontend/es2panda/ir/expressions/unaryExpression.h"
#include "../ets_frontend/es2panda/ir/expressions/arrayExpression.h"

#include "../ets_frontend/es2panda/ir/expressions/literals/bigIntLiteral.h"
#include "../ets_frontend/es2panda/ir/expressions/literals/numberLiteral.h"
#include "../ets_frontend/es2panda/ir/expressions/literals/stringLiteral.h"
#include "../ets_frontend/es2panda/ir/expressions/literals/booleanLiteral.h"
#include "../ets_frontend/es2panda/ir/expressions/literals/nullLiteral.h"
#include "../ets_frontend/es2panda/ir/expressions/literals/regExpLiteral.h"
#include "../ets_frontend/es2panda/ir/expressions/literals/taggedLiteral.h"


#include "../ets_frontend/es2panda/ir/base/property.h"




#include "../ets_frontend/es2panda/ir/module/exportDefaultDeclaration.h"
#include "../ets_frontend/es2panda/ir/module/exportNamedDeclaration.h"
#include "../ets_frontend/es2panda/ir/statements/blockStatement.h"
#include "../ets_frontend/es2panda/ir/statements/classDeclaration.h"
#include "../ets_frontend/es2panda/ir/statements/doWhileStatement.h"
#include "../ets_frontend/es2panda/ir/statements/expressionStatement.h"
#include "../ets_frontend/es2panda/ir/statements/forInStatement.h"
#include "../ets_frontend/es2panda/ir/statements/forOfStatement.h"
#include "../ets_frontend/es2panda/ir/statements/forUpdateStatement.h"
#include "../ets_frontend/es2panda/ir/statements/functionDeclaration.h"
#include "../ets_frontend/es2panda/ir/statements/returnStatement.h"
#include "../ets_frontend/es2panda/ir/statements/switchStatement.h"
#include "../ets_frontend/es2panda/ir/statements/variableDeclaration.h"
#include "../ets_frontend/es2panda/ir/statements/variableDeclarator.h"
#include "../ets_frontend/es2panda/ir/statements/whileStatement.h"
#include "../ets_frontend/es2panda/ir/ts/tsConstructorType.h"
#include "../ets_frontend/es2panda/ir/ts/tsEnumDeclaration.h"
#include "../ets_frontend/es2panda/ir/ts/tsEnumMember.h"
#include "../ets_frontend/es2panda/ir/ts/tsFunctionType.h"
#include "../ets_frontend/es2panda/ir/ts/tsImportEqualsDeclaration.h"
#include "../ets_frontend/es2panda/ir/ts/tsInterfaceDeclaration.h"
#include "../ets_frontend/es2panda/ir/ts/tsMethodSignature.h"
#include "../ets_frontend/es2panda/ir/ts/tsModuleBlock.h"
#include "../ets_frontend/es2panda/ir/ts/tsModuleDeclaration.h"
#include "../ets_frontend/es2panda/ir/ts/tsParameterProperty.h"
#include "../ets_frontend/es2panda/ir/ts/tsPrivateIdentifier.h"
#include "../ets_frontend/es2panda/ir/ts/tsQualifiedName.h"
#include "../ets_frontend/es2panda/ir/ts/tsSignatureDeclaration.h"
#include "../ets_frontend/es2panda/ir/ts/tsTypeParameterDeclaration.h"

#include "../ets_frontend/es2panda/parser/parserImpl.h"

/////////////////////////////////////////////////////


namespace panda::bytecodeopt {

using compiler::BasicBlock;
using compiler::Inst;
using compiler::Opcode;

void DoLda(compiler::Register reg, std::vector<pandasm::Ins> &result);
void DoSta(compiler::Register reg, std::vector<pandasm::Ins> &result);



class AstGen : public compiler::Optimization, public compiler::GraphVisitor {
public:
    explicit AstGen(compiler::Graph *graph, pandasm::Function *function,
        const BytecodeOptIrInterface *iface, pandasm::Program *prog,  es2panda::parser::Program* programast)
        : compiler::Optimization(graph), function_(function), ir_interface_(iface), program_(prog), programast_(programast)
    {
        
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
    void EncodeSta(compiler::Register reg, compiler::DataType::Type type);

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

    bool success_ {true};
    std::vector<pandasm::Ins> result_;
   
    std::string* acc_global_str = NULL;

    panda::es2panda::ir::Expression* acc = NULL;

    panda::es2panda::ir::Identifier* thisptr= NULL;

    es2panda::parser::Program* programast_;
   
   
    std::map<compiler::Register, panda::es2panda::ir::Identifier*> identifers;
    std::map<std::string, panda::es2panda::ir::Identifier*> str2identifers;

    std::map<compiler::Register, panda::es2panda::ir::Expression*> reg2expression;

    

    panda::es2panda::ir::Identifier* constant_undefined = AllocNode<panda::es2panda::ir::Identifier>(this, "undefined");
    panda::es2panda::ir::BooleanLiteral* constant_true = AllocNode<panda::es2panda::ir::BooleanLiteral>(this, true);
    panda::es2panda::ir::BooleanLiteral* constant_false = AllocNode<panda::es2panda::ir::BooleanLiteral>(this, false);
    panda::es2panda::ir::NullLiteral* constant_null = AllocNode<panda::es2panda::ir::NullLiteral>(this);

    panda::es2panda::ir::NumberLiteral* constant_one = AllocNode<panda::es2panda::ir::NumberLiteral>(this, 1);


};




}  // namespace panda::bytecodeopt

#endif  // BYTECODE_OPTIMIZER_CODEGEN_H
