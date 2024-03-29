#include "BinOpExpr.hpp"
#include "../../error/error.hpp"

BinOpExpr::BinOpExpr(Expr *l, BinOp p, Expr *r): lhs(l), rhs(r), op(p) {}

BinOpExpr::~BinOpExpr() { delete lhs; delete rhs; }

void BinOpExpr::printOn(std::ostream &out) const {
  out << "BinOpExpr(" << op << "," << *lhs << "," << *rhs << ")";
}

void BinOpExpr::sem() {
  lhs->sem();
  rhs->sem();

  Type *l = lhs->getType();
  Type *r = rhs->getType();
  switch (op) {
    case BIN_PLUS:
    case BIN_MINUS:
    case STAR:
    case DIV:
    case MOD:
    case L:
    case G:
    case LE:
    case GE: {
      if (
        !Type::equal_types(l, intType) 
        || !Type::equal_types(r, intType)
      ) {
        Logger::error(lineno, "Operands must be of type INT");
      }
      type = intType;
      break; 
    }
    
    case STRUCT_EQ:
    case STRUCT_NE:
    case EQ:
    case NE:
    case AND:
    case OR: {
      if (
        !Type::equal_types(l, r) ||
        (!Type::equal_types(l, boolType) && !Type::equal_types(l, intType))
      ) {
        Logger::error(lineno, "Operands must be of type INT or BOOL");
      }
      type = boolType;
      break;
    }
    
    case ASS: {
      if (l == nullptr)
        Logger::error(lineno, "Expresion has unkown type");
      else if (l->getClassType() != TypeClassType::REF) 
        Logger::error(lineno, "Left side of operator \":=\" is not a reference type");
      else if (((RefType *) l)->getType() == nullptr) {
        ((RefType *) l)->setType(r);
      }
      else if (!Type::equal_types(((RefType *) l)->getType(), r))
        Logger::error(lineno, "Left and right side of assignment operator \":=\" have not the same type");
      
      type = unitType;
      break;
    }

    case PAR: {
      type = rhs->getType();
      break;
    }
      
    default: {
      Logger::error(lineno, "Not implemented");
      break;
    }
  }
}

llvm::Value* BinOpExpr::codegen() {
  llvm::Value *l = lhs->codegen();
  llvm::Value *r = rhs->codegen();

  switch (op) {
    case BIN_PLUS: 
      return Builder.CreateAdd(l, r, "add_tmp");
    case BIN_MINUS:
      return Builder.CreateSub(l, r, "sub_tmp");
    case STAR:
      return Builder.CreateMul(l, r);
    case DIV:
      return Builder.CreateSDiv(l, r);
    case MOD:
      return Builder.CreateSRem(l, r);
    case L:
      return Builder.CreateICmpULT(l, r);
    case G:
      return Builder.CreateICmpSGT(l, r, "gtr_tmp");
    case LE:
      return Builder.CreateICmpULE(l, r);
    case GE:
      return Builder.CreateICmpSGE(l, r);
    case STRUCT_EQ:
      return Builder.CreateICmpEQ(l, r, "eq_temp");
    case STRUCT_NE:
      return Builder.CreateICmpNE(l, r);
    case EQ:
    case NE:
      // not implemented
      return nullptr;
    case AND:
      return Builder.CreateAnd(l, r);
    case OR:
      return Builder.CreateOr(l, r);
    case ASS: {
      Builder.CreateStore(r, l);
      return llvm::ConstantAggregateZero::get(TheModule->getTypeByName("unit"));
    }
    case PAR:
      return llvm::ConstantAggregateZero::get(TheModule->getTypeByName("unit"));
    default:
      return nullptr;
  }
}