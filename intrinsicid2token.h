#include "assembler/annotation.h"
#include "assembler/assembly-function.h"
#include "assembler/assembly-ins.h"
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


#include "../ets_frontend/es2panda/ir/expressions/literals/bigIntLiteral.h"
#include "../ets_frontend/es2panda/ir/expressions/literals/numberLiteral.h"
#include "../ets_frontend/es2panda/ir/expressions/literals/stringLiteral.h"
#include "../ets_frontend/es2panda/ir/expressions/literals/booleanLiteral.h"
#include "../ets_frontend/es2panda/ir/expressions/literals/nullLiteral.h"
#include "../ets_frontend/es2panda/ir/expressions/literals/regExpLiteral.h"
#include "../ets_frontend/es2panda/ir/expressions/literals/taggedLiteral.h"



#include "../ets_frontend/es2panda/ir/expressions/memberExpression.h"
#include "../ets_frontend/es2panda/ir/expressions/objectExpression.h"
#include "../ets_frontend/es2panda/ir/expressions/sequenceExpression.h"
#include "../ets_frontend/es2panda/ir/expressions/templateLiteral.h"
#include "../ets_frontend/es2panda/ir/expressions/thisExpression.h"
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

panda::es2panda::lexer::TokenType BinIntrinsicIdToToken(panda::compiler::RuntimeInterface::IntrinsicId id){

    switch (id) {
        case panda::compiler::RuntimeInterface::IntrinsicId::AND2_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_BITWISE_AND;
        case panda::compiler::RuntimeInterface::IntrinsicId::OR2_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_BITWISE_OR;
        case panda::compiler::RuntimeInterface::IntrinsicId::MUL2_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_MULTIPLY;
        case panda::compiler::RuntimeInterface::IntrinsicId::DIV2_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_DIVIDE;
        case panda::compiler::RuntimeInterface::IntrinsicId::SUB2_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_MINUS;
        case panda::compiler::RuntimeInterface::IntrinsicId::NOT_IMM8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_TILDE;
        case panda::compiler::RuntimeInterface::IntrinsicId::SHL2_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_LEFT_SHIFT;
        case panda::compiler::RuntimeInterface::IntrinsicId::ASHR2_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_RIGHT_SHIFT;
        case panda::compiler::RuntimeInterface::IntrinsicId::LESSEQ_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_LESS_THAN_EQUAL;
        case panda::compiler::RuntimeInterface::IntrinsicId::GREATEREQ_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_GREATER_THAN_EQUAL;
        case panda::compiler::RuntimeInterface::IntrinsicId::MOD2_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_MOD;
        case panda::compiler::RuntimeInterface::IntrinsicId::XOR2_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_BITWISE_XOR;

        case panda::compiler::RuntimeInterface::IntrinsicId::EXP_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_EXPONENTIATION;

        case panda::compiler::RuntimeInterface::IntrinsicId::SHR2_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_UNSIGNED_RIGHT_SHIFT;

        case panda::compiler::RuntimeInterface::IntrinsicId::ADD2_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_PLUS;

        /////////////////////////
        
        case panda::compiler::RuntimeInterface::IntrinsicId::LESS_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_LESS_THAN;

        case panda::compiler::RuntimeInterface::IntrinsicId::GREATER_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_GREATER_THAN;

        case panda::compiler::RuntimeInterface::IntrinsicId::EQ_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_EQUAL;

        case panda::compiler::RuntimeInterface::IntrinsicId::NOTEQ_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_NOT_EQUAL;

        case panda::compiler::RuntimeInterface::IntrinsicId::STRICTEQ_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_STRICT_EQUAL;

        case panda::compiler::RuntimeInterface::IntrinsicId::STRICTNOTEQ_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_NOT_STRICT_EQUAL;

        default:
            UNREACHABLE();
    }
}


panda::es2panda::lexer::TokenType UnaryPrefixIntrinsicIdToToken(panda::compiler::RuntimeInterface::IntrinsicId id){

    switch (id) {
        case panda::compiler::RuntimeInterface::IntrinsicId::NEG_IMM8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_MINUS;
        case panda::compiler::RuntimeInterface::IntrinsicId::NOT_IMM8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_TILDE;
        case panda::compiler::RuntimeInterface::IntrinsicId::TYPEOF_IMM8:
            return panda::es2panda::lexer::TokenType::KEYW_TYPEOF;
        case panda::compiler::RuntimeInterface::IntrinsicId::TONUMBER_IMM8:
            return panda::es2panda::lexer::TokenType::KEYW_NUMBER;
        default:
            UNREACHABLE();
    }
}


panda::es2panda::lexer::TokenType IncDecIntrinsicIdToToken(panda::compiler::RuntimeInterface::IntrinsicId id){
    switch (id) {
        case panda::compiler::RuntimeInterface::IntrinsicId::DEC_IMM8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_MINUS;

        case panda::compiler::RuntimeInterface::IntrinsicId::INC_IMM8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_PLUS;
        default:
            UNREACHABLE();

    }
}