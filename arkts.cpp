#include "arkts.h"

namespace panda::es2panda::ir {

ArkTSGen::ArkTSGen(const BlockStatement *program, util::StringView sourceCode) : index_(sourceCode), indent_(0)
{
    SerializeObject(reinterpret_cast<const ir::AstNode *>(program));
}

ArkTSGen::ArkTSGen(const ir::AstNode *node) : indent_(0), dumpNodeOnly_(true)
{
    EmitStatement(node);
}

void ArkTSGen::EmitBlockStatement(const ir::AstNode *node){
    auto tmp = const_cast<ir::AstNode*>(node);
    auto blockstatement = tmp->AsBlockStatement();

    const auto &statements = blockstatement->Statements();
    
    if(statements.size() == 0){
       // this->WriteNewLine();
    }

    for (const auto *stmt : statements) {
        this->EmitStatement(stmt);
    }
}

void ArkTSGen::WriteTrailingSemicolon(){
    ss_ << ";" << std::endl;
}

void ArkTSGen::WriteSpace(){
    ss_ << " ";
}

void ArkTSGen::WriteLeftBrace(){
    ss_ << "{";
}

void ArkTSGen::WriteRightBrace(){
    ss_ << "}";
}

void ArkTSGen::WriteLeftBracket(){
    ss_ << "[";
}

void ArkTSGen::WriteRightBracket(){
    ss_ << "]";
}

void ArkTSGen::WriteLeftParentheses(){
    ss_ << "(";
}

void ArkTSGen::WriteRightParentheses(){
    ss_ << ")";
}

void ArkTSGen::WriteColon(){
    ss_ << " : ";
}

void ArkTSGen::WriteEqual(){
    ss_ << " = ";
}

void ArkTSGen::WriteComma(){
    ss_ << ", ";
}

void ArkTSGen::WriteDot(){
    ss_ << ".";
}

void ArkTSGen::WriteKeyWords(std::string keyword){
    ss_ << keyword ;
}

void ArkTSGen::WriteSpreadDot(){
    ss_ << "...";
}

void ArkTSGen::WriteNewLine(){
    ss_ << "\n";
}

void ArkTSGen::WriteIndent(){
    for (int i = 0; i < this->indent_; ++i) {
        ss_ << " ";
    } 
}

void ArkTSGen::EmitExpression(const ir::AstNode *node){
    if(node == nullptr){
        HandleError("#EmitExpression: emitExpression for null astnode");
    }

    switch(node->Type()){ 
        case AstNodeType::BINARY_EXPRESSION:{
            std::cout << "enter BINARY_EXPRESSION >>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl; 
            auto binexpression = node->AsBinaryExpression();

            this->EmitExpression(binexpression->Left());
            WriteSpace();
            ss_ << TokenToString(binexpression->OperatorType());
            WriteSpace();
            this->EmitExpression(binexpression->Right());
            break;
        }

        case AstNodeType::UNARY_EXPRESSION:{
            std::cout << "enter UNARY_EXPRESSION >>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl; 
            auto unaryexpression = node->AsUnaryExpression();
            ss_ << TokenToString(unaryexpression->OperatorType());
            WriteSpace();
            this->EmitExpression(unaryexpression->Argument());
            break;
        }

        case AstNodeType::ASSIGNMENT_EXPRESSION:{
            auto assignexpression = node->AsAssignmentExpression();
            std::cout << "enter ASSIGNMENT_EXPRESSION >>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl; 
            
            this->EmitExpression(assignexpression->Left());
            WriteSpace();
            ss_ << TokenToString(assignexpression->OperatorType());
            WriteSpace();
            this->EmitExpression(assignexpression->Right());
            break;
        }
        case AstNodeType::IDENTIFIER:{
            auto identifier = node->AsIdentifier();
            ss_ << identifier->Name();
            break;
        }

        case AstNodeType::NUMBER_LITERAL:{
            auto number_literal = node->AsNumberLiteral();
            this->SerializeNumber(number_literal->Number());
            break;
        } 

        case AstNodeType::BIGINT_LITERAL:{
            auto bigint_literal = node->AsBigIntLiteral();
            this->SerializeString(bigint_literal->Str());
            break;
        } 

        case AstNodeType::NULL_LITERAL:{
            this->SerializeConstant(Property::Constant::PROP_NULL);
            break;
        }

        case AstNodeType::STRING_LITERAL:{
            auto string_literal = node->AsStringLiteral();
            this->SerializeString(string_literal->Str());
            break;
        }
        
        case AstNodeType::BOOLEAN_LITERAL:{
            auto bool_literal = node->AsBooleanLiteral();
            this->SerializeBoolean(bool_literal->Value());
            break;
        }
        case AstNodeType::MEMBER_EXPRESSION:{
            auto member_expression = node->AsMemberExpression();
            this->EmitExpression(member_expression->Object());
            if(member_expression->IsComputed()){
                this->WriteLeftBracket();
                this->EmitExpression(member_expression->Property());
                this->WriteRightBracket();
            }else{
                this->WriteDot();
                this->EmitExpression(member_expression->Property());
            }
            break;
        }

        case AstNodeType::OBJECT_EXPRESSION:{
            std::cout << "enter OBJECT_EXPRESSION >>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl; 
            auto objexpression = node->AsObjectExpression();
            
            WriteLeftBrace();
            size_t properties_size = objexpression->Properties().size();
            size_t count = 1;
            for (auto *it : objexpression->Properties()) {
                switch (it->Type()) {
                    case AstNodeType::PROPERTY: {
                        this->EmitExpression(it);
                        
                        if(count++ < properties_size)
                            this->WriteComma();
                        
                        break;
                    }
                    case AstNodeType::SPREAD_ELEMENT:{
                        this->EmitExpression(it);
                        
                        if(count++ < properties_size)
                            this->WriteComma();
                        
                        break;
                    }
                    default: {
                        std::cout << "unsupport AstNodeType >>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
                        break;
                    }
                }
            }
            WriteRightBrace();

            break;

        }

        case AstNodeType::ARRAY_EXPRESSION:{
            std::cout << "enter OBJECT_EXPRESSION >>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl; 
            auto arrayexpression = node->AsArrayExpression();
            
            WriteLeftBracket();
            int count = 0;
            int array_size = arrayexpression->Elements().size();
            for (auto *it : arrayexpression->Elements()) {
                this->EmitExpression(it);
                if(++count < array_size ){
                    this->WriteComma();
                }
            }
            WriteRightBracket();

            break;

        }

        case AstNodeType::PROPERTY:{
            auto propertyexpression = node->AsProperty();

            this->EmitExpression(propertyexpression->Key());
            this->WriteColon();
            this->EmitExpression(propertyexpression->Value());
            

            break;
        }

        case AstNodeType::CALL_EXPRESSION:{
            auto callexpression = node->AsCallExpression();

            this->EmitExpression(callexpression->Callee());
            this->WriteLeftParentheses();

            int count = 1;
            int argumentsize = callexpression->Arguments().size();
            for (const auto *it : callexpression->Arguments()) {
                this->EmitExpression(it);
                if(count ++ < argumentsize){
                    this->WriteComma();
                }
            }

            this->WriteRightParentheses();

            break;
        }

        case AstNodeType::NEW_EXPRESSION:{
            auto newexpression = node->AsNewExpression();

            this->WriteKeyWords("new");
            this->WriteSpace();
            this->EmitExpression(newexpression->Callee());
            this->WriteLeftParentheses();

            int count = 1;
            int argumentsize = newexpression->Arguments().size();
            for (const auto *it : newexpression->Arguments()) {
                this->EmitExpression(it);
                if(count ++ < argumentsize){
                    this->WriteComma();
                }
            }

            this->WriteRightParentheses();

            break;
        }

        case AstNodeType::SPREAD_ELEMENT:{
            this->WriteSpreadDot();
            auto spreadarg = node->AsSpreadElement();
            this->EmitExpression(spreadarg->Argument());
            break;
        }

        case AstNodeType::AWAIT_EXPRESSION:{
            auto awaitexpression = node->AsAwaitExpression();
            this->WriteKeyWords("await");
            this->WriteSpace();
            this->EmitExpression(awaitexpression->Argument());
            break;
        }

        case AstNodeType::YIELD_EXPRESSION:{
            auto awaitexpression = node->AsYieldExpression();
            this->WriteKeyWords("yield");
            this->WriteSpace();
            this->EmitExpression(awaitexpression->Argument());
            break;
        }

        case AstNodeType::OMITTED_EXPRESSION:{
            this->WriteSpace();
            break;
        }

        default:
            HandleError("#EmitExpression : unsupport expression");;

    }
}

void ArkTSGen::EmitExpressionStatement(const ir::AstNode *node){
    auto expressionstatement = node->AsExpressionStatement();
    this->EmitExpression(expressionstatement->GetExpression());
    this->WriteTrailingSemicolon();
}

void ArkTSGen::EmitVariableDeclarationStatement(const ir::AstNode *node){
     auto vardeclstatement = node->AsVariableDeclaration();
    
    int size = vardeclstatement->Declarators().size();
    int count = 1;
    
    if(vardeclstatement->Kind() == es2panda::ir::VariableDeclaration::VariableDeclarationKind::CONST){
        this->WriteKeyWords("const");
        this->WriteSpace();
    }else if(vardeclstatement->Kind() == es2panda::ir::VariableDeclaration::VariableDeclarationKind::LET){
        this->WriteKeyWords("let");
        this->WriteSpace();
    }else {
        this->WriteKeyWords("var");
        this->WriteSpace();
    }

    for (const auto *it : vardeclstatement->Declarators()) {
        this->EmitStatement(it);
        if(++count < size ){
            this->WriteColon();
        }
    }
    this->WriteTrailingSemicolon();
}

void ArkTSGen::EmitVariableDeclaratorStatement(const ir::AstNode *node){
    auto vardeclstatement = node->AsVariableDeclarator();

    this->EmitExpression(vardeclstatement->Id());
    this->WriteEqual();
    this->EmitExpression(vardeclstatement->Init());
}


void ArkTSGen::EmitReturnStatement(const ir::AstNode *node){
    auto returnstatement = node->AsReturnStatement();
    this->WriteKeyWords("return");
    this->WriteSpace();
    this->EmitExpression(returnstatement->Argument());
    this->WriteTrailingSemicolon();
}

void ArkTSGen::EmitBreakStatement(const ir::AstNode *node){
    this->WriteKeyWords("break");
    this->WriteTrailingSemicolon();
}

void  ArkTSGen::EmitDebuggerStatement(const ir::AstNode *node){
    this->WriteKeyWords("debugger");
    this->WriteTrailingSemicolon();
}

void  ArkTSGen::EmitThrowStatement(const ir::AstNode *node){
    auto throwstatement = node->AsThrowStatement();

    this->WriteKeyWords("throw");
    this->WriteSpace();
    this->EmitExpression(throwstatement->Argument());
    this->WriteTrailingSemicolon();
}


void  ArkTSGen::EmitFunctionDeclaration(const ir::AstNode *node){
    auto fundeclare = node->AsFunctionDeclaration();
    auto scriptfunction =  fundeclare->Function();

    if(scriptfunction->IsAsync()){
        this->WriteKeyWords("async");
        this->WriteSpace();    
    }
    this->WriteKeyWords("function");
    this->WriteSpace();
    this->EmitExpression(scriptfunction->Id());
    this->WriteLeftParentheses();

    int count = 1;
    int argumentsize = scriptfunction->Params().size();
    for (const auto *param : scriptfunction->Params()) {
        // if(count > 3){
        //     this->EmitExpression(param);
        // }else{
        //     count++;
        //     continue;
        // }
        this->EmitExpression(param);
        if(count ++ < argumentsize){
            this->WriteComma();
        }
    }
    this->WriteRightParentheses();
    this->WriteLeftBrace();
    this->WriteNewLine();
    this->indent_ = this->indent_ + this->singleindent_;

    this->EmitStatement(scriptfunction->Body());

    this->indent_ = this->indent_ - this->singleindent_;
    this->WriteRightBrace();
    this->WriteNewLine();
}

void ArkTSGen::EmitTryStatement(const ir::AstNode *node){
    auto trystatement = node->AsTryStatement();
    
    // if(test)
    this->WriteKeyWords("try");
    this->WriteLeftBrace();
    this->WriteNewLine();

    //  statements
    this->indent_ = this->indent_ + this->singleindent_;
    this->EmitStatement(trystatement->Block());
    this->indent_ = this->indent_ - this->singleindent_;
    this->WriteIndent();
    this->WriteRightBrace();

    // }catch(error){
    this->WriteKeyWords("catch");
    this->WriteLeftParentheses();
    this->EmitExpression(trystatement->GetCatchClause()->Param());
    this->WriteRightParentheses();
    this->WriteLeftBrace();
    this->WriteNewLine();

    // catch body
    this->indent_ = this->indent_ + this->singleindent_;
    this->EmitStatement(trystatement->GetCatchClause()->Body());
    this->indent_ = this->indent_ - this->singleindent_;

    // }
    this->WriteIndent();
    this->WriteRightBrace();
    this->WriteNewLine();
}

void ArkTSGen::EmitIfStatement(const ir::AstNode *node){
    std::cout << "[+] start EmitIfStatement"  << std::endl;
    auto ifstatement = node->AsIfStatement();
    if(ifstatement->Consequent() == nullptr ||  ifstatement->Consequent()->AsBlockStatement()->Statements().size() == 0){
        return;
    }

    // if(test)
    this->WriteKeyWords("if");
    this->WriteLeftParentheses();
    this->EmitExpression(ifstatement->Test());
    this->WriteRightParentheses();
    this->WriteLeftBrace();
    this->WriteNewLine();

    // if statements
    this->indent_ = this->indent_ + this->singleindent_;
    this->EmitStatement(ifstatement->Consequent());
    this->indent_ = this->indent_ - this->singleindent_;
    this->WriteIndent();
    this->WriteRightBrace();

    if(ifstatement->Alternate() != nullptr && ifstatement->Alternate()->AsBlockStatement()->Statements().size() > 0){
        // }else{
        this->WriteKeyWords("else");
        this->WriteLeftBrace();
        this->WriteNewLine();

        // else statements
        this->indent_ = this->indent_ + this->singleindent_;
        this->EmitStatement(ifstatement->Alternate());
        this->indent_ = this->indent_ - this->singleindent_;

        // }
        this->WriteIndent();
        this->WriteRightBrace();
    }
    this->WriteNewLine();
    
    std::cout << "[-] end EmitIfStatement"  << std::endl;
}

void ArkTSGen::EmitWhileStatement(const ir::AstNode *node){
    std::cout << "[+] start emit while statement"  << std::endl;
    auto whilestatement = node->AsWhileStatement();

    // while(test){
    this->WriteKeyWords("while");
    this->WriteLeftParentheses();
    this->EmitExpression(whilestatement->Test());
    this->WriteRightParentheses();
    this->WriteLeftBrace();
    this->WriteNewLine();

    // while statement
    this->indent_ = this->indent_ + this->singleindent_;
    this->EmitStatement(whilestatement->Body());
    this->indent_ = this->indent_ - this->singleindent_;

    // }
    this->WriteIndent();
    this->WriteRightBrace();
    this->WriteNewLine();
        
    std::cout << "[-] end emit while statement"  << std::endl;
}

void ArkTSGen::EmitDoWhileStatement(const ir::AstNode *node){
    std::cout << "[+] start emit dowhile statement"  << std::endl;
    auto dowhilestatement = node->AsDoWhileStatement();
    
    // do {
    this->WriteKeyWords("do");
    this->WriteLeftBrace();
    this->WriteNewLine();

    // while statement
    this->indent_ = this->indent_ + this->singleindent_;
    this->EmitStatement(dowhilestatement->Body());
    this->indent_ = this->indent_ - this->singleindent_;

    // }while(test)
    this->WriteIndent();
    this->WriteRightBrace();
    this->WriteKeyWords("while");
    this->WriteLeftParentheses();
    this->EmitExpression(dowhilestatement->Test());
    this->WriteRightParentheses();
    this->WriteTrailingSemicolon();        
    std::cout << "[-] end emit dowhile statement"  << std::endl;
}


void ArkTSGen::EmitImportSpecifier(const ir::AstNode *node){
    std::cout << "[+] start emit import specifier statement"  << std::endl;
    auto importspecifier = node->AsImportSpecifier();
    
    this->WriteKeyWords("import");

    this->WriteSpace();
    this->EmitExpression(importspecifier->Imported());

    if(importspecifier->Local() != importspecifier->Imported()){
        this->WriteSpace();
        this->WriteKeyWords("as");
        this->WriteSpace();
        this->EmitExpression(importspecifier->Local());
    }
    this->WriteTrailingSemicolon();
}

void ArkTSGen::EmitImportDeclaration(const ir::AstNode *node){
    std::cout << "[+] start emit import declaration statement"  << std::endl;
    auto importdeclaration = node->AsImportDeclaration();
    
    for (const auto *astnode : importdeclaration->Specifiers()) {
        auto importspecifier = astnode->AsImportSpecifier();
        this->WriteKeyWords("import");

        this->WriteSpace();
        this->EmitExpression(importspecifier->Imported());

        if(importspecifier->Local() != importspecifier->Imported()){
            this->WriteSpace();
            this->WriteKeyWords("as");
            this->WriteSpace();
            this->EmitExpression(importspecifier->Local());
        }

        this->WriteSpace();
        this->WriteKeyWords("from");
        this->WriteSpace();

        this->EmitExpression(importdeclaration->Source());
    }
    this->WriteTrailingSemicolon();
}

void ArkTSGen::EmitExportAllDeclaration(const ir::AstNode *node){
    std::cout << "[+] start emit export all declaration statement"  << std::endl;
    auto exportdeclaration = node->AsExportAllDeclaration();
    
    this->WriteKeyWords("export");
    this->WriteSpace();
    this->WriteKeyWords("*");
    this->WriteSpace();
    this->WriteKeyWords("from");
    this->WriteSpace();
    this->EmitExpression(exportdeclaration->Source());
    this->WriteTrailingSemicolon();
}

void ArkTSGen::EmitExportSpecifier(const ir::AstNode *node){
    std::cout << "[+] start emit export specifier statement"  << std::endl;
    auto exportspecifier = node->AsExportSpecifier();
    
    this->WriteKeyWords("export");
    this->WriteSpace();
    this->EmitExpression(exportspecifier->Local());
    this->WriteSpace();
    this->WriteKeyWords("as");
    this->WriteSpace();
    this->EmitExpression(exportspecifier->Exported());
    this->WriteTrailingSemicolon();
}

void ArkTSGen::EmitExportNamedDeclaration(const ir::AstNode *node){
    std::cout << "[+] start emit export named declaration statement"  << std::endl;
    auto exportnameddeclaration = node->AsExportNamedDeclaration();
    
    for (const auto *astnode : exportnameddeclaration->Specifiers()) {
        auto exportspecifier = astnode->AsExportSpecifier();
        this->WriteKeyWords("export");

        this->WriteSpace();
        this->EmitExpression(exportspecifier->Local());

        if(exportspecifier->Local() != exportspecifier->Exported()){
            this->WriteSpace();
            this->WriteKeyWords("as");
            this->WriteSpace();
            this->EmitExpression(exportspecifier->Exported());
        }

        this->WriteSpace();
        this->WriteKeyWords("from");
        this->WriteSpace();

        this->EmitExpression(exportnameddeclaration->Source());
    }
    this->WriteTrailingSemicolon();
}

void ArkTSGen::EmitClassDeclaration(const ir::AstNode *node){
    std::cout << "[+] start emit class declaration statement"  << std::endl;
    auto classdeclaration = node->AsClassDeclaration();
    auto classdefinition = const_cast<ir::ClassDefinition*>(classdeclaration->Definition());

    this->WriteKeyWords("class");
    this->WriteSpace();
    std::cout << classdefinition->Ident()->Name().Mutf8()  << std::endl;
    this->EmitExpression(classdefinition->Ident());
    this->WriteSpace();

    if(classdefinition->Super()){
        this->WriteKeyWords("extends");
        this->WriteSpace();
        this->EmitExpression(classdefinition->Super());
        this->WriteSpace();
    }

    this->WriteLeftBrace();
    this->WriteNewLine();

    // constructor
    this->indent_ = this->indent_ + this->singleindent_;
    this->EmitStatement(classdefinition->Ctor());

    
    // member function
    const ArenaVector<es2panda::ir::Statement *> &statements = classdefinition->Body();
    for (es2panda::ir::Statement *stmt : statements) {
        this->EmitStatement(stmt);
    }

    this->indent_ = this->indent_ - this->singleindent_;

    this->WriteRightBrace();
    this->WriteTrailingSemicolon();
    this->WriteNewLine();
}

void ArkTSGen::EmitMethodDefinition(const ir::AstNode *node){
    std::cout << "[+] start emit method definition statement"  << std::endl;
    auto methoddefinition = node->AsMethodDefinition();
    
    this->EmitExpression(methoddefinition->Key());
    this->WriteLeftParentheses();

    auto scriptfunction = methoddefinition->Value()->Function()->AsScriptFunction();

    int count = 1;
    int argumentsize = scriptfunction->Params().size();
    for (const auto *param : scriptfunction->Params()) {
        // if(count > 3){
        //     this->EmitExpression(param);
        // }else{
        //     count++;
        //     continue;
        // }
        this->EmitExpression(param);
        if(count ++ < argumentsize){
            this->WriteComma();
        }
    }
    this->WriteRightParentheses();
    this->WriteLeftBrace();
    this->WriteNewLine();
    this->indent_ = this->indent_ + this->singleindent_;
    this->EmitStatement(scriptfunction->Body());
    this->indent_ = this->indent_ - this->singleindent_;
    this->WriteIndent();
    this->WriteRightBrace();
    this->WriteNewLine();
}


void ArkTSGen::EmitStatement(const ir::AstNode *node)
{
    if(node == nullptr){
        HandleError("#EmitStatement: emitStatement for null astnode");
    }

    if(node->Type() != AstNodeType::BLOCK_STATEMENT && node->Type() != AstNodeType::VARIABLE_DECLARATOR ){
        this->WriteIndent();
    }

    std::cout << "emit statement start " << std::endl;
    switch(node->Type()){
        case AstNodeType::EXPRESSION_STATEMENT:
            std::cout << "enter EXPRESSION_STATEMENT >>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
            this->EmitExpressionStatement(node); 
            break;
        case AstNodeType::BLOCK_STATEMENT:
            std::cout << "enter BLOCK_STATEMENT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl; 
            this->EmitBlockStatement(node);
            break;

        case AstNodeType::VARIABLE_DECLARATION:
            std::cout << "enter VARIABLE_DECLARATION >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl; 
            this->EmitVariableDeclarationStatement(node);
            break;

        case AstNodeType::VARIABLE_DECLARATOR:
            std::cout << "enter VARIABLE_DECLARATO >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl; 
            this->EmitVariableDeclaratorStatement(node);
            break;

        case AstNodeType::RETURN_STATEMENT:
            std::cout << "enter RETURN STATEMENT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl; 
            this->EmitReturnStatement(node);
            break;

        case AstNodeType::DEBUGGER_STATEMENT:
            std::cout << "enter DEBUGGER STATEMENT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl; 
            this->EmitDebuggerStatement(node);
            break;

        case AstNodeType::FUNCTION_DECLARATION:
            std::cout << "enter FUNCTION_DECLARATION STATEMENT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl; 
            this->EmitFunctionDeclaration(node);
            break;

        case AstNodeType::IF_STATEMENT:
            std::cout << "enter IF_STATEMENT STATEMENT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl; 
            this->EmitIfStatement(node);
            break;
        
        case AstNodeType::TRY_STATEMENT:
            std::cout << "enter TRY_STATEMENT STATEMENT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl; 
            this->EmitTryStatement(node);
            break;

        case AstNodeType::THROW_STATEMENT:
            std::cout << "enter THROW_STATEMENT STATEMENT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl; 
            this->EmitThrowStatement(node);
            break;

        case AstNodeType::WHILE_STATEMENT:
            std::cout << "enter WHILE_STATEMENT STATEMENT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl; 
            this->EmitWhileStatement(node);
            break;
        
        case AstNodeType::DO_WHILE_STATEMENT:
            std::cout << "enter DOWHILE_STATEMENT STATEMENT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl; 
            this->EmitDoWhileStatement(node);
            break;

        case AstNodeType::BREAK_STATEMENT:
            std::cout << "enter BREAK_STATEMENT STATEMENT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl; 
            this->EmitBreakStatement(node);
            break;

        case AstNodeType::IMPORT_SPECIFIER:{
            std::cout << "enter ImportSpecifier STATEMENT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
            this->EmitImportSpecifier(node);
            break;
        }

        case AstNodeType::IMPORT_DECLARATION:{
            std::cout << "enter IMPORT_DECLARATION STATEMENT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
            this->EmitImportDeclaration(node);
            break;
        }

        case AstNodeType::EXPORT_ALL_DECLARATION:{
            std::cout << "enter EXPORT_ALL_DECLARATION STATEMENT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
            this->EmitExportAllDeclaration(node);
            break;
        }

        case AstNodeType::EXPORT_SPECIFIER:{
            std::cout << "enter EXPORT_SPECIFIER STATEMENT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
            this->EmitExportSpecifier(node);
            break;
        }

        case AstNodeType::EXPORT_NAMED_DECLARATION:{
            std::cout << "enter EXPORT_NAMED_DECLARATION STATEMENT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
            this->EmitExportNamedDeclaration(node);
            break;
        }

        case AstNodeType::CLASS_DECLARATION: {
            std::cout << "enter CLASS_DECLARATION STATEMENT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
            this->EmitClassDeclaration(node);
            break;
        }

        case AstNodeType::METHOD_DEFINITION:{
            std::cout << "enter METHOD_DEFINITION STATEMENT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
            this->EmitMethodDefinition(node);
            break;
        }

        case AstNodeType::FUNCTION_EXPRESSION:{
            std::cout << "enter FUNCTION_EXPRESSION STATEMENT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
            break;
        }

        default:
            std::cout << "--------------------------------------------------------------------" << std::endl;
            HandleError("#EmitStatement : unsupport statement");
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
            EmitStatement(std::get<const ir::AstNode *>(value));
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
        case Property::Constant::PROP_UNDEFINED: {
            ss_ << "undefined";
            break;
        }
        default: {
            std::cout << "S1" << std::endl;
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
                    EmitStatement(*it);
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
