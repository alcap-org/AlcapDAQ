#include "Run.h"
#include "Source.h"
#include "Peaks.h"

#include "TTimeStamp.h"
#include "TH1.h"
#include "TFile.h"

#include <iostream>
#include <sstream>
#include <stdio.h>

Peaks initialize_peaks(Run run, bool gehi) {
  const Double_t *Es, *Is, *ADCs;
  const int n = run.Spectrum(Es, Is, ADCs);
  Double_t ADCs_Hi[n];
  for (int i = 0; i < n; ++i) ADCs_Hi[i] = run.HiScaleEstimate()*ADCs[i];
  return gehi ? Peaks(n, Es, Is, ADCs_Hi) : Peaks(n, Es, Is, ADCs);
}

Run SelectRun(int run) {
  Source eu152(TTimeStamp(2000, 3, 1, 0, 0, 0), 47.8e3, Source::Eu152);
  Source co60 (TTimeStamp(2000, 4, 1, 0, 0, 0), 34.e3,  Source::Co60);
  Source y88  (TTimeStamp(2014, 3, 1, 0, 0, 0), 855.5e+3, Source::Y88); // MIGHT BE WRONG ACTIVITY
  switch(run) {
  case 8304:
    return Run(8304, 2931,   TTimeStamp(2015, 11, 9, 0, 0, 0),
	       eu152, false, 1.);
  case 8310:
    return Run(8310, 23918,  TTimeStamp(2015, 11, 9, 0, 0, 0),
	       eu152, true,  2.24);
  case 8367:
    return Run(8367, 3800,   TTimeStamp(2015, 11, 10, 0, 0, 0),
	       eu152, false, 2.16);
  case 8374: // Ge face
    return Run(8374, 2139,   TTimeStamp(2015, 11, 10, 0, 0, 0),
	       eu152, false, 2.16);
  case 8377: // Ge face
    return Run(8377, 5111,   TTimeStamp(2015, 11, 10, 0, 0, 0),
	       eu152, false, 2.16);
  case 9282: // Ge face
    return Run(9282, 4801,   TTimeStamp(2015, 11, 15, 0, 0, 0),
	       eu152, false, 2.15);
  case 9284: // Ge face
    return Run(9284, 4571,   TTimeStamp(2015, 11, 15, 0, 0, 0),
	       co60,  false, 2.14);
  case 9302: // Rough vacuum, first Al position
    return Run(9302, 15102,  TTimeStamp(2015, 11, 16, 0, 0, 0),
	       eu152, true,  2.15);
  case 9684: // Ge face, prefill
    return Run(9684, 4106,   TTimeStamp(2015, 11, 18, 0, 0, 0),
	       eu152, false, 2.15);
  case 9685: // Back Al100, prefill
    return Run(9685, 15533,  TTimeStamp(2015, 11, 18, 0, 0, 0),
	       eu152, true,  2.15);
  case 9686: // Ge face, postfill
    return Run(9686, 9576,   TTimeStamp(2015, 11, 18, 0, 0, 0),
	       eu152, false, 2.15);
  case 10319: // In empty target, rough vacuum
    return Run(10319, 30303, TTimeStamp(2015, 11, 22, 0, 0, 0),
	       eu152, true,  2.15);
  case 10320: // Empty target position, atmospheric, removed prematurely
    return Run(10320, 6353,  TTimeStamp(2015, 11, 22, 0, 0, 0),
	       y88,   true, 2.16);
  case 10321: // Ge face
    return Run(10321, 27661, TTimeStamp(2015, 11, 22, 0, 0, 0),
	       y88,   false, 2.16);
  case 10322: // Ge face
    return Run(10322, 7823,  TTimeStamp(2015, 11, 25, 0, 0, 0),
	       y88,   false, 2.16);
 }
  throw "Error: Invalid run";
}


void R15b_ge_calib(const char* fname, int rnum, bool gehi = false) {
  TFile* f = new TFile(fname, "READ");
  std::ostringstream filename;
  gehi ? filename << "GeHiGain_" : filename << "GeLoGain_";
  filename << rnum;
  filename << ".root";
  TFile* output = new TFile(filename.str().c_str(), "RECREATE");
  std::ostringstream os;
  gehi ? os << "GeHiGain_" : os << "GeLoGain_";
  os << rnum;
  TDirectory *directory = output->mkdir(os.str().c_str() );
  directory->cd();
  TH1* spec = gehi ?
//    (TH1*)f->Get("PlotTAP_Amplitude/hGeHiGain#MaxBinAPGenerator#any_Amplitude"):
//    (TH1*)f->Get("PlotTAP_Amplitude/hGeLoGain#MaxBinAPGenerator#any_Amplitude");
    (TH1*)f->Get("PlotTAP_Amplitude/hGeHiGain#MaxBinAPGenerator#{no_time_shift=true}_Amplitude") :
    (TH1*)f->Get("PlotTAP_Amplitude/hGeLoGain#MaxBinAPGenerator#{no_time_shift=true}_Amplitude");
  if (!spec) {
    std::cout << "No spectrum found!" << std::endl;
    return;
  }
  char str[128];
  sprintf(str, "Run %d Ge%s Spectrum;Energy (ADC)", rnum, gehi ? "Hi" : "Lo");
  spec->SetTitle(str);
  spec->Sumw2();

  Run run = SelectRun(rnum);
  Peaks peaks = initialize_peaks(run, gehi);
  peaks.EstimateParameters(spec);
  peaks.Fit(spec, true);
  peaks.Calibrate(run, true);
  peaks.Save();
  output->Write();
}
