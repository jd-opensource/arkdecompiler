#ifndef DECOMPILER_INTRINSIC2TOKEN
#define DECOMPILER_INTRINSIC2TOKEN

#include "intrinsicid2name.h"

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


        case panda::compiler::RuntimeInterface::IntrinsicId::INSTANCEOF_IMM8_V8: 
            return panda::es2panda::lexer::TokenType::KEYW_INSTANCEOF;
        case panda::compiler::RuntimeInterface::IntrinsicId::ISIN_IMM8_V8:
            return panda::es2panda::lexer::TokenType::KEYW_IN;
            
        default:
            std::cout << "S6" << std::endl;
            UNREACHABLE();
    }
}

panda::es2panda::lexer::TokenType BinInverseToken2Token(panda::es2panda::lexer::TokenType id){
    switch (id) {
        case panda::es2panda::lexer::TokenType::PUNCTUATOR_LESS_THAN_EQUAL:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_GREATER_THAN;

        case panda::es2panda::lexer::TokenType::PUNCTUATOR_GREATER_THAN_EQUAL:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_LESS_THAN;


        case panda::es2panda::lexer::TokenType::PUNCTUATOR_LESS_THAN:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_GREATER_THAN_EQUAL;

        case panda::es2panda::lexer::TokenType::PUNCTUATOR_GREATER_THAN:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_LESS_THAN_EQUAL;

        case panda::es2panda::lexer::TokenType::PUNCTUATOR_EQUAL:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_NOT_EQUAL;

        case panda::es2panda::lexer::TokenType::PUNCTUATOR_NOT_EQUAL:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_EQUAL;

        case panda::es2panda::lexer::TokenType::PUNCTUATOR_STRICT_EQUAL:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_NOT_STRICT_EQUAL;

        case panda::es2panda::lexer::TokenType::PUNCTUATOR_NOT_STRICT_EQUAL:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_STRICT_EQUAL;
  
        default:
            return id;
    }
}


panda::es2panda::lexer::TokenType BinInverseIntrinsicIdToToken(panda::compiler::RuntimeInterface::IntrinsicId id){

    switch (id) {
        case panda::compiler::RuntimeInterface::IntrinsicId::LESSEQ_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_GREATER_THAN;

        case panda::compiler::RuntimeInterface::IntrinsicId::GREATEREQ_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_LESS_THAN;


        case panda::compiler::RuntimeInterface::IntrinsicId::LESS_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_GREATER_THAN_EQUAL;

        case panda::compiler::RuntimeInterface::IntrinsicId::GREATER_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_LESS_THAN_EQUAL;

        case panda::compiler::RuntimeInterface::IntrinsicId::EQ_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_NOT_EQUAL;

        case panda::compiler::RuntimeInterface::IntrinsicId::NOTEQ_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_EQUAL;

        case panda::compiler::RuntimeInterface::IntrinsicId::STRICTEQ_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_NOT_STRICT_EQUAL;

        case panda::compiler::RuntimeInterface::IntrinsicId::STRICTNOTEQ_IMM8_V8:
            return panda::es2panda::lexer::TokenType::PUNCTUATOR_STRICT_EQUAL;
  
        default:
            std::cout << "S7" << std::endl;
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
        case panda::compiler::RuntimeInterface::IntrinsicId::DELOBJPROP_V8:
            return panda::es2panda::lexer::TokenType::KEYW_DELETE;
        default:
            std::cout << "S8: " << GetIntrinsicOpcodeName(id) << std::endl;
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
            std::cout << "S9" << std::endl;
            UNREACHABLE();

    }
}
#endif