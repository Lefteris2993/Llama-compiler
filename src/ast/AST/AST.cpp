#include "AST.hpp"

AST::~AST() {}

void AST::setLineno(unsigned l) { this->lineno = l; }

void AST::sem() { /* Do nothing */ }
