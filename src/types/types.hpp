#ifndef __TYPES_HPP__
#define __TYPES_HPP__

#include <iostream>

enum BinOp { BIN_PLUS, BIN_MINUS, STAR, DIV, MOD, STRUCT_EQ, STRUCT_NE, L, G, LE, GE, EQ, NE, AND, OR, ASS, PAR };

enum SigOp { NOT, SIG_PLUS, SIG_MINUS, EXCL };

enum DefType { DEF_MUTABLE, DEF_IMMUTABLE_FUN, DEF_IMMUTABLE_VAR, DEF_ARRAY };

enum BaseType { UNIT, INT, CHAR, BOOL };

enum TypeClassType { SIMPLE, FUNCTION, ARRAY, REF };

class Type {
public:
  virtual ~Type();
  virtual void printOn(std::ostream &out) const = 0;
  virtual std::string typeName() const = 0;

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
  virtual std::string typeName() const override;
private:
  BaseType type;
};

class FunctionType: public Type {
public:
  FunctionType(Type *l, Type *r);
  ~FunctionType();
  virtual void printOn(std::ostream &out) const override;
  virtual std::string typeName() const override;
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
  virtual std::string typeName() const override;
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
  virtual std::string typeName() const override;
  Type *getType();
  void setType(Type* t);
private:
  Type *type;
};

extern Type *intType;
extern Type *unitType;
extern Type *charType;
extern Type *boolType;
extern Type *stringType;

#endif
