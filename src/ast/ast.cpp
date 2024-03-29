#include "AST/AST.cpp"
#include "Def/Def.cpp"
#include "Expr/Expr.cpp"
#include "ExprBlock/ExprBlock.cpp"
#include "NumPattern/NumPattern.cpp"
#include "CharPattern/CharPattern.cpp"
#include "BoolPattern/BoolPattern.cpp"
#include "VarPattern/VarPattern.cpp"
#include "Clause/Clause.cpp"
#include "ClauseBlock/ClauseBlock.cpp"
#include "BinOpExpr/BinOpExpr.cpp"
#include "SigOpExpr/SigOpExpr.cpp"
#include "Par/Par.cpp"
#include "ParBlock/ParBlock.cpp"
#include "ImmutableDefFunc/ImmutableDefFunc.cpp"
#include "DefBlock/DefBlock.cpp"
#include "LetDef/LetDef.cpp"
#include "LetInExpr/LetInExpr.cpp"
#include "WhileExpr/WhileExpr.cpp"
#include "ForExpr/ForExpr.cpp"
#include "DimExpr/DimExpr.cpp"
#include "NewExpr/NewExpr.cpp"
#include "DeleteExpr/DeleteExpr.cpp"
#include "IfThenElseExpr/IfThenElseExpr.cpp"
#include "MatchExpr/MatchExpr.cpp"
#include "DeRefHighPrioExpr/DeRefHighPrioExpr.cpp"
#include "UnitHighPrioExpr/UnitHighPrioExpr.cpp"
#include "IntHighPrioExpr/IntHighPrioExpr.cpp"
#include "CharHighPrioExpr/CharHighPrioExpr.cpp"
#include "StringHighPrioExpr/StringHighPrioExpr.cpp"
#include "BoolHighPrioExpr/BoolHighPrioExpr.cpp"
#include "IdHighPrioExpr/IdHighPrioExpr.cpp"
#include "HighPrioExprBlock/HighPrioExprBlock.cpp"
#include "StmtBlock/StmtBlock.cpp"
#include "MutableDef/MutableDef.cpp"
#include "MutableArrayDef/MutableArrayDef.cpp"
#include "ImmutableDefVar/ImmutableDefVar.cpp"
#include "FunctionCall/FunctionCall.cpp"
