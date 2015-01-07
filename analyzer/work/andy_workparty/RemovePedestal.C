// RemovePedestal.C
// -- Removes the pedestal from a histogram by taking the mean of the first 5 bins
// -- Returns the pedestal subtracted histogram

#include <iostream>
#include <sstream>

#include "TH1.h"

TH1* RemovePedestal(TH1* hist) {

  std::stringstream pedSubHistname;
  pedSubHistname << "PedSub_" << hist->GetName();

  double low_val = hist->GetBinLowEdge(1);
  double high_val = hist->GetBinLowEdge(hist->GetXaxis()->GetNbins() + 1);

  // Create the pedestal subtracted histogram
  TH1F* pedSub = new TH1F(pedSubHistname.str().c_str(), pedSubHistname.str().c_str(), 100,low_val,high_val);
  pedSub->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
  pedSub->GetYaxis()->SetTitle(hist->GetYaxis()->GetTitle());  

  // Loop through the first few bins and take the mean value for the pedestal
  double pedestal = 0.0;
  int nBinsForMean = 5;
  for (int iBin = 1; iBin <= nBinsForMean; iBin++) {
    pedestal += hist->GetBinContent(iBin);
  }
  pedestal /= nBinsForMean;
  //  std::cout << "Pedestal: " << pedestal << std::endl;

  // Loop through the bins (NB bin numbering starts at 1!)
  for (int iBin = 1; iBin <= hist->GetXaxis()->GetNbins(); iBin++) {
    double binValue = hist->GetBinContent(iBin);
    float binCenter = hist->GetBinCenter(iBin);

    if (binValue != 0) { // make sure the bin's not empty
      pedSub->Fill(binCenter, binValue - pedestal);
      //      std::cout << "Bin Value (before) = " << binValue;
      //      std::cout << ", Bin Value (after) = " << binValue - pedestal << std::endl;
    }
  }

  return pedSub;
}
