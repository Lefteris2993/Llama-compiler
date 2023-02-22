#ifndef __CharPattern_HPP__
#define __CharPattern_HPP__

#include <iostream>
#include "../Pattern/Pattern.hpp"

class CharPattern: public Pattern {
public:
  CharPattern(char c);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  char var;
};



#endif
