#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TDirectory.h"
#include "TList.h"
#include "TRandom3.h"
#include "TPolyMarker.h"
#include "TLegend.h"
#include "TPaveText.h"

#include <iostream>
#include <sstream>
#include <cmath>

// This macro draws a single TPIs and its template TAPs that were exported using ExportPulse in rootana
bool DrawTPIAndTemplateTAPs_fromPulseViewer_file(TFile* file, std::string pulse_name, TCanvas& c1, std::string histtitle) {

  int n_pulses_to_draw = 1;
  //  c1.Divide(2);
  /*
  TPad *pad_w_errors = new TPad("pad_w_errors","This is pad1",0.02,0.29,0.5,0.97);
  TPad *pad_wout_errors = new TPad("pad_wout_errors","This is pad1",0.5,0.29,0.98,0.97);
  TPad *pad_w_errors_residual = new TPad("pad_w_errors_residual","This is pad2",0.02,0.02,0.5,0.27);
  TPad *pad_wout_errors_residual = new TPad("pad_wout_errors_residual","This is pad2",0.5,0.02,0.98,0.27);
  pad_w_errors->Draw();
  pad_wout_errors->Draw();
  pad_w_errors_residual->Draw();
  pad_wout_errors_residual->Draw();
  */
  // TPad *pad_w_errors = new TPad("pad_w_errors","This is pad1",0.02,0.29,0.98,0.97);
  // pad_w_errors->SetFillColor(kWhite);
  // TPad *pad_w_errors_residual = new TPad("pad_w_errors_residual","This is pad2",0.02,0.02,0.98,0.27);
  // pad_w_errors_residual->SetFillColor(kWhite);
  // pad_w_errors->Draw();
  // pad_w_errors_residual->Draw();

  TPad *pad_w_errors = new TPad("pad_w_errors","This is pad1",0.0,0.0,1.0,1.0);
  pad_w_errors->SetFillColor(kWhite);
  pad_w_errors->Draw();
  TPad *pad_w_errors_residual = new TPad("pad_w_errors_residual","This is pad2",0.02,0.02,0.98,0.27); // unused for now
  
  TDirectory* tpi_dir = file->GetDirectory("ExportPulse/TPIs");
  TList* tpi_hists = tpi_dir->GetListOfKeys();
  int n_max_hists = tpi_hists->GetSize();

  TLegend* leg = new TLegend(0.1,0.6,0.40,0.9);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(0);


  //  TRandom3 random(seed);
  for (int i_hist = 0; i_hist < n_pulses_to_draw; ++i_hist) {

    //    int rndm = (random.Rndm())*n_max_hists;

    //    std::string pulse_name = tpi_hists->At(rndm)->GetName();
    std::string tpi_histname = "ExportPulse/TPIs/" + pulse_name;
 
    TH1F* hTPI = (TH1F*) file->Get(tpi_histname.c_str());
    if (!hTPI) {
      std::cout << "Problem obtaining TPI histogram " << tpi_histname << std::endl;
      return false;
    }
    else {
      TList *functions = hTPI->GetListOfFunctions();
      TPolyMarker *pm = (TPolyMarker*)functions->FindObject("TPolyMarker");
      if (pm) {
	//      	for (int i_pm = 0; i_pm < pm->GetN(); ++i_pm) {
	//	  *(pm->GetY() + i_pm) = *(pm->GetY() + i_pm) + 20;
	//      	}
	pm->SetMarkerColor(kBlue);
      }

      hTPI->SetLineWidth(2);
      hTPI->SetLineColor(kBlack);
      hTPI->GetXaxis()->SetTitleSize(0.05);
      hTPI->GetXaxis()->SetLabelSize(0.05);
      hTPI->GetXaxis()->SetTitleOffset(1.0);
      hTPI->GetYaxis()->SetTitleSize(0.05);
      hTPI->GetYaxis()->SetLabelSize(0.05);
      hTPI->GetYaxis()->SetTitleOffset(1.07);

      pad_w_errors->cd();
      hTPI->Draw("HIST");
      //      hTPI->SetTitle("with error bars");
      hTPI->SetTitle(histtitle.c_str());
      if (pm) {
	pm->Draw("SAME");
      }

      /*      pad_wout_errors->cd();
      TH1F* hTPI_wout_errors = (TH1F*) hTPI->Clone("hTPI_wout_errors");
      hTPI_wout_errors->SetTitle("without error bars");
      hTPI_wout_errors->Draw("HIST P");
      if (pm) {
	pm->Draw("SAME");
      }
      */
      leg->AddEntry(hTPI, "Original TPI", "l");
      if (pm) {
	leg->AddEntry(pm, "Pulse Candidate Peaks", "p");
      }
    }


    TLatex* latex = new TLatex();
    latex->SetTextAlign(22);
    latex->SetTextSize(0.03);
    
    double ped_line_x1 = 0; double ped_line_y1 = 0;
    double ped_line_x2 = 5000; double ped_line_y2 = 0;
    TLine* ped_line = new TLine(ped_line_x1, ped_line_y1, ped_line_x2, ped_line_y2);
    ped_line->SetLineWidth(2);
    ped_line->SetLineStyle(kDashed);
    ped_line->SetLineColor(kRed);
    ped_line->Draw("LSAME");

    double text_x = 1000;
    double text_y = 40;
    latex->SetTextColor(kRed);
    //    latex->DrawLatex(text_x, text_y, "pedestal");
    
    std::stringstream tap_histname;
    const int n_max_sub_pulses = 3;
    TH1F* hSumTAP = NULL;
    TH1F* hSumTAPRebin = NULL;
    bool more_than_one_sub_pulse = false;
    for (int i_sub_pulse = 0; i_sub_pulse < n_max_sub_pulses; ++i_sub_pulse) {
      tap_histname.str("");
      tap_histname << "ExportPulse/TAPs/" << pulse_name << "_" << i_sub_pulse <<  "_templateAP";
      TH1F* hTAP = (TH1F*) file->Get(tap_histname.str().c_str());
      if (!hTAP) {
	std::cout << "Problem obtaining TAP histogram " << tap_histname.str() << std::endl;
	continue;
      }

      tap_histname << "_rebin";
      TH1F* hTAPRebin = (TH1F*) file->Get(tap_histname.str().c_str());
      if (!hTAPRebin) {
	std::cout << "Problem obtaining TAP histogram " << tap_histname.str() << std::endl;
      }
      else {

	pad_w_errors->cd();
	hTAP->SetLineColor(kBlue);
	hTAP->Draw("HIST E SAME");

	double amplitude = hTAP->GetMaximum(); // pulse amplitude
	double max_peak_pos = hTAP->GetXaxis()->GetBinCenter(hTAP->GetMaximumBin()); // maximum bin
	double cf_amp = 0.20*amplitude;//hTPI->GetBinContent(hTAP->GetMaximumBin()); // pulse height at time
	double time = hTAP->GetXaxis()->GetBinCenter(hTAP->FindFirstBinAbove(cf_amp)); // pulse time

	double amp_line_x1 = 0; double amp_line_y1 = amplitude;
	double amp_line_x2 = max_peak_pos; double amp_line_y2 = amplitude;
	TLine* amp_line = new TLine(amp_line_x1, amp_line_y1, amp_line_x2, amp_line_y2);
	amp_line->SetLineWidth(2);
	amp_line->SetLineStyle(kDashed);
	amp_line->SetLineColor(kRed);
	amp_line->Draw("LSAME");

	double amp_arrow_x1 = amp_line_x2; double amp_arrow_y1 = ped_line_y1;
	double amp_arrow_x2 = amp_line_x2; double amp_arrow_y2 = amp_line_y2;
	TArrow* amp_arrow = new TArrow(amp_arrow_x1, amp_arrow_y1, amp_arrow_x2, amp_arrow_y2,0.01, "<>");
	amp_arrow->SetLineWidth(1);
	amp_arrow->SetLineStyle(kSolid);
	amp_arrow->SetLineColor(kDashed);
	amp_arrow->SetLineColor(kBlack);
	amp_arrow->Draw();

	latex->SetTextAlign(12);
	text_x = amp_arrow_x2;// + 50;
	text_y = amp_arrow_y2 + 50;//amp_arrow_y1 + (amp_arrow_y2 - amp_arrow_y1)/4;
	latex->SetTextColor(kBlack);
	latex->SetTextSize(0.05);
	latex->SetTextAlign(22);
	latex->DrawLatex(text_x, text_y, "pulse amplitude");

	// double cf_line_x1 = 0; double cf_line_y1 = cf_amp;
	// double cf_line_x2 = amp_line_x2-500; double cf_line_y2 = cf_amp;
	// TLine* cf_line = new TLine(cf_line_x1, cf_line_y1, cf_line_x2, cf_line_y2);
	// cf_line->SetLineWidth(2);
	// cf_line->SetLineStyle(kDashed);
	// cf_line->SetLineColor(kRed);
	// cf_line->Draw("LSAME");

	// double cf_arrow_x1 = cf_line_x2; double cf_arrow_y1 = ped_line_y1;
	// double cf_arrow_x2 = cf_line_x2; double cf_arrow_y2 = cf_line_y2;
	// TArrow* cf_arrow = new TArrow(cf_arrow_x1, cf_arrow_y1, cf_arrow_x2, cf_arrow_y2,0.01, "<>");
	// cf_arrow->SetLineWidth(1);
	// cf_arrow->SetLineStyle(kSolid);
	// cf_arrow->SetLineColor(kDashed);
	// cf_arrow->SetLineColor(kBlack);
	// cf_arrow->Draw();

	// text_x = cf_arrow_x2 + 50;
	// text_y = cf_arrow_y1 + (cf_arrow_y2 - cf_arrow_y1)/2;
	// latex->DrawLatex(text_x, text_y, "20% pulse amplitude");

	double time_arrow_x1 = time; double time_arrow_y1 = -50;
	double time_arrow_x2 = time; double time_arrow_y2 = cf_amp;
	TArrow* time_arrow = new TArrow(time_arrow_x1, time_arrow_y1, time_arrow_x2, time_arrow_y2, 0.01, "<");
	time_arrow->SetLineWidth(2);
	time_arrow->SetLineStyle(kSolid);
	time_arrow->SetLineColor(kBlue);
	time_arrow->Draw();

	text_x = time_arrow_x2 - 50;// + 50;
	text_y = time_arrow_y2;//-75/2;//time_arrow_y1 + (time_arrow_y2 - time_arrow_y1)/2;
	latex->SetTextAlign(22);
	latex->SetTextColor(kBlue);
	latex->DrawLatex(text_x, text_y, "pulse time");

	//	pad_wout_errors->cd();
	//	hTAP->Draw("HIST SAME");

	pad_w_errors->cd();
	TList* functions = hTAP->GetListOfFunctions();
	TPaveText *text_b = (TPaveText*)functions->FindObject("TPave");
	if (text_b) {
	  //	  double x_width = std::fabs(text_b->GetX1() - text_b->GetX2());
	  double y_height = std::fabs(text_b->GetY1() - text_b->GetY2());
	  text_b->SetY1(text_b->GetY1() - i_sub_pulse*y_height);
	  text_b->SetY2(text_b->GetY2() - i_sub_pulse*y_height);
	  //	  text_b->Draw();
	}
	// TLine* time_line = (TLine*)functions->FindObject("TLine");
	// if (time_line) {
	//   time_line->Draw();
	// }

      }

      if (!hSumTAP) {
	leg->AddEntry(hTAP, "Template Fit TAP", "l");

	hSumTAP = (TH1F*) hTAP->Clone("hSumTAP");
	hSumTAPRebin = (TH1F*) hTAPRebin->Clone("hSumTAPRebin");
      }
      else if (hTAP) {
	more_than_one_sub_pulse = true;
	hSumTAP->Add(hTAP);
	hSumTAPRebin->Add(hTAPRebin);

	leg->AddEntry(hSumTAP, "Sum of TAPs", "l");
      }
    }
    //    if (more_than_one_sub_pulse) {// only want to plot the sum if there is more than one template

      tap_histname << "_residual";
      TH1F* hTAP_residual = (TH1F*) hTPI->Clone(tap_histname.str().c_str());
      hTAP_residual->Add(hSumTAPRebin, -1.0);
      /*      for (int i_bin = 1; i_bin <= hTAP_residual->GetNbinsX(); ++i_bin) {
	double sum_bin_error = hSumTAPRebin->GetBinError(i_bin);
	if (sum_bin_error == 0) {
	  continue;
	}
	double delta = hTAP_residual->GetBinContent(i_bin);
	double chi2 = (delta*delta) / (sum_bin_error*sum_bin_error);
	std::cout << "AE: Bin = " << i_bin << ", TAP = " << hSumTAPRebin->GetBinContent(i_bin) << ", TPI = " << hTPI->GetBinContent(i_bin) << ", delta = " << delta << ", sum_bin_error = " << sum_bin_error << ", chi2 = " << chi2 << std::endl;
	hTAP_residual->SetBinContent(i_bin, chi2);
      }
      */
      //      hTAP_residual->Divide(hTPI);
      hTAP_residual->GetXaxis()->SetTitleOffset(1);
      hTAP_residual->GetXaxis()->SetTitleSize(0.1);
      hTAP_residual->GetXaxis()->SetLabelSize(0.1);
      hTAP_residual->SetYTitle("TPI - TAP");
      //      hTAP_residual->SetYTitle("#chi^2");
      hTAP_residual->GetYaxis()->SetTitleSize(0.1);
      hTAP_residual->GetYaxis()->SetTitleOffset(0.1);
      hTAP_residual->GetYaxis()->SetLabelSize(0.1);
      
      pad_w_errors_residual->cd();
      hTAP_residual->SetLineColor(kGreen);
      hTAP_residual->Draw("HIST SAME");
      
      //      pad_wout_errors_residual->cd();
      //      hTAP_residual->Draw("HIST SAME");

      hSumTAP->SetLineColor(kGreen);

      pad_w_errors->cd();
      hSumTAP->Draw("HIST E SAME");

      //      pad_wout_errors->cd();
      //      hSumTAP->Draw("HIST SAME");

      // Draw the TPI again but with a bolder line and color
      hTPI->SetMaximum(hSumTAP->GetMaximum()*1.10);
      
      pad_w_errors->cd();
      hTPI->Draw("HIST SAME");

      //      pad_wout_errors->cd();
      //      hTPI->Draw("HIST SAME");
      //    }

    //    leg->Draw();
  }
  //  file->Close();

  TLatex* latex = new TLatex();
  latex->SetTextAlign(22);
  latex->SetTextSize(0.07);
  latex->SetTextColor(kBlack);
  latex->DrawLatex(3500, 800, "Template Fitter");
  latex->DrawLatex(3500, 700, "Pulse Analysis");

  
  return true;
}
bool DrawTPIAndTemplateTAPs_fromPulseViewer(std::string filename, std::string pulse_name, TCanvas& c1, std::string histtitle) { 
  TFile* file = new TFile(filename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Problem opening file " << filename << std::endl;
    return false;
  }
  DrawTPIAndTemplateTAPs_fromPulseViewer_file(file, pulse_name, c1, histtitle);
  return true;
}
 
