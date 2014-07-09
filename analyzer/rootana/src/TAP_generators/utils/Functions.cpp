#include "Functions.h"

#include <TMath.h>

double functions::gauss_lin(double *x, double *par) {
    // parameters:
    // 0: constant (pedestal)
    // 1: gradient
    // 2: Gauss amplitude
    // 3: Gauss mean
    // 4: Gauss width
    Double_t arg=(x[0]-par[3])/par[4];
    Double_t gauss1=par[2]*TMath::Exp(-0.5*arg*arg)/(2.5066283*par[4]);
    // function=gauss+constant background
    return gauss1+par[0] + par[1]*x[0];
}

