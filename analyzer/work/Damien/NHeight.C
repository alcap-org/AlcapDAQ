#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <TH1.h>

void main(int runNum)
{
   char name[32];
   sprintf(name, "his0%04d.root", runNum);
   TFile file(name); //not accepting filename this way 

   if(runNum > 2900)  //double check values, need for each range channel changed
   {
      TCanvas *MyC = new TCanvas("MyC", "Neutron Detectors", 1);
      //MyC->Divide(2,2);
      
      char file1[32], file2[32];
      sprintf(file1, "hNf81_Heights");
      //sprintf(file2, "hNg81_Heights");

      TH1I f = file.Get(file1);    //get first detector
      f.Draw();    //draw to pad see if other options are needed (range, etc)
      //MyC->cd(2);
      //MyC->SetLogy(1);
      //  f.Draw();    //draw logarithmic to pad


      //TH1I g = file.Get(file2);  //need to doublecheck the histogram names
      //MyC->cd(3);
      //g.Draw();    //draw to pad
      //MyC->cd(4);
      //MyC->SetLogy(1);
      //g.Draw();    //draw to logarithmic pad

   }
}
