#ifndef __BoolPattern_HPP__
#define __BoolPattern_HPP__


#include <iostream>
#include "../Pattern/Pattern.hpp"

class BoolPattern: public Pattern {
public:
  BoolPattern(bool v);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;

private:
  bool val;
};

#endif