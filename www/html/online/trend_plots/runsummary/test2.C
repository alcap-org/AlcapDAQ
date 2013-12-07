#include<fstream>
#include<iostream>
#include<string>
#include "TBox.h"
#include "THStack.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TColor.h"
#include "../../common/mysql.h"
#include "../../common/init.C"
using namespace std;

void test2()
{
  Double_t hmax[3] = {14., 15., 16.};
  Double_t fmax;
  cout << hmax[0] << hmax[1] << hmax[2] << endl;
  if (hmax[0] > hmax[1] &&  hmax[0] > hmax[2])
    fmax = hmax[0];
  else if (hmax[1] > hmax[0] &&  hmax[1] > hmax[2])
    fmax = hmax[1];
  else 
    fmax = hmax[2];

  cout << fmax << endl;
 
  TH1D * pretty = new TH1D("pretty", "pretty", 17, 0, 17);
  pretty->SetFillColor(1);
  for (int i=1; i<18; i+= 4)
    pretty->SetBinContent(i, fmax*(14/12));
  pretty->Draw();

}
