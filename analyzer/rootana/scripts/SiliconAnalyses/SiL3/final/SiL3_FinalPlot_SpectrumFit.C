//#include "../../Utils/IntegrateRates.C"

void SiL3_FinalPlot_SpectrumFit(std::string savedir = "", std::ostream& numbers_file = std::cout) {

  //  gStyle->SetOptFit(1);
  
  std::string filename = "~/data/results/SiL3/systematics_geq2TgtPulse_newPP20us.root";
  TFile* file = new TFile(filename.c_str(), "READ");


  const int n_slices = 1;
  double min_time_slices[n_slices] = {2000};
  double max_time_slices[n_slices] = {4000};
  Int_t colours[n_slices] = {kRed};
  std::string leglabels[n_slices] = {"SiL3 (active target)"};
  
  TLegend* leg = new TLegend(0.25,0.65,0.65,0.85);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);
  
  int rebin_factor = 1;
  std::stringstream time_slice_str, leglabel;
  numbers_file << "% SiL3_FinalPlot_SpectrumFit.C" << std::endl;
  for (int i_slice = 0; i_slice < n_slices; ++i_slice) {
    double i_min_time_slice = min_time_slices[i_slice];
    double i_max_time_slice = max_time_slices[i_slice];

    time_slice_str.str("");
    time_slice_str << "TimeSlice" << i_min_time_slice << "_" << i_max_time_slice;

    Int_t i_colour = colours[i_slice];

    TCanvas* c1 = new TCanvas("c1", "c1");
    c1->SetLogy();
      
    std::string i_dirname = "FinalSystPlot_" + time_slice_str.str() + "_allRecoil_200keVBins";
    std::string i_statsystname = i_dirname + "/hFinalStatSyst";
    TGraphAsymmErrors* statsyst_spectrum = (TGraphAsymmErrors*) file->Get(i_statsystname.c_str());
    if (!statsyst_spectrum) {
      std::cout << "Error: Problem getting statsyst_spectrum " << i_statsystname << std::endl;
      return;
    }
      
    statsyst_spectrum->SetFillColor(kBlue);
    statsyst_spectrum->SetLineColor(kBlue);
    statsyst_spectrum->SetLineWidth(2);

    statsyst_spectrum->Draw("APE");
    statsyst_spectrum->GetXaxis()->SetRangeUser(0, 26000);
    statsyst_spectrum->GetYaxis()->SetRangeUser(1e-8, 1e-3);

    //    leg->AddEntry(statsyst_spectrum, leglabel.str().c_str(), "l");

    // TF1* expo_fit = new TF1("expo_fit","[0]*(exp(-x/[1]))",5000,26000);//27000);
    // expo_fit->SetParName(0, "N");
    // expo_fit->SetParName(1, "T_{0}");
    // expo_fit->SetParameter(0, 1e-6);
    // expo_fit->SetParLimits(0, 1e-7, 1e-4);
    // expo_fit->SetParameter(1, 4600);
    // expo_fit->SetParLimits(1, 2000, 6000);
    // statsyst_spectrum->Fit(expo_fit, "RME");

    // expo_fit->SetLineColor(kRed);
    // expo_fit->Draw("LSAME");

    TF1* spectral_fit = new TF1("spectral_fit","[0]*(1 - [1]/x)^[2]*(exp(-x/[3]))",1500,20000);//27000);
    //    TF1* spectral_fit = new TF1("spectral_fit","[0]*(1 - [1]/x)^[2]*(exp(-x/[3]))",1500,8000);//27000);
    // //    TF1* spectral_fit = new TF1("spectral_fit","[0]*(1 - [1]/x)^[2]*(exp(-x/[3])+[4]*exp(-x/[5]))",1500,27000);
    spectral_fit->SetParName(0, "N");
    spectral_fit->SetParName(1, "T_{th}");
    spectral_fit->SetParName(2, "#alpha");
    spectral_fit->SetParName(3, "T_{0}");
    spectral_fit->SetParameter(0, 1e-4);
    spectral_fit->SetParameter(1, 1500);
    spectral_fit->SetParLimits(1, 0, 5000);
    spectral_fit->SetParameter(2, 25);
    spectral_fit->SetParLimits(2, 0, 100);
    spectral_fit->SetParameter(3, 4000);
    spectral_fit->SetParLimits(3, 1000, 10000);
    statsyst_spectrum->Fit(spectral_fit, "RM");

    double start = 1000;
    double end = 25000;
    double point_step = 500;
    int n_points = (end - start) / point_step;
    TGraphErrors* fit_interval = new TGraphErrors(n_points);
    fit_interval->SetFillColor(kRed-9);
    fit_interval->SetFillStyle(3001);
    for (int i_point = 0; i_point < n_points; ++i_point) {
      fit_interval->SetPoint(i_point, start + i_point*point_step, 0);
    }
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(fit_interval, 0.95);//0.68);
    fit_interval->Draw("4 SAME");

    spectral_fit->SetLineColor(kRed);
    spectral_fit->Draw("LSAME");

    TF1* spectral_fit_dblexp = new TF1("spectral_fit_dblexp","[0]*(1 - [1]/x)^[2]*(exp(-x/[3])+[4]*exp(-x/[5]))",1500,20000);
    spectral_fit_dblexp->SetParName(0, "N");
    spectral_fit_dblexp->SetParName(1, "T_{th}");
    spectral_fit_dblexp->SetParName(2, "#alpha");
    spectral_fit_dblexp->SetParName(3, "T_{0}");
    spectral_fit_dblexp->SetParameter(0, 1e-4);
    spectral_fit_dblexp->FixParameter(1, spectral_fit->GetParameter(1));
    //    spectral_fit_dblexp->SetParLimits(1, 0, 5000);
    spectral_fit_dblexp->FixParameter(2, spectral_fit->GetParameter(2));
    //    spectral_fit_dblexp->SetParLimits(2, 0, 100);
    spectral_fit_dblexp->SetParameter(3, 4000);
    spectral_fit_dblexp->FixParameter(3, spectral_fit->GetParameter(3));
    //    spectral_fit_dblexp->SetParLimits(3, 1000, 20000);
    spectral_fit_dblexp->SetParName(4, "r");
    spectral_fit_dblexp->SetParName(5, "T_{1}");
    spectral_fit_dblexp->SetParameter(4, 10);
    spectral_fit_dblexp->SetParLimits(4, 0, 100);
    spectral_fit_dblexp->SetParameter(5, 8000);
    spectral_fit_dblexp->SetParLimits(5, 1000, 10000);
    statsyst_spectrum->Fit(spectral_fit_dblexp, "RM");

    TGraphErrors* fit_dblexp_interval = new TGraphErrors(n_points);
    fit_dblexp_interval->SetFillColor(kSpring-9);
    fit_dblexp_interval->SetFillStyle(3001);
    for (int i_point = 0; i_point < n_points; ++i_point) {
      fit_dblexp_interval->SetPoint(i_point, start + i_point*point_step, 0);
    }
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(fit_dblexp_interval, 0.95);//0.68);
    fit_dblexp_interval->Draw("4 SAME");

    spectral_fit_dblexp->SetLineColor(kSpring);
    spectral_fit_dblexp->Draw("LSAME");


    // TF1* spectral_fit = new TF1("spectral_fit","[0]*(1 - [1]/x)^[2]*(exp(-x/[3]))+[4]*(1 - [5]/x)^[6]*(exp(-x/[7]))",1500,10000);//27000);
    // spectral_fit->SetParName(0, "N_{0}");
    // spectral_fit->SetParName(1, "T_{th}");
    // spectral_fit->SetParName(2, "#alpha");
    // spectral_fit->SetParName(3, "T_{0}");
    // spectral_fit->SetParameter(0, 1e-4);
    // spectral_fit->SetParameter(1, 1500);
    // spectral_fit->SetParLimits(1, 0, 5000);
    // spectral_fit->SetParameter(2, 25);
    // spectral_fit->SetParLimits(2, 0, 100);
    // spectral_fit->SetParameter(3, 4000);
    // spectral_fit->SetParLimits(3, 1000, 10000);
    // spectral_fit->SetParName(4, "N_{1}");
    // spectral_fit->SetParName(5, "T_{th,1}");
    // spectral_fit->SetParName(6, "#alpha, 1");
    // spectral_fit->SetParName(7, "T_{1}");
    // spectral_fit->SetParameter(4, 1e-4);
    // spectral_fit->SetParLimits(4, 1e-8, 1e-3);
    // spectral_fit->SetParameter(5, 1500);
    // spectral_fit->SetParLimits(5, 0, 5000);
    // spectral_fit->SetParameter(6, 25);
    // spectral_fit->SetParLimits(6, 0, 100);
    // spectral_fit->SetParameter(7, 4000);
    // spectral_fit->SetParLimits(7, 1000, 10000);
    // statsyst_spectrum->Fit(spectral_fit, "RM");


    // TF1* spectral_fit_dblexp = new TF1("spectral_fit_dblexp","[0]*(1 - [1]/x)^[2]*(exp(-x/[3])+[4]*exp(-x/[5]))",1500,26000);
    // spectral_fit_dblexp->SetParName(0, "N");
    // spectral_fit_dblexp->SetParName(1, "T_{th}");
    // spectral_fit_dblexp->SetParName(2, "#alpha");
    // spectral_fit_dblexp->SetParName(3, "T_{0}");
    // spectral_fit_dblexp->SetParameter(0, 1e-5);
    // spectral_fit_dblexp->FixParameter(1, spectral_fit->GetParameter(1));
    // //    spectral_fit_dblexp->SetParameter(1, 1500);
    // //    spectral_fit_dblexp->SetParLimits(1, 0, 5000);
    // //    spectral_fit_dblexp->SetParameter(2, 0.5);
    // spectral_fit_dblexp->FixParameter(2, spectral_fit->GetParameter(2));
    // //    spectral_fit_dblexp->SetParLimits(2, 0, 10);
    // //    spectral_fit_dblexp->SetParameter(3, 4000);
    // spectral_fit_dblexp->FixParameter(3, spectral_fit->GetParameter(3));
    // //    spectral_fit_dblexp->SetParLimits(3, 1000, 10000);
    // spectral_fit_dblexp->SetParName(4, "r");
    // spectral_fit_dblexp->SetParName(5, "T_{1}");
    // spectral_fit_dblexp->SetParameter(4, 0.5);
    // spectral_fit_dblexp->SetParLimits(4, 0, 100);
    // spectral_fit_dblexp->SetParameter(5, 6500);
    // spectral_fit_dblexp->SetParLimits(5, 0, 10000);
    // statsyst_spectrum->Fit(spectral_fit_dblexp, "RM");

    // spectral_fit_dblexp->SetLineColor(kMagenta);
    // spectral_fit_dblexp->Draw("LSAME");

    // TF1* dblexp_only = new TF1("dblexp_only","[0]*((exp(-x/[1])+[2]*exp(-x/[3])))",5000,26000);
    // dblexp_only->SetParName(0, "N");
    // dblexp_only->SetParName(1, "T_{0}");
    // dblexp_only->SetParameter(0, 1e-5);
    // dblexp_only->SetParameter(1, 4000);
    // dblexp_only->SetParLimits(1, 1000, 10000);
    // dblexp_only->SetParName(2, "r");
    // dblexp_only->SetParName(3, "T_{1}");
    // dblexp_only->SetParameter(2, 0.5);
    // dblexp_only->SetParLimits(2, 0, 100);
    // dblexp_only->SetParameter(3, 6500);
    // dblexp_only->SetParLimits(3, 0, 10000);
    // statsyst_spectrum->Fit(dblexp_only, "RM");

    // dblexp_only->SetLineColor(kMagenta);
    // dblexp_only->Draw("LSAME");

    // TF1* quadexp_only = new TF1("quadexp_only","[0]*(1 - [1]/x)^[2]*([3]*(exp(-x/[4])+[5]*exp(-x/[6])+[7]*exp(-x/[8])+[9]*exp(-x/[10])))",1500,20000);
    // quadexp_only->SetParName(0, "N");
    // quadexp_only->SetParameter(0, 1e-5);
    // quadexp_only->SetParName(1, "T_{th}");
    // quadexp_only->SetParName(2, "#alpha");
    // quadexp_only->SetParameter(1, 1500);
    // quadexp_only->SetParLimits(1, 0, 5000);
    // quadexp_only->SetParameter(2, 0.5);
    // quadexp_only->SetParLimits(2, 0, 10);
    // quadexp_only->SetParName(3, "a_{0}");
    // quadexp_only->SetParameter(3, 0.5);
    // quadexp_only->SetParLimits(3, 0.0, 10);
    // quadexp_only->SetParName(4, "T_{0}");
    // quadexp_only->SetParameter(4, 3500);
    // quadexp_only->SetParLimits(4, 3400, 3600);
    // quadexp_only->SetParName(5, "a_{1}");
    // quadexp_only->SetParameter(5, 0.1);
    // quadexp_only->SetParLimits(5, 0.0, 10);
    // quadexp_only->SetParName(6, "T_{1}");
    // quadexp_only->SetParameter(6, 7600);
    // quadexp_only->SetParLimits(6, 6700, 10000);
    // quadexp_only->SetParName(7, "a_{2}");
    // quadexp_only->SetParameter(7, 0.01);
    // quadexp_only->SetParLimits(7, 0.0, 10);
    // quadexp_only->SetParName(8, "T_{2}");
    // quadexp_only->SetParameter(8, 4100);
    // quadexp_only->SetParLimits(8, 3500, 4700);
    // quadexp_only->SetParName(9, "a_{3}");
    // quadexp_only->SetParameter(9, 0.4);
    // quadexp_only->SetParLimits(9, 0.0, 10);
    // quadexp_only->SetParName(10, "T_{3}");
    // quadexp_only->SetParameter(10, 2700);
    // quadexp_only->SetParLimits(10, 2200, 3200);
    // statsyst_spectrum->Fit(quadexp_only, "R");

    // quadexp_only->SetLineColor(kMagenta);
    // quadexp_only->Draw("LSAME");

    //    leg->Draw();

    if (savedir != "") {
      std::string savename = savedir + "AlCapData_SiL3Dataset_SpectrumFit";
      
      std::string pdfname = savename + ".pdf";
      c1->SaveAs(pdfname.c_str());
      std::string pngname = savename + ".png";
      c1->SaveAs(pngname.c_str());
    }
  }

  numbers_file << std::endl;
}
