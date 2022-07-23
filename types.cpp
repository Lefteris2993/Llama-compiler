#include "types.hpp"
#include <iostream>


Type::~Type() {}

bool Type::equal_types(const Type *t1, const Type *t2) {
  if (
    t1 == nullptr ||
    t2 == nullptr
  ) return false;
  
  TypeClassType cl = t1->classType;
  if (t1->classType != t2-> classType) return false;

  switch (cl)
  {
  case TypeClassType::SIMPLE: {
    BaseType b1 = ((SimpleType *) t1)->getType();
    BaseType b2 = ((SimpleType *) t2)->getType();
    return b1 == b2;
  }
    
  case TypeClassType::FUNCTION: {
    Type *l1 = ((FunctionType *) t1)->getLeft();
    Type *r1 = ((FunctionType *) t1)->getRight();

    Type *l2 = ((FunctionType *) t2)->getLeft();
    Type *r2 = ((FunctionType *) t2)->getRight();
    
    return Type::equal_types(l1, l2) && Type::equal_types(r1, r2);
  }
    
  case TypeClassType::ARRAY: {
    Type *a1 = ((ArrayType *) t1)->getType();
    unsigned s1 = ((ArrayType *) t1)->getStars();

    Type *a2 = ((ArrayType *) t2)->getType();
    unsigned s2 = ((ArrayType *) t2)->getStars();
    
    return Type::equal_types(a1, a2) && s1 == s2;
  }
    
  case TypeClassType::REF: {
    Type *a1 = ((RefType *) t2)->getType();
    Type *a2 = ((RefType *) t2)->getType();
    return Type::equal_types(a1, a2);
  }
  
  default:
    return false;
  }
}

TypeClassType Type::getClassType() { return classType; }

SimpleType::SimpleType(BaseType t): type(t) { this->classType = SIMPLE; }
void SimpleType::printOn(std::ostream &out) const {
  out << "SimpleType(" << type << ")";
}
BaseType SimpleType::getType() { return this->type; }

FunctionType::FunctionType(Type *l, Type *r): left(l), right(r) { this->classType = FUNCTION; }
FunctionType::~FunctionType() { delete left; delete right; }
void FunctionType::printOn(std::ostream &out) const {
  out << "FunctionType(" << *left << "," << *right << ")";
}
Type* FunctionType::getLeft() { return this->left; }
Type* FunctionType::getRight() { return this->right; }


ArrayType::ArrayType(unsigned s, Type *t): stars(s), type(t) { this->classType = ARRAY; }
ArrayType::~ArrayType() { delete type; }
void ArrayType::printOn(std::ostream &out) const {
  out << "ArrayType(" << stars << "," << *type << ")";
}
Type* ArrayType::getType() { return this->type; }
unsigned ArrayType::getStars() { return stars; }


RefType::RefType(Type *t): type(t) { this->classType = REF; }
RefType::~RefType() { delete type; }
void RefType::printOn(std::ostream &out) const {
  out << "RefType(" << *type << ")";
}
Type* RefType::getType() { return this->type; }
