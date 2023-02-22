#include "FunctionCall.hpp"
#include "../../symbol/symbol.hpp"
#include "../../error/error.hpp"

extern SymbolTable *symbolTable;

FunctionCall::FunctionCall(std::string id, HighPrioExprBlock *b): id(id), block(b) {}

FunctionCall::~FunctionCall() { delete block; }

void FunctionCall::printOn(std::ostream &out) const {
  out << "FunctionCall(" << id << "," << *block << ")";
}

void FunctionCall::sem() {
  FunSymbolEntry *f = symbolTable->lookupEntry<FunSymbolEntry>(id, true, lineno);
  if (f->entryType != EntryType::ENTRY_FUNCTION) 
    Logger::error(lineno, "\"%s\" is not a function", f->id.c_str());

  if (f->paramNum != block->getBlockLength())
    Logger::error(lineno, "Incorrect number of parameters passed in function :%s", id.c_str());

  block->sem();
  block->parCheck(f);

  type = f->returnType;
}