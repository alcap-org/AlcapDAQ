#include "TOctalFADCIsland.h"
#include "Parameters.h"
#include <algorithm>
#include <iostream>
#include <cmath>
#include <math.h>
#include <TH1I.h>
#include <TF1.h>

extern OCTALFADC_PARAM octalfadc_parameters;

using std::cout; using std::cerr; using std::endl;
using std::vector;

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
  //cout << "(good one) first = " << first << "   last : " << last << "   size : " << fData.size() << endl;
  // vector iterators obey pointer arithmetic - the subtraction gives the index
  return std::max_element(fData.begin()+first,fData.begin()+last+1) 
            - fData.begin();
 
}


double TOctalFADCIsland::GetAverageMax(int nAv, int first, int last) const
{
  if(last < first || last >= fData.size() || first < 0 || nAv < 0){
    cerr << "TOctalFADCIsland::GetAverageMax() : Error! arguments out of range." << endl;
  }

  int maxbin = GetMaxBin(first,last);
  int sum = *(std::max_element(fData.begin()+first,fData.begin()+last+1));
  int skipped = 0;
  int ind;

  for(int i  = 1; i <= nAv; i++)
  {
    ind = maxbin + static_cast<int>((i/2.+ 0.5)*pow(-1,i));
    if(ind > first && ind < last){sum = sum + fData[ind];}
    else{skipped++;}     
  }

    return static_cast<double>(sum)/(nAv+1.-skipped);
}



int TOctalFADCIsland::GetMaxBinBlockTime(int first, int last) const
{
  if(last < first || last >= fData.size() || first < 0){
    cerr << "TOctalFADCIsland::GetMaxBinBlockTime() : Error! arguments out of range." << endl;
  }
  return GetTime() + GetMaxBin(first,last); 
}

int TOctalFADCIsland::GetCFBlockTime(int first, int last) const // 
{
  if(last < first || last >= fData.size() || first < 0){
    cerr << "TOctalFADCIsland::GetCFBlockTime() : Error! arguments out of range."
         << endl;
  }
  return GetCFBin(first,last)+GetTime(); 
}

int TOctalFADCIsland::GetCFBin(int first, int last) const //Will miss a bit if there is a overflow of the FADC. If the timing of high energy signals becomes an issue, this algorithm will have to be come a little bit more sofisticated
{
  if(last < first || last >= fData.size() || first < 0){
    cerr << "TOctalFADCIsland::GetCFBlockTime() : Error! arguments out of range." << endl;

  }

  int max = GetMax(first,last)-fData[first];//taken the non zero pedestal into account
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


double TOctalFADCIsland::GetTimeNs() const
{
  double period = octalfadc_parameters.kClockPeriod[fChannel];
  return fTime*period;
}

double TOctalFADCIsland::GetIntegral(int first, int last) const
{
  if(last < first ){return -1.;}
  if(first<0){first=0;}
  if(last >= fData.size()){last = fData.size();}

  int sum = 0;
  
  for(int i = first; i <= last; i++)
  {
    sum += fData[i];
  }

  return static_cast<double>(sum);
}



//this is optimized for a Ge detector!
bool TOctalFADCIsland::GoodPulse(int shapTime, double clockPeriod, int level) // Do not reject pulse with noisy spikes, just tag them in the hits object
{
  int secondSample = 1;
  int oneButLast = fData.size() - 1;
  int maxbin = GetMaxBin();
  int channel = GetFADCChannel();
  int threshold = octalfadc_parameters.threshold[channel];

  if(fData.size()*clockPeriod < static_cast<double>(shapTime) && level >= 1){SetPulseQuality(1);return false;} //A good pulse is longer then the shaping time. For a non-shaped PMT pulse you just can send the width,

  //This doesn`t work so well. Energy samples which are missing a piece, but have a nice maximum, are not accepted. Also good timing pulses with an undershoot don`t make it.
  //else if( abs(fData[secondSample] - fData[oneButLast] ) > 50 && level >= 2){return false;} //The first and the last few samples should be about the same value. "50" is somewhat arbitary
  
  //Alternative: asking that the maximum doesn`t set at the edge (12.5% on each side) of the island
  else if( ( static_cast<double>(maxbin) < fData.size()/12. || static_cast<double>(maxbin) > 11*fData.size()/12. ) && level >=2) { SetPulseQuality(2);return false;}

  else if( ( GetMax() < threshold && level >=3 )) {SetPulseQuality(3);return false;}

  else if( abs(*(std::max_element(fData.begin(), fData.begin() + fData.size())) - *(std::max_element(fData.begin(), fData.begin() + fData.size()) + 1)) > 50 && level >= 4){SetPulseQuality(4);return false;} //eleminating spikes, again the "50" is arbitary. Might be dangerous for very sharp PMT pulses

  else{SetPulseQuality(0);return true;}

}

// This method is customized for the run4 Ge datasets
bool TOctalFADCIsland::GoodPulse(int shapTime, double clockPeriod, int ped, int runnr, int level, int channel) 
{
  int secondSample = 1;
  int oneButLast = fData.size() - 1;
  int maxbin = GetMaxBin();
  double max = static_cast<double>(GetMax());
  double leftAv = GetAverage(maxbin-40,maxbin-30);
  double rightAv = GetAverage(maxbin+30,maxbin+40);
  double totalAv = GetAverage();
  double pedestal = static_cast<double>(ped);

  double ampCut_a = 0;
  double ampCut_b = 0;
  double avCut_a = 1.;
  double avCut_b = 0.;//"_a" is for the Drop Maximum method, "_b" is for the Island Average method

  if((runnr >= 51683) &&(runnr <= 51984))
  {
    if(channel == 1){ampCut_a = 2500; avCut_a = 0.04; ampCut_b = 2700; avCut_b = 0.75;} //channel 1 = HG, channel 2 = LG
    if(channel == 2){ampCut_a = 1380; avCut_a = 0.032; ampCut_b = 1385; avCut_b = 0.975;}
  }
  if((runnr >= 51985) &&(runnr <= 52280))
  {
    if(channel == 1){ampCut_a = 2500; avCut_a = 0.04; ampCut_b = 2500; avCut_b = 0.8;}
    if(channel == 2){ampCut_a = 1500; avCut_a = 0.022; ampCut_b = 1510; avCut_b = 0.935;}
    //if(channel == 2){ampCut_a = 1500; avCut_a = 0.022; ampCut_b = 1510; avCut_b = 0.935;}
  }

  if(fData.size()*clockPeriod < static_cast<double>(shapTime) && level >= 1){SetPulseQuality(1);return false;} //A good pulse is longer then the shaping time. For a non-shaped PMT pulse you just can send the width,

  //This doesn`t work so well. Energy samples which are missing a piece, but have a nice maximum, are not accepted. Also good timing pulses with an undershoot don`t make it.
  //else if( abs(fData[secondSample] - fData[oneButLast] ) > 50 && level >= 2){return false;} //The first and the last few samples should be about the same value. "50" is somewhat arbitary

  //Alternative: asking that the maximum doesn`t set at the edge (12.5% on each side) of the island
  else if( ( static_cast<double>(maxbin) < fData.size()/12. || static_cast<double>(maxbin) > 11*fData.size()/12. ) && level >=2){ SetPulseQuality(2);return false;}

  else if( abs(*(std::max_element(fData.begin(), fData.begin() + fData.size())) - *(std::max_element(fData.begin(), fData.begin() + fData.size()) + 1)) > 50 && level >= 3){SetPulseQuality(3);return false;} //eleminating spikes, again the "50" is arbitary. Might be dangerous for very sharp PMT pulses

  //This checks if the pulse drops fast enough next to the maximum
  else if( level >=4 && ( fabs((leftAv - max)/(max - pedestal))  < avCut_a && max > ampCut_a) ){SetPulseQuality(4);return false;}
  else if( level >=4 && ( fabs((rightAv - max)/(max - pedestal))  < avCut_a && max > ampCut_a) ){SetPulseQuality(4);return false;}

  //checks if the Average (~integral) is consistent with the amplitude 
  else if( level >=5 && ( totalAv/max > avCut_b && max > ampCut_b ) ) {SetPulseQuality(5);return false;}


  else{SetPulseQuality(0);return true;}
//printf("max = %f,avCut_a = %f, ampCut_a = %f, Figure of Merit (R) = %f\n",max ,avCut_a,ampCut_a,fabs((rightAv - max)/(max - pedestal)));
//printf("max = %f, avCut_a = %f, ampCut_a = %f, Figure of Merit (L) = %f\n",max ,avCut_a,ampCut_a,fabs((leftAv - max)/(max - pedestal)));
//printf("Max = %f, Pedestal = %f, LeftAv = %f, Figure of merit = %f \n", max,pedestal,leftAv,fabs((leftAv - max)/(max - pedestal)) );printf("Max = %f, Pedestal = %f, RightAv = %f, Figure of merit = %f \n", max,pedestal, rightAv,fabs((rightAv - max)/(max - pedestal)) );

}

int TOctalFADCIsland::FitGauss(int left, int right, int shapTime, double clockPeriod, int pedestal) 
{
  int size = fData.size();
  if( GetMaxBin() - left < 1) { return 0; }
  if(GetMaxBin() + right > size -1) { return 0; }
  
  TH1I* hIsland = new TH1I("hIsland","The OctalFADCIsland as a Histogram instead of a vector",size,0,size);
  for(int i = 0; i<size; i++) { hIsland->SetBinContent(i+1,fData[i]);}
  
  TF1* myG = new TF1("myG", "gaus(0)+[3]", GetMaxBin()-left,GetMaxBin()+right);
  myG->SetParLimits(1, left+1., right-1.);
  myG->SetParLimits(2, shapTime/(4.*clockPeriod), (shapTime*4.)/clockPeriod);
  myG->FixParameter(3, pedestal*1.);
  //myG->SetParLimits(3,octalfadc_parameters.pedestalGeHighGainchannel-500,octalfadc_parameters.pedestalGeHighGainchannel+100);

  hIsland->Fit("myG","RQ");

  double chiSqr = myG->GetChisquare();
  double maxbin = myG->GetParameter(1);
  double max = myG->GetMaximum();
  int NDF = myG->GetNDF();

  SetChiSqrRed(chiSqr/NDF);
  SetMaxBinGauss (maxbin);
  SetMaxGauss(max);

  //printf("Size = %d  Pulse Quality = %d  Chi Sqr = %.1f  NDF = %d   Fitted amp = %f  Amplitude = %d \n", size, GetPulseQuality(), myG->GetChisquare(), myG->GetNDF(),max,GetMax());

  delete hIsland;
  delete myG;

  if(chiSqr/NDF > 5.){ return 0; }

  return 1;
}


