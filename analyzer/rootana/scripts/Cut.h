#include "TF1.h"

struct Cut {
  Cut(TF1* f, bool b) : eqn(f), remove_below(b) {}
  TF1* eqn;
  bool remove_below; // true if we remove entries below this line, false if we remove entries above this line
};
