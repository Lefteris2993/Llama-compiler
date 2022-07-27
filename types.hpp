#ifndef __TYPES_HPP__
#define __TYPES_HPP__

#include <iostream>

enum BaseType { UNIT, INT, CHAR, BOOL };

enum TypeClassType { SIMPLE, FUNCTION, ARRAY, REF };

class Type {
public:
  virtual ~Type();
  virtual void printOn(std::ostream &out) const = 0;

  static bool equal_types(const Type *t1, const Type *t2);

  TypeClassType getClassType();

protected:
  TypeClassType classType;
};

inline std::ostream& operator<< (std::ostream &out, const Type &t) {
  t.printOn(out);
  return out;
}

class SimpleType: public Type {
public:
  SimpleType(BaseType t);
  BaseType getType();
  virtual void printOn(std::ostream &out) const override;
private:
  BaseType type;
};

class FunctionType: public Type {
public:
  FunctionType(Type *l, Type *r);
  ~FunctionType();
  virtual void printOn(std::ostream &out) const override;
  Type *getLeft();
  Type *getRight();
private:
  Type *left;
  Type *right;
};

class ArrayType: public Type {
public:
  ArrayType(Type *t, unsigned s = 1);
  ~ArrayType();
  virtual void printOn(std::ostream &out) const override;
  Type *getType();
  unsigned getStars();
private:
  unsigned stars;
  Type *type;
};

class RefType: public Type {
public:
  RefType(Type *t);
  ~RefType();
  virtual void printOn(std::ostream &out) const override;
  Type *getType();
private:
  Type *type;
};

extern Type *intType;
extern Type *unitType;
extern Type *charType;
extern Type *boolType;
extern Type *stringType;

#endif
