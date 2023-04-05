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
    Type *a1 = ((RefType *) t1)->getType();
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
  out << "SimpleType(";
  switch (type)
  {
  case BaseType::UNIT :
    out << "UNIT";
    break;
  
  case BaseType::INT :
    out << "INT";
    break;
    
  case BaseType::CHAR :
    out << "CHAR";
    break;

  case BaseType::BOOL :
    out << "BOOL";
    break;
  }
  out << ")";
}
std::string SimpleType::typeName() const {
  switch (type)
  {
  case BaseType::UNIT :
    return "UNIT";
  case BaseType::INT :
    return "INT";
  case BaseType::CHAR :
    return "CHAR";
  case BaseType::BOOL :
    return "BOOL";
  }
};
BaseType SimpleType::getType() { return this->type; }

FunctionType::FunctionType(Type *l, Type *r): left(l), right(r) { this->classType = FUNCTION; }
FunctionType::~FunctionType() { delete left; delete right; }
void FunctionType::printOn(std::ostream &out) const {
  out << "FunctionType(" << *left << "," << *right << ")";
}
std::string FunctionType::typeName() const {
  std::string l = left->typeName();
  std::string r = right->typeName();
  return "(" + l + "->" + r + ")";
}
Type* FunctionType::getLeft() { return this->left; }
Type* FunctionType::getRight() { return this->right; }


ArrayType::ArrayType(Type *t, unsigned s): stars(s) {
  type = new RefType(t);
  classType = ARRAY; 
}
ArrayType::~ArrayType() { delete type; }
void ArrayType::printOn(std::ostream &out) const {
  out << "ArrayType(" << stars << "," << *type << ")";
}
std::string ArrayType::typeName() const {
  std::string t = type->typeName();
  return "array_of_" + t + "_dim_" + std::to_string(stars);
}
Type* ArrayType::getType() { return this->type; }
unsigned ArrayType::getStars() { return stars; }


RefType::RefType(Type *t): type(t) { this->classType = REF; }
RefType::~RefType() { delete type; }
void RefType::printOn(std::ostream &out) const {
  out << "RefType(" << *type << ")";
}
std::string RefType::typeName() const {
  std::string t = type->typeName();
  return "ref_" + t;
}
Type* RefType::getType() { return this->type; }

Type *intType = new SimpleType(INT);
Type *unitType = new SimpleType(UNIT);
Type *charType = new SimpleType(CHAR);
Type *boolType = new SimpleType(BOOL);
Type *stringType = new ArrayType(charType);
