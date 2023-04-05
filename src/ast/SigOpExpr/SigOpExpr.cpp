#include "SigOpExpr.hpp"
#include "../../error/error.hpp"

SigOpExpr::SigOpExpr(SigOp p, Expr *e): expr(e), op(p) {}

SigOpExpr::~SigOpExpr() { delete expr; }

void SigOpExpr::printOn(std::ostream &out) const {
  out << "SigOp(" << op << "," << *expr << ")";
}

void SigOpExpr::sem() {
  expr->sem();
  switch (op) {
    case SigOp::NOT: {
      if (!Type::equal_types(expr->getType(), boolType))
        Logger::error(lineno, "Invalid argument for \"not\" expresion");
      type = boolType;
      break;
    }
    case SigOp::SIG_MINUS:
    case SigOp::SIG_PLUS: {
      if (!Type::equal_types(expr->getType(), intType))
        Logger::error(lineno, "Invalid argument for sign expresion");
      type = intType;
      break;
    }
    default: {
      Logger::error(lineno, "Not implemented");
      break;
  }
  }
}

llvm::Value* SigOpExpr::codegen() {
  llvm::Value *v = expr->codegen();
  switch (op) {
    case SigOp::NOT: 
      return v;
    case SigOp::SIG_MINUS:
      return Builder.CreateMul(v, c32(-1));
    case SigOp::SIG_PLUS:
      return Builder.CreateNot(v);
    default:
      return nullptr;
  }
}
