#ifndef BANKBRANCH_H
#define BANKBRANCH_H

//C++/STL

//ROOT
#include "TNamed.h"

//Local


template <typename BANK>
class BankBranch : public TNamed 
{
public:
  BankBranch()
    : TNamed("unknown","Unknown Bank Branch")
  {}
  BankBranch(const char* name)
    : TNamed(name, (TString(name)+"Bank Branch").Data())
  {}
  
  virtual ~BankBranch();

private:
  BANK* fBank;

  ClassDef(BankBranch, 1);
};

#endif //BANKBRANCH_H
