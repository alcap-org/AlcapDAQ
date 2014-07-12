#ifndef BANKBRANCH_H
#define BANKBRANCH_H

//C++/STL

//ROOT
#include "TNamed.h"

//Local
#include "IdGenerator.h"

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
  
  BankBranch(const IDs::generator& gid)
    : TNamed(gid.str(), gid.str())
    ,  fGenerator(gid)
  {
    fBank = 0x0;
  }

  virtual ~BankBranch();

private:
  BANK* fBank;
  IDs::generator fGenerator;

  ClassDef(BankBranch, 1);
};

#endif //BANKBRANCH_H
