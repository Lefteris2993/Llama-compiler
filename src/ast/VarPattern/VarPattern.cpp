#include "VarPattern.hpp"
#include "../../symbol/symbol.hpp"
#include "../../error/error.hpp"

extern SymbolTable *symbolTable;

VarPattern::VarPattern(std::string v): var(v) {}

void VarPattern::printOn(std::ostream &out) const {
  out << "VarPattern(" << var << ")";
}

void VarPattern::sem() {
  SymbolEntry *s = symbolTable->lookupEntry<SymbolEntry>(var, true, lineno);
  if (
    s->entryType == EntryType::ENTRY_FUNCTION &&
    ((FunSymbolEntry *) s)->paramNum != 0
  )
    Logger::error(lineno, "\"%s\" is function 1", s->id.c_str());
  type = s->type;
}
