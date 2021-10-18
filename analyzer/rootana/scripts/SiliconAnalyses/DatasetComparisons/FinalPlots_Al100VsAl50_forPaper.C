#include "../Utils/IntegrateRates.C"

#include "CanvasPartition.h"

void FinalPlots_Al100VsAl50_forPaper(std::string savedir = "", std::ostream& numbers_file = std::cout) {

  gStyle->SetOptStat(0);

  // Number of PADS
  const Int_t Nx = 1;
  const Int_t Ny = 1;

  // Margins
  Float_t lMargin = 0.12;
  Float_t rMargin = 0.05;
  Float_t bMargin = 0.08;
  Float_t tMargin = 0.05;

  // Canvas setup
  TCanvas* c = new TCanvas("c", "c");//"c", "c",500,700);
  //  TCanvas* c = new TCanvas("c", "c",500*2,700);
  c->SetFillStyle(4000);
  //  c->SetLogy();

  //  CanvasPartition(c,Nx,Ny,lMargin,rMargin,bMargin,tMargin);

  TLegend* leg = new TLegend(0.6, 0.5, 0.8, 0.8);
  leg->SetBorderSize(0);
  leg->SetTextFont(43);
  leg->SetTextSize(16);
  leg->SetFillColor(kWhite);
  std::string legheader = "AlCap Al, Thin vs Thick";
  leg->SetHeader(legheader.c_str());

  const int n_datasets = 2;
  std::string datasets[n_datasets] = {"al100", "al50"};
  std::string targets[n_datasets] = {"Al", "Al"};
  std::string labels[n_datasets] = {"thick target", "thin target"};
  TLegend* legs[n_datasets];
  Int_t colours[n_datasets] = {kRed, kBlue};

  //  const int n_arms = 2;
  //  std::string arms[n_arms] = {"SiL", "SiR"};
  
  const int n_particles = 1;
  std::string particle_names[n_particles] = {"proton"};//, "deuteron", "triton", "alpha"};
  std::string Particle_names[n_particles] = {"Proton"};//, "Deuteron", "Triton", "Alpha"};
  TPad *pad[Nx][Ny];

  TGraphAsymmErrors* hCurrentYAxisHist = 0;
  numbers_file << "% FinalPlots_Al100VsAl50_forPaper.C" << std::endl;
  for (int i_dataset = 0; i_dataset < n_datasets; ++i_dataset) {
    std::string dataset = datasets[i_dataset];
    //    if (dataset=="si16b") {
    //      continue;
    //    }

    std::string target = targets[i_dataset];
    std::string infilename = "/data/ssd3/R15bAnalysis/hists/final-" + dataset + "-plots.root";
    TFile* infile = new TFile(infilename.c_str(), "READ");

    std::stringstream axistitle;
    //    int rebin_factor = 1;

    //    for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
    //      std::string arm = arms[i_arm];

    Int_t colour = colours[i_dataset];

    for (int i_particle = 0; i_particle < n_particles; ++i_particle) {

      std::string particle = particle_names[i_particle];
      std::string Particle = Particle_names[i_particle];

      std::string grname = "gCombined";
      grname += "-" + dataset;
      grname += "-" + particle;

      TGraphAsymmErrors* grStatAndSyst = 0;
      if (dataset != "si16b") {
	TGraphErrors* grStatAndSystOriginal =  (TGraphErrors*) infile->Get(grname.c_str());
	grStatAndSyst =  new TGraphAsymmErrors(grStatAndSystOriginal->GetN(),
					       grStatAndSystOriginal->GetX(),
					       grStatAndSystOriginal->GetY(),
					       grStatAndSystOriginal->GetEX(),
					       grStatAndSystOriginal->GetEX(),
					       grStatAndSystOriginal->GetEY(),
					       grStatAndSystOriginal->GetEY()
					       );
	for (int i_point = 0; i_point < grStatAndSyst->GetN(); ++i_point) {
	  *(grStatAndSyst->GetY()+i_point) = *(grStatAndSyst->GetY()+i_point)/0.5; // convert to per MeV
	  *(grStatAndSyst->GetEYhigh()+i_point) = *(grStatAndSyst->GetEYhigh()+i_point)/0.5; // convert to per MeV
	  *(grStatAndSyst->GetEYlow()+i_point) = *(grStatAndSyst->GetEYlow()+i_point)/0.5; // convert to per MeV
	}
      }
      else {
	grStatAndSyst =  (TGraphAsymmErrors*) infile->Get(grname.c_str());
      }

      grname = "gStats";
      grname += "-" + dataset;
      grname += "-" + particle;

      TGraphAsymmErrors* grStat = 0;
      if (dataset != "si16b") {
	TGraphErrors* grStatOriginal =  (TGraphErrors*) infile->Get(grname.c_str());
	grStat =  new TGraphAsymmErrors(grStatOriginal->GetN(),
					grStatOriginal->GetX(),
					grStatOriginal->GetY(),
					grStatOriginal->GetEX(),
					grStatOriginal->GetEX(),
					grStatOriginal->GetEY(),
					grStatOriginal->GetEY()
					);

	for (int i_point = 0; i_point < grStat->GetN(); ++i_point) {
	  *(grStat->GetY()+i_point) = *(grStat->GetY()+i_point)/0.5; // convert to per MeV
	  *(grStat->GetEYhigh()+i_point) = *(grStat->GetEYhigh()+i_point)/0.5; // convert to per MeV
	  *(grStat->GetEYlow()+i_point) = *(grStat->GetEYlow()+i_point)/0.5; // convert to per MeV
	}

      }
      else {
	grStat =  (TGraphAsymmErrors*) infile->Get(grname.c_str());
      }

      if (grStatAndSyst) {
	//	grStatAndSyst->Rebin(rebin_factor);
	//	grStatAndSyst->SetStats(false);
	grStatAndSyst->SetMarkerColor(colour);
	grStatAndSyst->SetLineColor(colour);
	grStatAndSyst->SetLineWidth(2);
	grStatAndSyst->SetFillColorAlpha(colour, 0.3);
	//	grStatAndSyst->SetFillStyle(1001);
	grStat->SetMarkerColor(colour);
	grStat->SetLineColor(colour);
	grStat->SetLineWidth(2);
	//      grStatAndSyst->GetXaxis()->SetRangeUser(0, 20);
	//      grStatAndSyst->SetMinimum(0.5);
      }
    


      c->cd(0);
      // Get the pads previously created.
      // char pname[16];
      // int i = 0;//i_arm;
      // int j = i_dataset;
      // sprintf(pname,"pad_%i_%i",i,j);
      // pad[i][j] = (TPad*) gROOT->FindObject(pname);
      // pad[i][j]->Draw();
      // pad[i][j]->SetFillStyle(4000);
      // pad[i][j]->SetFrameFillStyle(4000);
      // pad[i][j]->cd();
      // pad[i][j]->SetLogy();

      // Delete bad Al100 points
      if (dataset == "al100") {
	double min_energy = 4;
	for (int i_point = 0; i_point < grStatAndSyst->GetN(); ++i_point) {
	  if (*(grStatAndSyst->GetX()+i_point) < min_energy) {
	    grStatAndSyst->RemovePoint(i_point);
	    --i_point;
	  }
	}
	for (int i_point = 0; i_point < grStat->GetN(); ++i_point) {
	  if (*(grStat->GetX()+i_point) < min_energy) {
	    grStat->RemovePoint(i_point);
	    --i_point;
	  }
	}
      }
	
      if (i_dataset == 0) {
	// Size factors
	//	Float_t xFactor = pad[0][0]->GetAbsWNDC()/pad[i][j]->GetAbsWNDC();
	//	Float_t yFactor = pad[0][0]->GetAbsHNDC()/pad[i][j]->GetAbsHNDC();
	//	std::cout << "xFactor, yFactor = " << xFactor << ", " << yFactor << std::endl;
	char hname[16];
	//	sprintf(hname,"h_%i_%i",i,j);
	if (i_dataset==0 && grStatAndSyst) {
	  hCurrentYAxisHist = (TGraphAsymmErrors*) grStatAndSyst->Clone("hCurrentYAxisHist"); // keep track of the y-axis so that left and right match)
	}
	TGraphAsymmErrors *hFrame;
	if (grStatAndSyst) {
	  hFrame = (TGraphAsymmErrors*) grStatAndSyst->Clone(hname);
	}
	else {
	  hFrame = (TGraphAsymmErrors*) hCurrentYAxisHist->Clone(hname);
	}
	//	hFrame->Reset();
	hFrame->SetLineColor(kWhite);
	hFrame->SetFillColor(kWhite);
	hFrame->SetFillStyle(0);
	hFrame->Draw("APL2");

	//	hFrame->GetXaxis()->ChangeLabel(1,-1,0.); // remove the first label
	//	hFrame->GetXaxis()->ChangeLabel(-1,-1,0.); // remove the last label

	// y axis range
	hFrame->GetXaxis()->SetRangeUser(2, 15);
	hFrame->GetYaxis()->SetRangeUser(0,0.008);
	//
	//	hFrame->GetYaxis()->SetRangeUser(5e1, 5e5);

	// Format for y axis
	hFrame->GetYaxis()->SetLabelFont(43);
	hFrame->GetYaxis()->SetLabelSize(16);
	//	hFrame->GetYaxis()->SetLabelOffset(0.02);
	hFrame->GetYaxis()->SetTitleFont(43);
	hFrame->GetYaxis()->SetTitleSize(16);
	hFrame->GetYaxis()->SetTitleOffset(1.5);

	//	hFrame->GetYaxis()->CenterTitle();
	hFrame->GetYaxis()->SetNdivisions(505);

	// TICKS Y Axis
	//	hFrame->GetYaxis()->SetTickLength(xFactor*0.04/yFactor);

	// Format for x axis
	hFrame->GetXaxis()->SetLabelFont(43);
	hFrame->GetXaxis()->SetLabelSize(16);
	//	hFrame->GetXaxis()->SetLabelOffset(0.02);
	hFrame->GetXaxis()->SetTitleFont(43);
	hFrame->GetXaxis()->SetTitleSize(16);
	//	hFrame->GetXaxis()->SetTitleOffset(3.5);
	//	hFrame->GetXaxis()->CenterTitle();
	hFrame->GetXaxis()->SetNdivisions(505);

	// TICKS X Axis
	//	hFrame->GetXaxis()->SetTickLength(yFactor*0.06/xFactor);

	axistitle.str("");
	axistitle << "Yield / Capture / MeV";
	hFrame->GetXaxis()->SetTitle("Energy [MeV]");
	hFrame->GetYaxis()->SetTitle(axistitle.str().c_str());

	// TLine* line = new TLine();
	// line->SetLineColor(kBlack);
	// line->SetLineStyle(kDashed);
	// double y = 
	// std::cout << "Bot. Marg. = " << pad[i][j]->GetBottomMargin() << std::endl;
	// std::cout << "Top Marg. = " << pad[i][j]->GetTopMargin() << std::endl;
	// //	std::cout << "AbsHNDC = " << pad[i][j]->GetHNDC() << std::endl;
	// std::cout << "y = " << y << std::endl;
	// line->DrawLineNDC(0.0, y, 1.0, y);
	
	//	double leg_x1 = pad[i][j]->GetLeftMargin() + 0.75*((1-pad[i][j]->GetRightMargin()) - pad[i][j]->GetLeftMargin());;
	//	double leg_x2 =  pad[i][j]->GetLeftMargin() + 0.90*((1-pad[i][j]->GetRightMargin()) - pad[i][j]->GetLeftMargin());;
	//	double leg_y1 = pad[i][j]->GetBottomMargin() + 0.60*((1-pad[i][j]->GetTopMargin()) - pad[i][j]->GetBottomMargin());
	//	double leg_y2 = pad[i][j]->GetBottomMargin() + 0.90*((1-pad[i][j]->GetTopMargin()) - pad[i][j]->GetBottomMargin());
	//	legs[i_dataset] = new TLegend(leg_x1, leg_y1, leg_x2, leg_y2);
	//	std::cout << leg_x1 << ", " << leg_y1 << ", " << leg_x2 << ", " << leg_y2 << std::endl;
	//	legs[i_dataset]->ConvertNDCtoPad();
	//	std::cout << pad[i][j]->GetXlowNDC()<< ", " << pad[i][j]->GetYlowNDC()<< ", " << pad[i][j]->GetXlowNDC()<< ", " << pad[i][j]->GetYlowNDC() <<std::endl;
	//	std::cout << pad[i][j]->GetX1()<< ", " << pad[i][j]->GetY1()<< ", " << pad[i][j]->GetX2()<< ", " << pad[i][j]->GetY2() <<std::endl;
	//	legs[i_dataset]->SetBorderSize(0);
	//	legs[i_dataset]->SetTextFont(43);
	//	legs[i_dataset]->SetTextSize(16);
	//	legs[i_dataset]->SetFillColor(kWhite);
	//	std::string legheader = "AlCap " + target;// + ", " + arm;
	//	legs[i_dataset]->SetHeader(legheader.c_str());
	//	std::cout << "New TLegend: " << leg << std::endl;
	//	std::cout << "Set Header: " << legheader << std::endl;
	//	std::cout << "Set Text Size: " << leg->GetTextSize() << std::endl;
      }

      if (grStatAndSyst) {
	grStatAndSyst->Draw("PL2 SAME");
      }
      if (grStat) {
	grStat->Draw("PL SAME");
      }
      if (grStatAndSyst && grStat) {
	double min_energy = 4;//min_integral_energies[i_particle];
	double max_energy = 8;//max_integral_energies[i_particle];
	double rate = 0; double total_stat_error = 0;
	double total_high_syst_error = 0; double total_low_syst_error = 0;
	//	std::cout << "AE: grStat = " << grStat << ", grStatAndSyst = " << grStatAndSyst << std::endl;
	IntegrateRates_wStatAndStatSystSpectra(grStat, grStatAndSyst,
					       min_energy, max_energy,
					       rate, total_stat_error,
					       total_high_syst_error, total_low_syst_error);
	std::cout << dataset << ", " << particle << " " << min_energy << " MeV -- " << max_energy << " MeV: rate = (" << rate*1e3 << ")(" << total_stat_error*1e3 << ")(+" << total_high_syst_error*1e3 << ")(-" << total_low_syst_error*1e3 << ") x 10^{-3}" << std::endl;
	// numbers_file << std::fixed << std::setprecision(1);
	// numbers_file << "\\newcommand\\" << target << Particle << "MinE{" << min_energy << "}" << std::endl;
	// numbers_file << "\\newcommand\\" << target << Particle << "MaxE{" << max_energy << "}" << std::endl;

	// numbers_file << std::fixed << std::setprecision(2);
	// numbers_file << "\\newcommand\\" << target << Particle << "BestRateBare{" << "" << rate/1e-3 << "}" << std::endl;
	// numbers_file << "\\newcommand\\" << target << Particle << "BestStatErrBare{" << total_stat_error/1e-3 << "}" << std::endl;
	// numbers_file << "\\newcommand\\" << target << Particle << "BestHighSystErrBare{" << total_high_syst_error/1e-3 << "}" << std::endl;
	// numbers_file << "\\newcommand\\" << target << Particle << "BestLowSystErrBare{" << total_low_syst_error/1e-3 << "}" << std::endl;
	// numbers_file << "\\newcommand\\" << target << Particle << "BestRateFull{$(\\" << target << Particle << "BestRateBare \\pm \\" << target << Particle << "BestStatErrBare ~\\text{(stat.)} ^{+\\" << target << Particle << "BestHighSystErrBare}_{-\\" << target << Particle << "BestLowSystErrBare}~\\text{(syst.)}) \\times 10^{-3}$}" << std::endl;
	// numbers_file << "\\newcommand\\" << target << Particle << "BestStatFracUncertainty{$\\pm" << (total_stat_error/rate)*100 << "\\%$}" << std::endl;
	// numbers_file << "\\newcommand\\" << target << Particle << "BestSystFracUncertainty{$^{+" << (total_high_syst_error/rate)*100 << "\\%}_{-" << (total_low_syst_error/rate)*100 << "\\%}$}" << std::endl;
	// numbers_file << std::setprecision(1);
	// numbers_file << "\\newcommand\\" << target << Particle << "BestFracUncertainty{\\" << target << Particle << "BestStatFracUncertainty ~(stat.) \\" << target << Particle << "BestSystFracUncertainty ~(syst.)}" << std::endl;

	// numbers_file << std::setprecision(2);
	// numbers_file << "\\newcommand\\" << target << Particle << "Table{" << rate/1e-3 << "(" << std::setprecision(0) << total_stat_error/1e-3  * 100<< ")" << "(";
	// if (total_high_syst_error > total_low_syst_error) {
	//   numbers_file << total_high_syst_error/1e-3 * 100;
	// }
	// else {
	//   numbers_file << total_low_syst_error/1e-3 * 100;
	// }
	// numbers_file << ")" << "}" << std::endl;

      }

      std::stringstream leglabel;
      leglabel.str("");
      leglabel << labels[i_dataset];
      leg->AddEntry(grStatAndSyst, leglabel.str().c_str(), "l");
    }

    // if (!(dataset == "si16b" && arm == "SiL")) {
    //    legs[i_dataset]->Draw();
    // }
    //    infile->Close();
    //    pad->Update();
    //    TLatex* latex = new TLatex();
    //    latex->DrawLatexNDC(0.55, 0.80, "AlCap Preliminary");
  }
  c->Update();
  c->cd();
  leg->Draw();

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_DatasetComparison_Al100VsAl50";
    std::string pdfname = savename + ".pdf";
    c->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c->SaveAs(pngname.c_str());
  }


  //  c->SaveAs("~/plots/2021-07-09/FinalPlots.pdf");
}
