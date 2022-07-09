#ifndef __AST_HPP__
#define __AST_HPP__

#include <iostream>
#include <vector>
#include <string>


class Program {
public:
  virtual ~Program() {}
  virtual void printOn(std::ostream &out) const = 0;
};

inline std::ostream& operator<< (std::ostream &out, const AST &t) {
  t.printOn(out);
  return out;
}

class Statement : public Program {
  public:
    virtual void run() const = 0;
};

class StatementBlock : public Statement {
  public:
    StatementBlock(): stmt_list() {}
    ~StatementBlock(): { for (Statement *s : stmt_list) delete s; }
    virtual void run() const override {
      // implement
    }
    virtual void printOn(std::ostream &out) const override {
      // implement
    }
  
  private:
    std::vector<Statement *> stmt_list;
}

class Parametre : public Statement {
  public:
    
  private:
    std::string id;
    Type type;
}

class Definition : public Statement {
  public:

  private:

}

class LetDefinition : public Statement {
  public:

  private:
    bool rec;

}

#endif
