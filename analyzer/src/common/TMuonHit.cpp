#include "TMuonHit.h"

#include <cmath>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <numeric>

#include "TF1.h"

using std::vector;
using std::string;


ClassImp(TMuonHit)

TMuonHit::TMuonHit() 
{
}

TMuonHit::TMuonHit(int value) 
{
  Reset();
  id = value;
}


TMuonHit::~TMuonHit()
{
}

void TMuonHit::Reset() {

  lowNotHi = true;
  muSCHiPileUp = false;
  muSCLoPileUp = false;
  muSCAPileUp = false;
  muSCHiPileUpWide = false;
  muSCLoPileUpWide = false;
  muSCAPileUpWide = false;
  bookEnd = false;
  
  time = 0.;
}

bool TMuonHit::IsPPMuon()
{
  if( IsMuon() && !muSCHiPileUp & !muSCAPileUp && !bookEnd ) return true;
  else return false;
}

bool TMuonHit::IsPPMuonWide()
{
  if( IsMuon() && !muSCHiPileUpWide & !muSCAPileUpWide && !bookEnd ) return true;
  else return false;
}

bool TMuonHit::IsPPMuonWithLo()
{
  if( IsPPMuon() && !muSCLoPileUp ) return true;
  else return false; 
}

