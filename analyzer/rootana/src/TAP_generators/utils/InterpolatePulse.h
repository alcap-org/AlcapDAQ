#ifndef UTILS_INTERPOLATEPULSE_H
#define UTILS_INTERPOLATEPULSE_H

class TPulseIsland;
#include <string>
#include <vector>
#include <TH1.h>

namespace functions{
// Creates a histogram with sub-bin resolution
TH1D* InterpolatePulse(
    const TPulseIsland* pulse, std::string histname,
     std::string histtitle, bool interpolate, int refine);

template<typename ValueType>
TH1F* VectorToHist( const std::vector<ValueType>& vect, std::string name,
     std::string title){
   const int size=vect.size();
   double labels[size];
   FillBinLabels(labels,size);
   TH1F* hist=new TH1F(name.c_str(),title.c_str(), size,0,size);
   hist->FillN(size,labels,vect.data());
   hist->SetDirectory(0);
   return hist;
}

void FillBinLabels(double* labels, int size, int start=-1, int increment=1);
}

using functions::InterpolatePulse;

#endif //UTILS_INTERPOLATEPULSE_H
