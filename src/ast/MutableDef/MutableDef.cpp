#include "MutableDef.hpp"
#include "../../error/error.hpp"
#include "../../symbol/symbol.hpp"

extern SymbolTable *symbolTable;

MutableDef::MutableDef(std::string i, Type *t): id(i), type(t) {}

MutableDef::~MutableDef() { delete type; }

DefType MutableDef::getDefType() const { return DefType::DEF_MUTABLE; }

void MutableDef::printOn(std::ostream &out) const {
  out << "MutableDef(" << id;
  if (type != nullptr) out << "," << *type;
  out << ")";
}

void MutableDef::sem() {
  if (type == nullptr)
    Logger::error(lineno, "Definition of \"%s\" does not contain a type", id.c_str());
  if (type->getClassType() != TypeClassType::REF)
    Logger::error(lineno, "Mutable \"%s\" must be a REF type", id.c_str());
  symbolTable->newVariable(id, type, lineno);
}

llvm::Value* MutableDef::codegen() {
  llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();
  llvm::AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, id, getLLVMType(type));

  LLVMValueStore->newLLVMValue(id, Alloca);
  return Alloca;
}