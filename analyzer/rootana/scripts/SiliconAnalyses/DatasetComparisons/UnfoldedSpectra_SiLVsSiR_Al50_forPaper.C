void UnfoldedSpectra_SiLVsSiR_Al50_forPaper(std::string savedir = "") {

  gStyle->SetOptStat(0);

  // Canvas setup
  TCanvas* c = new TCanvas();//"c", "c",500,700);
  //  TCanvas* c = new TCanvas("c", "c",500*2,700);
  //  c->SetLogy();
  c->SetFillStyle(4000);

  const int n_arms = 2;
  std::string arms[n_arms] = {"SiL", "SiR"};
  std::string arm_labels[n_arms] = {"left detector package", "right detector package"};
  Int_t colours[n_arms] = {kBlue, kRed};

  std::string dataset = "al50";
  std::string particle = "proton";

  TLegend* leg = new TLegend(0.5, 0.5, 0.8, 0.8);
  
  TH1F* hCurrentYAxisHist = 0;  
  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {    
    std::string infilename = "/data/ssd3/R15bAnalysis/hists/" + dataset + "-unfolded.root";
    TFile* infile = new TFile(infilename.c_str(), "READ");

    std::stringstream axistitle;
    int rebin_factor = 1;

    std::string arm = arms[i_arm];
    std::string arm_label = arm_labels[i_arm];

    Int_t colour = colours[i_arm];

    std::string histname = "h";
    histname += particle;
    histname += "_" + arm;

    // TH1F* hFoldedSpectrum = new TH1F("h", "", 100,0,5);
    // hFoldedSpectrum->FillRandom("gaus",10000);
    // hFoldedSpectrum->GetXaxis()->SetTitle("x axis");
    // hFoldedSpectrum->GetYaxis()->SetTitle("y axis");
    std::cout << dataset << ": " << histname << std::endl;
    TH1F* hFoldedSpectrum = (TH1F*) infile->Get(histname.c_str());
    if (hFoldedSpectrum) {
      //	  continue;
      //	}
      //      hFoldedSpectrum->SetDirectory(0);
      hFoldedSpectrum->Rebin(rebin_factor);
      axistitle.str("");
      axistitle << "Unfolded Count / " << hFoldedSpectrum->GetXaxis()->GetBinWidth(1) << " MeV";
      //    hFoldedSpectrum->SetTitle("Si16b Dataset, Right Arm, Raw Spectrum");
      hFoldedSpectrum->SetXTitle("Unfolded Energy [MeV]");
      hFoldedSpectrum->SetYTitle(axistitle.str().c_str());
      hFoldedSpectrum->SetStats(false);
      hFoldedSpectrum->SetLineColor(colour);
      hFoldedSpectrum->SetLineWidth(2);
      //      hFoldedSpectrum->GetXaxis()->SetRangeUser(0, 20);
      //      hFoldedSpectrum->SetMinimum(0.5);
      double min_integral_energy = 3.5;
      double max_integral_energy = 10;
      int min_bin = hFoldedSpectrum->GetXaxis()->FindBin(min_integral_energy);
      int max_bin = hFoldedSpectrum->GetXaxis()->FindBin(max_integral_energy);
      std::cout << "Integral = " << hFoldedSpectrum->Integral(min_bin,max_bin) << std::endl;
	  
    }

    if (i_arm == 0) {
      if (hFoldedSpectrum) {
	hCurrentYAxisHist = (TH1F*) hFoldedSpectrum->Clone("hCurrentYAxisHist"); // keep track of the y-axis so that left and right match)
      }
      TH1F *hFrame;
      char hname[10] = "Frame";
      if (hFoldedSpectrum) {
	hFrame = (TH1F*) hFoldedSpectrum->Clone(hname);
      }
      else {
	hFrame = (TH1F*) hCurrentYAxisHist->Clone(hname);
      }
      hFrame->Reset();
      hFrame->SetXTitle("Energy [MeV]");
      hFrame->SetYTitle("Unfolded Count / MeV");
      hFrame->Draw();
      //	hFrame->GetXaxis()->ChangeLabel(1,-1,0.); // remove the first label
      //      hFrame->GetXaxis()->ChangeLabel(-1,-1,0.); // remove the last label
	
      // y axis range
      hFrame->GetXaxis()->SetRangeUser(0, 15);
      //	  hFrame->GetYaxis()->SetRangeUser(0.5,10*hFoldedSpectrum->GetMaximum());
      hFrame->GetYaxis()->SetRangeUser(0,1.1*hCurrentYAxisHist->GetMaximum());

      // Format for y axis
      hFrame->GetYaxis()->SetLabelFont(43);
      hFrame->GetYaxis()->SetLabelSize(17);
      hFrame->GetYaxis()->SetLabelOffset(0.02);
      hFrame->GetYaxis()->SetTitleFont(43);
      hFrame->GetYaxis()->SetTitleSize(18);
      hFrame->GetYaxis()->SetTitleOffset(1.2);
	
      //      hFrame->GetYaxis()->CenterTitle();
      hFrame->GetYaxis()->SetNdivisions(505);

      // Format for x axis
      hFrame->GetXaxis()->SetLabelFont(43);
      hFrame->GetXaxis()->SetLabelSize(18);
      hFrame->GetXaxis()->SetLabelOffset(0.02);
      hFrame->GetXaxis()->SetTitleFont(43);
      hFrame->GetXaxis()->SetTitleSize(18);
      hFrame->GetXaxis()->SetTitleOffset(1.2);
      //      hFrame->GetXaxis()->CenterTitle();
      hFrame->GetXaxis()->SetNdivisions(505);

      // TLine* line = new TLine();
      // line->SetLineColor(kBlack);
      // line->SetLineStyle(kDashed);
      // double y = 
      // std::cout << "Bot. Marg. = " << pad[i][j]->GetBottomMargin() << std::endl;
      // std::cout << "Top Marg. = " << pad[i][j]->GetTopMargin() << std::endl;
      // //	std::cout << "AbsHNDC = " << pad[i][j]->GetHNDC() << std::endl;
      // std::cout << "y = " << y << std::endl;
      // line->DrawLineNDC(0.0, y, 1.0, y);
	
      leg->SetBorderSize(0);
      leg->SetTextFont(43);
      leg->SetTextSize(16);
      leg->SetFillColor(kWhite);
      std::string legheader = "AlCap Al (stat. errors only)";// ", " + arm;
      leg->SetHeader(legheader.c_str());
      //	std::cout << "New TLegend: " << leg << std::endl;
      //	std::cout << "Set Header: " << legheader << std::endl;
      //	std::cout << "Set Text Size: " << leg->GetTextSize() << std::endl;

    }	

    if (hFoldedSpectrum) {
      hFoldedSpectrum->Draw("HIST E SAME");
    }

    std::stringstream leglabel;
    leglabel.str("");
    leglabel << arm_label;
    leg->AddEntry(hFoldedSpectrum, leglabel.str().c_str(), "l");
  }

  leg->Draw();
  c->cd();

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_UnfoldedSpectra_SiLVsSiR_Al50";
    std::string pdfname = savename + ".pdf";
    c->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c->SaveAs(pngname.c_str());
  }
}
