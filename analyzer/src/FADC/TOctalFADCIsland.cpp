#include "TOctalFADCIsland.h"
#include <algorithm>
#include <iostream>
#include <cmath>
#include <math.h>
#include <TH1I.h>
#include <TF1.h>


using std::cout; using std::cerr; using std::endl;
using std::vector;

TOctalFADCIsland::TOctalFADCIsland()
 : fTime(0)
{}

int TOctalFADCIsland::GetMax(int first, int last) const
{
  if(last < first || last >= fData.size() || first < 0){
    cerr << "TOctalFADCIsland::GetMax() : Error! arguments out of range." << endl;
  }
  // max_element returns an iterator to the maximum element in the range.
  return *(std::max_element(fData.begin() + first, fData.begin() + last + 1));
}

int TOctalFADCIsland::GetMaxBin(int first, int last) const
{
  if(last < first || last >= fData.size() || first < 0)
  {
    cout << "first = " << first << "   last : " << last << "   size : " << fData.size() << endl;
    cerr << "TOctalFADCIsland::GetMaxBin() : Error! arguments out of range." << endl;
  }
  return std::max_element(fData.begin()+first,fData.begin()+last+1)
            - fData.begin();
}

/** Give the average value between two boundaries. 
  * It is the user responsibility to make sure that "first" and 
  * "last" are ok. Use GetNSamples for that. The function will 
  * return -1 for arguments out of range.
  */
double TOctalFADCIsland::GetAverageMax(int nAv, int first, int last) const
{
  if(last < first || last >= fData.size() || first < 0 || nAv < 0){
    cerr << "TOctalFADCIsland::GetAverageMax() : Error! arguments out of range."
         << endl;
  }

  int maxbin = GetMaxBin(first,last);
  int sum = *(std::max_element(fData.begin()+first,fData.begin()+last+1));
  int skipped = 0;
  int ind;

  for(int i=1; i<=nAv; i++)
  {
    ind = maxbin + static_cast<int>((i/2.+ 0.5)*pow(-1,i));
    if(ind > first && ind < last){sum = sum + fData[ind];}
    else{skipped++;}
  }

  return static_cast<double>(sum)/(nAv+1.-skipped);
}

int TOctalFADCIsland::GetMaxBinTime(int first, int last) const
{
  if(last < first || last >= fData.size() || first < 0){
    cerr << "TOctalFADCIsland::GetMaxBinTime() : Error! arguments out of range." 
         << endl;
  }
  return GetTime() + GetMaxBin(first,last);
}

int TOctalFADCIsland::GetCFBlockTime(int first, int last) const
{
  if(last < first || last >= fData.size() || first < 0){
    cerr << "TOctalFADCIsland::GetCFBlockTime() : Error! arguments out of range."
         << endl;
  }
  return GetCFBin(first,last)+GetTime(); 
}

/** Constant fraction discrimination. This returns the sample where
  * the pulse first crosses half-maximum.
  */
int TOctalFADCIsland::GetCFBin(int first, int last) const
{
  if(last < first || last >= fData.size() || first < 0){
    cerr << "TOctalFADCIsland::GetCFBlockTime() : Error! arguments out of range." 
         << endl;
  }

  // Take the non-zero pedestal into account
  int max = GetMax(first,last) - fData[first];
  int maxbin = GetMaxBin(first,last);
  double halfmax = max/2.;
  int constantFraction = first;
  double value = -1.;

  while( value < halfmax && constantFraction < maxbin)
  {
    value = static_cast<double>(fData[constantFraction]);
    constantFraction++;
  }

  return constantFraction;
}

double TOctalFADCIsland::GetAverage(int first, int last) const
{
  if(last < first ){return -1.;}
  if(first<0){first=0;}
  if(last >= fData.size()){last = fData.size();}

  int sum = 0;
  int N = 0;

  for(int i = first; i <= last; i++)
  {
    sum += fData[i];
    N += 1;
  }

  return static_cast<double>(sum)/N;
}

double TOctalFADCIsland::GetIntegral(int first, int last) const
{
  if(last < first ){return -1.;}
  if(first<0) { first=0; }
  if(last >= fData.size()) { last = fData.size() - 1; }

  int sum = 0;

  for(int i = first; i <= last; i++)
  {
    sum += fData[i];
  }

  return static_cast<double>(sum);
}
