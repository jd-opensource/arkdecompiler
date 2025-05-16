#include "ast.h"


es2panda::ir::BlockStatement* complexstmt2body(panda::es2panda::compiler::ir *astnode){
    switch(astnode->Type()){
        case AstNodeType::DO_WHILE_STATEMENT:
            return astnode->AsDoWhileStatement()->Body();
            break;
        case AstNodeType::FOR_IN_STATEMENT:
            return astnode->AsForInStatement()->Body();
            break;
        case AstNodeType::FOR_OF_STATEMENT:
            return astnode->AsForOfStatement()->Body();
            break;

        case AstNodeType::IF_STATEMENT:


        case :;

        default :;


    }
}