#ifndef TEMPLATE_FITTING_QUADRATICFIT_H
#define TEMPLATE_FITTING_QUADRATICFIT_H

#include <iterator>

namespace functions{

class QuadraticFit{

  public:
  QuadraticFit(int length):fN(length),fX2(0),fX4(0){
   for(int i= -(length-1)/2; i < (length+1)/2; ++i){
     fX2+=i*i;
     fX4+=i*i*i*i;
   }
   fDet=fX4*fN*fX2 - fX2*fX2*fX2;
  }

  template <typename InputIterator>
  void Fit(InputIterator i_in, double& a, double& b, double& c){
   double Sy=0, Sxy=0, Sx2y=0, y=0;
   for(int x= -(fN)/2; x < (fN+1)/2; ++x){
     y=*i_in;
     Sy+=y;
     Sxy+=x*y;
     Sx2y+=x*x*y;
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
