#include "../Utils/IntegrateRates.C"

#include "CanvasPartition.h"

void FinalPlots_forPaper(std::string savedir = "", std::ostream& numbers_file = std::cout) {

  std::ofstream table_file("/home/edmonds/plots/paper/data-table.tex", std::ofstream::out);
  if (!table_file.is_open()) {
    std::cout << "ERROR with table_file" << std::endl;
    return;
  }

  gStyle->SetOptStat(0);

  // Number of PADS
  const Int_t Nx = 1;
  const Int_t Ny = 3;

  // Margins
  Float_t lMargin = 0.12;
  Float_t rMargin = 0.05;
  Float_t bMargin = 0.08;
  Float_t tMargin = 0.05;

  // Canvas setup
  TCanvas* c = new TCanvas("c", "c",500,700);
  //  TCanvas* c = new TCanvas("c", "c",500*2,700);
  c->SetFillStyle(4000);

  CanvasPartition(c,Nx,Ny,lMargin,rMargin,bMargin,tMargin);

  const int n_datasets = 3;
  std::string datasets[n_datasets] = {"ti50", "si16b", "al50"};
  std::string targets[n_datasets] = {"Ti", "Si", "Al"};
  TLegend* legs[n_datasets];

  //  const int n_arms = 2;
  //  std::string arms[n_arms] = {"SiL", "SiR"};
  
  const int n_particles = 4;
  std::string particle_names[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  std::string Particle_names[n_particles] = {"Proton", "Deuteron", "Triton", "Alpha"};
  std::string particle_inits[n_particles] = {"p", "d", "t", "\\alpha"};
  Int_t colours[n_particles] = {kRed, kBlue, kMagenta, kGreen+3};
  double min_integral_energies[n_datasets][n_particles] = {{3.5, 4.5, 5, 15}, {4, 5, 6, 15}, {3.5, 4.5, 5, 15}};
  double max_integral_energies[n_datasets][n_particles] = {{20, 17, 17, 20}, {20, 17, 17, 20}, {20, 17, 17, 20}};
  TGraphAsymmErrors* grStats[n_datasets][n_particles];
  TGraphAsymmErrors* grStatAndSysts[n_datasets][n_particles];

  TPad *pad[Nx][Ny];

  TGraphAsymmErrors* hCurrentYAxisHist = 0;  
  for (int i_dataset = 0; i_dataset < n_datasets; ++i_dataset) {
    std::string dataset = datasets[i_dataset];
    //    if (dataset=="si16b") {
    //      continue;
    //    }

    std::string target = targets[i_dataset];
    std::string infilename = "/data/ssd3/R15bAnalysis/hists/final-" + dataset + "-plots";
    infilename += ".root";
    //    infilename = "./final-" + dataset + "-plots.root";
    TFile* infile = new TFile(infilename.c_str(), "READ");

    std::stringstream axistitle;
    //    int rebin_factor = 1;

    //    for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
    //      std::string arm = arms[i_arm];

    numbers_file << "% FinalPlots_forPaper.C" << std::endl;
    for (int i_particle = 0; i_particle < n_particles; ++i_particle) {

      std::string particle = particle_names[i_particle];
      std::string Particle = Particle_names[i_particle];
      std::string particle_init = particle_inits[i_particle];
      Int_t colour = colours[i_particle];

      std::string grname = "gCombined";
      grname += "-" + dataset;
      grname += "-" + particle;

      // TH1F* hFoldedSpectrum = new TH1F("h", "", 100,0,5);
      // hFoldedSpectrum->FillRandom("gaus",10000);
      // hFoldedSpectrum->GetXaxis()->SetTitle("x axis");
      // hFoldedSpectrum->GetYaxis()->SetTitle("y axis");
      //      std::cout << dataset << ": " << grname << std::endl;
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
	grStatAndSyst->SetFillColorAlpha(colour, .3);
	//	grStatAndSyst->SetFillStyle(3003);
	grStat->SetMarkerColor(colour);
	grStat->SetLineColor(colour);
	grStat->SetLineWidth(2);
	//      grStatAndSyst->GetXaxis()->SetRangeUser(0, 20);
	//      grStatAndSyst->SetMinimum(0.5);
      }
    


      c->cd(0);
      // Get the pads previously created.
      char pname[16];
      int i = 0;//i_arm;
      int j = i_dataset;
      sprintf(pname,"pad_%i_%i",i,j);
      pad[i][j] = (TPad*) gROOT->FindObject(pname);
      pad[i][j]->Draw();
      pad[i][j]->SetFillStyle(4000);
      pad[i][j]->SetFrameFillStyle(4000);
      pad[i][j]->cd();
      pad[i][j]->SetLogy();
	
      if (i_particle == 0) {
	// Size factors
	Float_t xFactor = pad[0][0]->GetAbsWNDC()/pad[i][j]->GetAbsWNDC();
	Float_t yFactor = pad[0][0]->GetAbsHNDC()/pad[i][j]->GetAbsHNDC();
	//	std::cout << "xFactor, yFactor = " << xFactor << ", " << yFactor << std::endl;
	char hname[16];
	sprintf(hname,"h_%i_%i",i,j);
	if (i==0 && grStatAndSyst) {
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
	hFrame->SetFillStyle(kSolid);
	hFrame->Draw("APL2");
	//	hFrame->GetXaxis()->ChangeLabel(1,-1,0.); // remove the first label
	hFrame->GetXaxis()->ChangeLabel(-1,-1,0.); // remove the last label

	// y axis range
	hFrame->GetXaxis()->SetRangeUser(2, 20);
	hFrame->GetYaxis()->SetRangeUser(5e-6,5e-2);
	//
	//	hFrame->GetYaxis()->SetRangeUser(5e1, 5e5);

	// Format for y axis
	hFrame->GetYaxis()->SetLabelFont(43);
	hFrame->GetYaxis()->SetLabelSize(16);
	hFrame->GetYaxis()->SetLabelOffset(0.02);
	hFrame->GetYaxis()->SetTitleFont(43);
	hFrame->GetYaxis()->SetTitleSize(16);
	hFrame->GetYaxis()->SetTitleOffset(2.5);

	hFrame->GetYaxis()->CenterTitle();
	hFrame->GetYaxis()->SetNdivisions(505);

	// TICKS Y Axis
	hFrame->GetYaxis()->SetTickLength(xFactor*0.04/yFactor);

	// Format for x axis
	hFrame->GetXaxis()->SetLabelFont(43);
	hFrame->GetXaxis()->SetLabelSize(16);
	hFrame->GetXaxis()->SetLabelOffset(0.02);
	hFrame->GetXaxis()->SetTitleFont(43);
	hFrame->GetXaxis()->SetTitleSize(16);
	hFrame->GetXaxis()->SetTitleOffset(3.5);
	hFrame->GetXaxis()->CenterTitle();
	hFrame->GetXaxis()->SetNdivisions(505);

	// TICKS X Axis
	hFrame->GetXaxis()->SetTickLength(yFactor*0.06/xFactor);

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
	
	double leg_x1 = pad[i][j]->GetLeftMargin() + 0.75*((1-pad[i][j]->GetRightMargin()) - pad[i][j]->GetLeftMargin());;
	double leg_x2 =  pad[i][j]->GetLeftMargin() + 0.90*((1-pad[i][j]->GetRightMargin()) - pad[i][j]->GetLeftMargin());;
	double leg_y1 = pad[i][j]->GetBottomMargin() + 0.60*((1-pad[i][j]->GetTopMargin()) - pad[i][j]->GetBottomMargin());
	double leg_y2 = pad[i][j]->GetBottomMargin() + 0.90*((1-pad[i][j]->GetTopMargin()) - pad[i][j]->GetBottomMargin());
	legs[i_dataset] = new TLegend(leg_x1, leg_y1, leg_x2, leg_y2);
	//	std::cout << leg_x1 << ", " << leg_y1 << ", " << leg_x2 << ", " << leg_y2 << std::endl;
	//	legs[i_dataset]->ConvertNDCtoPad();
	//	std::cout << pad[i][j]->GetXlowNDC()<< ", " << pad[i][j]->GetYlowNDC()<< ", " << pad[i][j]->GetXlowNDC()<< ", " << pad[i][j]->GetYlowNDC() <<std::endl;
	//	std::cout << pad[i][j]->GetX1()<< ", " << pad[i][j]->GetY1()<< ", " << pad[i][j]->GetX2()<< ", " << pad[i][j]->GetY2() <<std::endl;
	legs[i_dataset]->SetBorderSize(0);
	legs[i_dataset]->SetTextFont(43);
	legs[i_dataset]->SetTextSize(16);
	legs[i_dataset]->SetFillColor(kWhite);
	std::string legheader = "AlCap " + target;// + ", " + arm;
	legs[i_dataset]->SetHeader(legheader.c_str());
	//	std::cout << "New TLegend: " << leg << std::endl;
	//	std::cout << "Set Header: " << legheader << std::endl;
	//	std::cout << "Set Text Size: " << leg->GetTextSize() << std::endl;

      }	

      if (grStatAndSyst && grStat) {
	double min_energy = min_integral_energies[i_dataset][i_particle];
	double max_energy = max_integral_energies[i_dataset][i_particle];
	double rate = 0; double total_stat_error = 0;
	double total_high_syst_error = 0; double total_low_syst_error = 0;
	//	std::cout << "AE: grStat = " << grStat << ", grStatAndSyst = " << grStatAndSyst << std::endl;
	IntegrateRates_wStatAndStatSystSpectra(grStat, grStatAndSyst,
					       min_energy, max_energy,
					       rate, total_stat_error,
					       total_high_syst_error, total_low_syst_error);
	
	TF1* fit_fn = new TF1("fit_fn", "[0]*TMath::Exp(-x/[1])", min_energy,max_energy);
	fit_fn->SetParameters(0.1, 2.0);
	//	grStatAndSyst->Fit(fit_fn, "R0N");
	std::cout << std::setprecision(2);
	std::cout << dataset << ", " << particle << ":" << std::endl;;
	std::cout << "N_0 = " << fit_fn->GetParameter(0) << " +/- " << fit_fn->GetParError(0) << std::endl;
	std::cout << "T_0 = " << fit_fn->GetParameter(1) << " +/- " << fit_fn->GetParError(1) << std::endl;
	fit_fn->SetLineColor(kBlack);
	//	fit_fn->Draw("LSAME");
	if (particle=="proton") {
	  TF1* fit_fn2 = new TF1("fit_fn2", "[0]*TMath::Exp(-x/[1]) + [2]*TMath::Exp(-x/[3])", min_energy,max_energy);
	  fit_fn2->SetParameters(0.1, 2.0, 0.05, 5.0);
	  //	  grStatAndSyst->Fit(fit_fn2, "R");
	}

	// Add systematics from
	// -- det misalignment (2.0%)
	std::cout << "Before = " << std::setprecision(5) << total_high_syst_error*1e3 << std::endl;
	total_high_syst_error = std::sqrt( (total_high_syst_error/rate)*(total_high_syst_error/rate) + 0.02*0.02) * rate;
	total_low_syst_error = std::sqrt( (total_low_syst_error/rate)*(total_low_syst_error/rate) + 0.02*0.02) * rate;
	// -- det placements (Si and all alphas only, 2.2%) (everything else. 1.6%)
	if (dataset=="si16b" || particle=="alpha") {
	  std::cout << "Det Place: " << dataset << ", " << particle << std::endl;
	  total_high_syst_error = std::sqrt( (total_high_syst_error/rate)*(total_high_syst_error/rate) + 0.022*0.022) * rate;
	  total_low_syst_error = std::sqrt( (total_low_syst_error/rate)*(total_low_syst_error/rate) + 0.022*0.022) * rate;
	}
	else {
	  //	  std::cout << "Det Place: " << dataset << ", " << particle << std::endl;
	  total_high_syst_error = std::sqrt( (total_high_syst_error/rate)*(total_high_syst_error/rate) + 0.016*0.016) * rate;
	  total_low_syst_error = std::sqrt( (total_low_syst_error/rate)*(total_low_syst_error/rate) + 0.016*0.016) * rate;
	}
	std::cout << "After = " << total_high_syst_error*1e3 << std::endl;
	std::cout << dataset << ", " << particle << " " << min_energy << " MeV -- " << max_energy << " MeV: rate = (" << rate*1e3 << ")(" << total_stat_error*1e3 << ")(+" << total_high_syst_error*1e3 << ")(-" << total_low_syst_error*1e3 << ") x 10^{-3}" << std::endl;
	numbers_file << std::fixed << std::setprecision(1);
	numbers_file << "\\newcommand\\" << target << Particle << "MinE{" << min_energy << "}" << std::endl;
	numbers_file << "\\newcommand\\" << target << Particle << "MaxE{" << max_energy << "}" << std::endl;
	numbers_file << "\\newcommand\\" << target << Particle << "BestRange{" << min_energy << " -- " << max_energy << "}" << std::endl;

	numbers_file << std::fixed << std::setprecision(2);
	numbers_file << "\\newcommand\\" << target << Particle << "BestRateBare{" << "" << rate/1e-3 << "}" << std::endl;
	numbers_file << "\\newcommand\\" << target << Particle << "BestStatErrBare{" << total_stat_error/1e-3 << "}" << std::endl;
	numbers_file << "\\newcommand\\" << target << Particle << "BestHighSystErrBare{" << total_high_syst_error/1e-3 << "}" << std::endl;
	numbers_file << "\\newcommand\\" << target << Particle << "BestLowSystErrBare{" << total_low_syst_error/1e-3 << "}" << std::endl;
	//	numbers_file << "\\newcommand\\" << target << Particle << "BestRateFull{$(\\" << target << Particle << "BestRateBare \\pm \\" << target << Particle << "BestStatErrBare ~\\text{(stat.)} ^{+\\" << target << Particle << "BestHighSystErrBare}_{-\\" << target << Particle << "BestLowSystErrBare}~\\text{(syst.)}) \\times 10^{-3}$}" << std::endl;
	numbers_file << "\\newcommand\\" << target << Particle << "BestRateFull{$\\" << target << Particle << "BestRateBare \\pm \\" << target << Particle << "BestStatErrBare ~\\text{(stat.)} \\pm \\" << target << Particle;
	if (total_high_syst_error > total_low_syst_error) {
	  numbers_file << "BestHighSystErrBare";
	}
	else {
	  numbers_file << "BestLowSystErrBare";
	}
	numbers_file << "~\\text{(syst.)}$}" << std::endl;// \\times 10^{-3}$}" << std::endl;
	numbers_file << "\\newcommand\\" << target << Particle << "BestStatFracUncertainty{$\\pm" << (total_stat_error/rate)*100 << "\\%$}" << std::endl;
	numbers_file << "\\newcommand\\" << target << Particle << "BestSystFracUncertainty{$^{+" << (total_high_syst_error/rate)*100 << "\\%}_{-" << (total_low_syst_error/rate)*100 << "\\%}$}" << std::endl;
	numbers_file << std::setprecision(1);
	numbers_file << "\\newcommand\\" << target << Particle << "BestFracUncertainty{\\" << target << Particle << "BestStatFracUncertainty ~(stat.) \\" << target << Particle << "BestSystFracUncertainty ~(syst.)}" << std::endl;

	numbers_file << std::setprecision(2);
	numbers_file << "\\newcommand\\" << target << Particle << "Table{" << rate/1e-3 << "(" << std::setprecision(0) << total_stat_error/1e-3  * 100<< ")" << "(";
	if (total_high_syst_error > total_low_syst_error) {
	  numbers_file << total_high_syst_error/1e-3 * 100;
	}
	else {
	  numbers_file << total_low_syst_error/1e-3 * 100;
	}
	numbers_file << ")" << "}" << std::endl;

	numbers_file << std::setprecision(2);
	numbers_file << "\\newcommand\\" << target << Particle << "TableFirst{" << rate/1e-3 << "(" << std::setprecision(0) << total_stat_error/1e-3  * 100<< "~stat.)" << "(";
	if (total_high_syst_error > total_low_syst_error) {
	  numbers_file << total_high_syst_error/1e-3 * 100;
	}
	else {
	  numbers_file << total_low_syst_error/1e-3 * 100;
	}
	numbers_file << "~syst.)" << "}" << std::endl;


	// Delete points not included in integrals
	for (int i_point = 0; i_point < grStatAndSyst->GetN(); ++i_point) {
	  if (*(grStatAndSyst->GetX()+i_point) < min_energy
	      || *(grStatAndSyst->GetX()+i_point) > max_energy) {
	    grStatAndSyst->RemovePoint(i_point);
	    --i_point;
	  }
	}
	for (int i_point = 0; i_point < grStat->GetN(); ++i_point) {
	  if (*(grStat->GetX()+i_point) < min_energy
	      || *(grStat->GetX()+i_point) > max_energy) {
	    grStat->RemovePoint(i_point);
	    --i_point;
	  }
	}

      }
      if (grStatAndSyst) {
	grStatAndSyst->Draw("PL2 SAME");
	grStatAndSysts[i_dataset][i_particle] = (TGraphAsymmErrors*) grStatAndSyst->Clone();
      }
      if (grStat) {
	grStat->Draw("PL SAME");
	grStats[i_dataset][i_particle] = (TGraphAsymmErrors*) grStat->Clone();
      }


      std::stringstream leglabel;
      leglabel.str("");
      leglabel << particle;
      legs[i_dataset]->AddEntry(grStatAndSyst, leglabel.str().c_str(), "l");
    }
      
    // if (!(dataset == "si16b" && arm == "SiL")) {
    legs[i_dataset]->Draw();
    // }
    //    infile->Close();
    //    pad->Update();
    //    TLatex* latex = new TLatex();
    //    latex->DrawLatexNDC(0.55, 0.80, "AlCap Preliminary");
  }
  c->cd();

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_DatasetComparison_FinalPlot";
    std::string pdfname = savename + ".pdf";
    c->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c->SaveAs(pngname.c_str());
  }

  for (int i_dataset = n_datasets-1; i_dataset >= 0; --i_dataset) {
    std::string dataset = datasets[i_dataset];
    std::string target = targets[i_dataset];
    std::cout << "TABLE:" << std::endl;
    table_file << "\\begin{table*}" << std::endl;
    //    table_file << "\\captionsetup{justification=centering}" << std::endl;
    table_file << "\\caption{Particle yields, $Y_{i}$, for particles emitted after nuclear muon capture on " << target << ". The energies quoted are the bin centers. The first error is statistical uncertainty and the second error is the systematic uncertainty.}" << std::endl;
    table_file << "\\label{tab:data-" << target << "}" << std::endl;
    table_file << "\\begin{center}" << std::endl;
    //    table_file << "\\begin{tabular}{ccccc}" << std::endl;
    table_file << "\\begin{tabular}{S[table-format=3.2]S[table-format=3.2]S[table-format=3.2]S[table-format=3.2]S[table-format=3.2]}" << std::endl;
    //      table_file << "\\multicolumn{2}{c}{\\textbf{" << target << ", " << particle << "}} \\\\" << std::endl;
    table_file << "\\hline" << std::endl;
    table_file << "\\hline" << std::endl;
    table_file << "\\multicolumn{1}{c}{\\multirow{2}{*}{E [MeV]}} ";
    for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
      std::string particle_init = particle_inits[i_particle];
      table_file << "& \\multicolumn{1}{c}{$Y_{" << particle_init << "}$}";
    }
    table_file << "\\\\" << std::endl;
    table_file << " & \\multicolumn{4}{c}{[per thousand muon captures per MeV]} \\\\" << std::endl;
    table_file << "\\hline" << std::endl;
    for (int i_point = 0; i_point < grStatAndSysts[0][0]->GetN(); ++i_point) {
      double E = *(grStatAndSysts[0][0]->GetX()+i_point);
      table_file << std::fixed << std::setprecision(2);
      table_file << E << " ";

      for (int i_particle = 0; i_particle < n_particles; ++i_particle) {
	std::string particle = particle_names[i_particle];
	std::string Particle = Particle_names[i_particle];
	std::string particle_init = particle_inits[i_particle];

	TGraphAsymmErrors* grStatAndSyst = grStatAndSysts[i_dataset][i_particle];
	TGraphAsymmErrors* grStat = grStats[i_dataset][i_particle];
	for (int j_point = 0; j_point < grStat->GetN(); ++j_point) {
	  double this_E = *(grStatAndSyst->GetX()+j_point);
	  if (this_E == E) {
	    //	double dE = *(grStatAndSyst->GetEX()+j_point);
	    double rate = *(grStatAndSyst->GetY()+j_point) * 1e3;
	    double stat_error = *(grStat->GetEYhigh()+j_point) * 1e3;
	    double statsyst_error = *(grStatAndSyst->GetEYhigh()+j_point) * 1e3;
	    double syst_error = std::sqrt(statsyst_error*statsyst_error - stat_error*stat_error);
	    table_file << std::setprecision(2) << " &~~$" << rate
		       << " \\pm " << stat_error 
		       << " \\pm " << syst_error << "$~~";
	    //"~\\text{(stat.)}" // "~\\text{(syst.)}$ ";
	    break;
	  }
	  else if (j_point == grStat->GetN()-1) { {
	      table_file << "& \\multicolumn{1}{c}{no result} ";
	    }
	  }
	}
      }
      table_file << "\\\\" << std::endl;
    }
    table_file << "\\end{tabular}" << std::endl;
    table_file << "\\end{center}" << std::endl;
    table_file << "\\end{table*}" << std::endl;
    table_file << std::endl;
  }
  table_file.close();

  //  c->SaveAs("~/plots/2021-07-09/FinalPlots.pdf");
}
