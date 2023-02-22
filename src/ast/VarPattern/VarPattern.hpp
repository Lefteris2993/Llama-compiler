#ifndef __VarPattern_HPP__
#define __VarPattern_HPP__

#include <iostream>

#include "../Pattern/Pattern.hpp"

class VarPattern: public Pattern {
public:
  VarPattern(std::string v);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;

private:
  std::string var;
};



#endif
