#ifndef TEMPLATE_FITTING_QUADRATICFIT_H
#define TEMPLATE_FITTING_QUADRATICFIT_H

#include <iterator>
#include "debug_tools.h"

namespace functions{

/// @brief Functor(-ish) class to calculate fit of a quadratic to N data-points
/// @author Ben Krikler
/// 
/// @details Assumes that the x values are spaced evenly and centred at 0 so that the sum over odd powers of x is 0.
/// Uses a formula derived from least squares regression 
class QuadraticFit{

  public:
  /// @brief constructor Taking the number of samples that will be fit to when using this instance 
  QuadraticFit(int length):fN(length+(length+1)%2),fX2(0),fX4(0){
   int range=(fN-1)/2;
   for(double x= -range; x <= range; ++x){
     fX2+=x*x;
     fX4+=x*x*x*x;
   }
   fDet=fX4*fN*fX2 - fX2*fX2*fX2;
  }

  /// @brief Fit a given set of samples.
  /// @details The input iterator should be the first sample to fit.  Since we
  /// assume the central sample is the origin, (which simplifies the maths
  /// considerably), the initial iterator should be set back N/2 samples from the
  /// centre of the fit, where N is the number of samples to fit. For example,
  /// to fit a peak at X using 6 samples,  the input iterator should point to X-3.
  template <typename InputIterator>
  void Fit(InputIterator i_in, double& a, double& b, double& c){
   double Sy=0, Sxy=0, Sx2y=0, y=0;
   int range=(fN-1)/2;
   for(double x= -range; x <= range; ++x){
     y=*i_in;
     Sy+=y;
     Sxy+=x*y;
     Sx2y+=x*x*y;
     //DEBUG_VALUE(x, y,Sy, Sxy, Sx2y);
     ++i_in;
   }
   double Da=Sx2y*fX2*fN  - Sy  *fX2*fX2;
   double Db=Sxy *fX4*fN  - Sxy *fX2*fX2;
   double Dc=Sy  *fX4*fX2 - Sx2y*fX2*fX2;
   a=Da/fDet;
   b=Db/fDet;
   c=Dc/fDet;
  }

  int GetSize()const{return fN;}
  private:
  int fN;
  double fDet, fX2, fX4;
};
   
} //namespace functions

#endif // TEMPLATE_FITTING_QUADRATICFIT_H
