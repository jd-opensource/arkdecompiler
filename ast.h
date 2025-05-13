#ifndef DECOMPILER_H
#define DECOMPILER_H

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


#include "../ets_frontend/es2panda/ir/base/property.h"
#include "../ets_frontend/es2panda/ir/base/spreadElement.h"


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

#include "../ets_frontend/es2panda/ir/expressions/newExpression.h"

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
#include "../ets_frontend/es2panda/ir/statements/debuggerStatement.h"
#include "../ets_frontend/es2panda/ir/statements/ifStatement.h"
#include "../ets_frontend/es2panda/ir/statements/tryStatement.h"

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

//////////////////////////////////////////////////////////////////////////////
#include "assembler/annotation.h"
#include "assembler/assembly-function.h"
#include "assembler/assembly-ins.h"

#include "ir_interface.h"
#include "compiler/optimizer/pass.h"
#include "compiler/optimizer/ir/basicblock.h"
#include "compiler/optimizer/ir/graph.h"
#include "compiler/optimizer/ir/graph_visitor.h"
#include "utils/logger.h"

    
#endif