#ifndef __AST_HPP__
#define __AST_HPP__

#include <iostream>

class AST {
public:
  virtual ~AST();
  virtual void printOn(std::ostream &out) const = 0;
  virtual void sem();
  virtual void setLineno(unsigned l);

  unsigned lineno;
};

inline std::ostream& operator<< (std::ostream &out, const AST &t) {
  t.printOn(out);
  return out;
}

#endif