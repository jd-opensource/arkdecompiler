#include "arktsgen.h"




namespace panda::es2panda::ir {

ArkTSGen::ArkTSGen(const BlockStatement *program, util::StringView sourceCode) : index_(sourceCode), indent_(0)
{
    SerializeObject(reinterpret_cast<const ir::AstNode *>(program));
}

ArkTSGen::ArkTSGen(const ir::AstNode *node) : indent_(0), dumpNodeOnly_(true)
{
    SerializeNode(node);
}

void ArkTSGen::EmitBlockStatement(const ir::AstNode *node){
    auto tmp = const_cast<ir::AstNode*>(node);
    auto blockstatement = static_cast<panda::es2panda::ir::BlockStatement*>(tmp);

    const auto &statements = blockstatement->Statements();
    
    for (const auto *stmt : statements) {
        this->SerializeNode(stmt);
    }
}

void ArkTSGen::writeTrailingSemicolon(){
    ss_ << std::endl;
}

void ArkTSGen::writeSpace(){
    ss_ << " ";
}

void ArkTSGen::EmitExpression(const ir::AstNode *node){
    switch(node->Type()){ 
        case AstNodeType::BINARY_EXPRESSION:{
            std::cout << "enter BINARY_EXPRESSION >>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl; 
            auto binexpression = static_cast<const panda::es2panda::ir::BinaryExpression*>(node);
            this->EmitExpression(binexpression->Left());
            writeSpace();
            ss_ << TokenToString(binexpression->OperatorType());
            writeSpace();
            this->EmitExpression(binexpression->Right());
            break;
        }

        case AstNodeType::UNARY_EXPRESSION:{
            std::cout << "enter UNARY_EXPRESSION >>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl; 
            auto unaryexpression = static_cast<const panda::es2panda::ir::UnaryExpression*>(node);
            writeSpace();
            ss_ << TokenToString(unaryexpression->OperatorType());
            writeSpace();
            this->EmitExpression(unaryexpression->Argument());
            break;
        }

        case AstNodeType::ASSIGNMENT_EXPRESSION:{
            auto assignexpression = static_cast<const panda::es2panda::ir::AssignmentExpression*>(node);

            std::cout << "enter ASSIGNMENT_EXPRESSION >>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl; 
            
            this->EmitExpression(assignexpression->Left());
            writeSpace();
            ss_ << TokenToString(assignexpression->OperatorType());
            writeSpace();
            this->EmitExpression(assignexpression->Right());
            break;
        }
        case AstNodeType::IDENTIFIER:{
            auto identifier = static_cast<const panda::es2panda::ir::Identifier*>(node);
            ss_ << identifier->Name();
            break;
        }

        case AstNodeType::NUMBER_LITERAL:{
            auto number_literal = static_cast<const panda::es2panda::ir::NumberLiteral*>(node);
            this->SerializeNumber(number_literal->Number());
            break;
        } 

        case AstNodeType::BIGINT_LITERAL:{
            auto bigint_literal = static_cast<const panda::es2panda::ir::BigIntLiteral*>(node);
            this->SerializeString(bigint_literal->Str());
            break;
        } 

        case AstNodeType::NULL_LITERAL:{
            this->SerializeConstant(Property::Constant::PROP_NULL);
            break;
        }

        case AstNodeType::STRING_LITERAL:{
            auto string_literal = static_cast<const panda::es2panda::ir::StringLiteral*>(node);
            this->SerializeString(string_literal->Str());
            break;
        }
        
        case AstNodeType::BOOLEAN_LITERAL:{
            auto bool_literal = static_cast<const panda::es2panda::ir::BooleanLiteral*>(node);
            this->SerializeBoolean(bool_literal->Value());
            break;
        }

        default:
            std::cout << "enter EmitExpression Default  >>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;

    }
}

void ArkTSGen::EmitExpressionStatement(const ir::AstNode *node){
    auto expressionstatement = static_cast<const panda::es2panda::ir::ExpressionStatement*>(node);
    
    this->EmitExpression(expressionstatement->GetExpression());
    this->writeTrailingSemicolon();
}


void ArkTSGen::SerializeNode(const ir::AstNode *node)
{
    // es2panda/ir/astNodeMapping.h
    switch(node->Type()){
        case AstNodeType::EXPRESSION_STATEMENT:
            std::cout << "enter EXPRESSION_STATEMENT >>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
            this->EmitExpressionStatement(node); 
            break;
        case AstNodeType::BLOCK_STATEMENT:
            std::cout << "enter BLOCK_STATEMENT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl; 
            this->EmitBlockStatement(node);
            break;
        default:
            std::cout << "enter SerializeNode Default  >>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
    }



    // Wrap(
    //     [this, node]() -> void {
    //         node->Dump(this);
    //     }
    // );
}




void ArkTSGen::Add(std::initializer_list<ArkTSGen::Property> props){
    AddList<std::initializer_list<ArkTSGen::Property>>(props);
}

void ArkTSGen::Add(const ArkTSGen::Property &prop){
    Serialize(prop);
}

const char *ArkTSGen::ModifierToString(ModifierFlags flags){
    if (flags & ModifierFlags::PRIVATE) {
        return "private";
    }

    if (flags & ModifierFlags::PROTECTED) {
        return "protected";
    }

    if (flags & ModifierFlags::PUBLIC) {
        return "public";
    }

    return nullptr;
}

const char *ArkTSGen::TypeOperatorToString(TSOperatorType operatorType)
{
    if (operatorType == TSOperatorType::KEYOF) {
        return "keyof";
    }

    if (operatorType == TSOperatorType::READONLY) {
        return "readonly";
    }

    if (operatorType == TSOperatorType::UNIQUE) {
        return "unique";
    }

    return nullptr;
}

void ArkTSGen::Serialize(const ArkTSGen::Property &prop)
{
    SerializePropKey(prop.Key());
    const auto &value = prop.Value();

    if (std::holds_alternative<const char *>(value)) {
        SerializeString(std::get<const char *>(value));
    } else if (std::holds_alternative<util::StringView>(value)) {
        SerializeString(std::get<util::StringView>(value));
    } else if (std::holds_alternative<bool>(value)) {
        SerializeBoolean(std::get<bool>(value));
    } else if (std::holds_alternative<double>(value)) {
        SerializeNumber(std::get<double>(value));
    } else if (std::holds_alternative<const ir::AstNode *>(value)) {
        if (dumpNodeOnly_) {
            SerializeNode(std::get<const ir::AstNode *>(value));
        } else {
            SerializeObject(std::get<const ir::AstNode *>(value));
        }
    } else if (std::holds_alternative<std::vector<const ir::AstNode *>>(value)) {
        SerializeArray(std::get<std::vector<const ir::AstNode *>>(value));
    } else if (std::holds_alternative<lexer::TokenType>(value)) {
        SerializeToken(std::get<lexer::TokenType>(value));
    } else if (std::holds_alternative<std::initializer_list<Property>>(value)) {
        SerializePropList(std::get<std::initializer_list<Property>>(value));
    } else if (std::holds_alternative<Property::Constant>(value)) {
        SerializeConstant(std::get<Property::Constant>(value));
    }
}

void ArkTSGen::SerializeToken(lexer::TokenType token)
{
    ss_ << "\"" << lexer::TokenToString(token) << "\"";
}

void ArkTSGen::SerializePropKey(const char *str)
{
    if (dumpNodeOnly_) {
        return;
    }
    ss_ << std::endl;
    Indent();
    SerializeString(str);
    ss_ << ": ";
}

void ArkTSGen::SerializeString(const char *str)
{
    ss_ << "\"" << str << "\"";
}

void ArkTSGen::SerializeString(const util::StringView &str)
{
    ss_ << "\"" << str.Utf8() << "\"";
}

void ArkTSGen::SerializeNumber(size_t number)
{
    ss_ << number;
}

void ArkTSGen::SerializeNumber(double number)
{
    if (std::isinf(number)) {
        ss_ << "\"Infinity\"";
    } else {
        ss_ << number;
    }
}

void ArkTSGen::SerializeBoolean(bool boolean)
{
    ss_ << (boolean ? "true" : "false");
}

void ArkTSGen::SerializeConstant(Property::Constant constant)
{
    switch (constant) {
        case Property::Constant::PROP_NULL: {
            ss_ << "null";
            break;
        }
        case Property::Constant::EMPTY_ARRAY: {
            ss_ << "[]";
            break;
        }
        default: {
            UNREACHABLE();
        }
    }
}

void ArkTSGen::SerializePropList(std::initializer_list<ArkTSGen::Property> props)
{
    Wrap(
        [this, &props]() -> void {
            for (const auto *it = props.begin(); it != props.end(); ++it) {
                Serialize(*it);
                if (std::next(it) != props.end()) {
                    ss_ << ',';
                }
            }
        }
    );
}

void ArkTSGen::SerializeArray(std::vector<const ir::AstNode *> array)
{
    Wrap(
        [this, &array]() -> void {
            for (auto it = array.begin(); it != array.end(); ++it) {
                if (dumpNodeOnly_) {
                    SerializeNode(*it);
                } else {
                    ss_ << std::endl;
                    Indent();
                    SerializeObject(*it);
                }

                if (std::next(it) != array.end()) {
                    ss_ << ',';
                }
            }
        },
        '[', ']');
}

void ArkTSGen::SerializeObject(const ir::AstNode *object)
{
    // Wrap(   
    //     [this, object]() -> void {
    //         object->Dump(this);
    //         SerializeLoc(object->Range());
    //     }
    // );
}

void ArkTSGen::Wrap(const WrapperCb &cb, char delimStart, char delimEnd)
{
    ss_ << delimStart;

    if (dumpNodeOnly_) {
        cb();
    } else {
        indent_++;
        cb();
        ss_ << std::endl;
        indent_--;
        Indent();
    }

    ss_ << delimEnd;
}


void ArkTSGen::Indent()
{
    for (int32_t i = 0; i < indent_; i++) {
        ss_ << "  ";
    }
}

}  // namespace panda::es2panda::ir
