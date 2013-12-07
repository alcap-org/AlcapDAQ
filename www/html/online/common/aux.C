
#include "TGraph.h"

void get_min_max(TGraph *gr, Double_t &xmin, Double_t &xmax, Double_t &ymin, Double_t &ymax)
{

  xmin = ymin =  1.0e32;
  xmax = ymax = -1.0e32;

  for (int i=0; i<gr->GetN(); i++) {
    Double_t x, y;
    gr->GetPoint(i, x, y);

    if ( x < xmin ) {
      xmin = x;
    }

    if ( x > xmax ) {
      xmax = x;
    }

    if ( y < ymin ) {
      ymin = y;
    }

    if ( y > ymax ) {
      ymax = y;
    }

    //printf("x %f, y %f, xmin %f, xmax %f, ymin %f, ymax %f\n",x,y,xmin,xmax,ymin,ymax);

  }

  if ( xmax < xmin ) {
    xmin = 0;
    xmax = 1;
    ymin = 0;
    ymax = 1;
  }

}
