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
  electron=false;
  electron_delay_time=-100000.;
  electronDetectorID = -1;
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

bool TMuonHit::HasElectronCoincidence()
{
  if(!HasElectron()) return false;
  if( electron_delay_time < 10. && electron_delay_time > -10.) return true;
  else return false;
}

bool TMuonHit::HasDelayedElectron(double lowT, double highT)
{
  if(electron_delay_time > lowT && electron_delay_time < highT) return true;
  else return false;
  
}

void TMuonHit::SetElectronDelayTime(double value)
{
  SetElectron(true);
  electron_delay_time = value; 
}
