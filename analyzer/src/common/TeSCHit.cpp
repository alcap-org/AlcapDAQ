#include "TeSCHit.h"

#include <cmath>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <numeric>

#include "TF1.h"

using std::vector;
using std::string;


ClassImp(TeSCHit)

TeSCHit::TeSCHit() 
{
}


TeSCHit::TeSCHit(int ch)
{
   channel = ch;
   lowTime = -1;
   isLow = false; 
   highTime = -1;
   isHigh = false; 
}

TeSCHit::~TeSCHit()
{

}

double TeSCHit::GetTime()
{
  if(isHigh) return highTime;
  if(isLow) return lowTime;
  else return 0.;
  
}
