#include "CanvasPartition.h"

void RawSpectra_forPaper(std::string savedir = "") {

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

  const int n_arms = 1;//2;
  std::string arms[n_arms] = {"SiR"};//{"SiL", "SiR"};
  
  const int n_particles = 4;
  std::string particle_names[n_particles] = {"proton", "deuteron", "triton", "alpha"};
  Int_t colours[n_particles] = {kRed, kBlue, kMagenta, kGreen+3};

  TPad *pad[Nx][Ny];

  TH1F* hCurrentYAxisHist = 0;
  for (int i_dataset = 0; i_dataset < n_datasets; ++i_dataset) {    
    std::string dataset = datasets[i_dataset];
    std::string target = targets[i_dataset];
    std::string infilename = "/data/ssd3/R15bAnalysis/hists/" + dataset + "-folded.root";
    TFile* infile = new TFile(infilename.c_str(), "READ");

    std::stringstream axistitle;
    int rebin_factor = 1;

    for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
      std::string arm = arms[i_arm];

      for (int i_particle = 0; i_particle < n_particles; ++i_particle) {

	std::string particle = particle_names[i_particle];
	Int_t colour = colours[i_particle];

	std::string histname = "h";
	histname += particle[0];
	histname += arm;

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
	  axistitle << "Raw Count / " << hFoldedSpectrum->GetXaxis()->GetBinWidth(1) << " MeV";
	  //    hFoldedSpectrum->SetTitle("Si16b Dataset, Right Arm, Raw Spectrum");
	  hFoldedSpectrum->SetXTitle("Raw Energy [MeV]");
	  hFoldedSpectrum->SetYTitle(axistitle.str().c_str());
	  hFoldedSpectrum->SetStats(false);
	  hFoldedSpectrum->SetLineColor(colour);
	  hFoldedSpectrum->SetLineWidth(2);
	  //      hFoldedSpectrum->GetXaxis()->SetRangeUser(0, 20);
	  //      hFoldedSpectrum->SetMinimum(0.5);
	}
    


	c->cd(0);
	// Get the pads previously created.
	char pname[16];
	int i = i_arm;
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
	  std::cout << "xFactor, yFactor = " << xFactor << ", " << yFactor << std::endl;
	  char hname[16];
	  sprintf(hname,"h_%i_%i",i,j);
	  if (i==0 && hFoldedSpectrum) {
	    hCurrentYAxisHist = (TH1F*) hFoldedSpectrum->Clone("hCurrentYAxisHist"); // keep track of the y-axis so that left and right match)
	  }
	  TH1F *hFrame;
	  if (hFoldedSpectrum) {
	    hFrame = (TH1F*) hFoldedSpectrum->Clone(hname);
	  }
	  else {
	    hFrame = (TH1F*) hCurrentYAxisHist->Clone(hname);
	  }
	  hFrame->Reset();
	  hFrame->Draw();
	  //	hFrame->GetXaxis()->ChangeLabel(1,-1,0.); // remove the first label
	  hFrame->GetXaxis()->ChangeLabel(-1,-1,0.); // remove the last label

	  // y axis range
	  hFrame->GetXaxis()->SetRangeUser(0, 20);
	  //	  hFrame->GetYaxis()->SetRangeUser(0.5,10*hFoldedSpectrum->GetMaximum());
	  hFrame->GetYaxis()->SetRangeUser(0.5,10*hCurrentYAxisHist->GetMaximum());

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
	  std::cout << leg_x1 << ", " << leg_y1 << ", " << leg_x2 << ", " << leg_y2 << std::endl;
	  //	legs[i_dataset]->ConvertNDCtoPad();
	  //	std::cout << pad[i][j]->GetXlowNDC()<< ", " << pad[i][j]->GetYlowNDC()<< ", " << pad[i][j]->GetXlowNDC()<< ", " << pad[i][j]->GetYlowNDC() <<std::endl;
	  //	std::cout << pad[i][j]->GetX1()<< ", " << pad[i][j]->GetY1()<< ", " << pad[i][j]->GetX2()<< ", " << pad[i][j]->GetY2() <<std::endl;
	  legs[i_dataset]->SetBorderSize(0);
	  legs[i_dataset]->SetTextFont(43);
	  legs[i_dataset]->SetTextSize(12);
	  legs[i_dataset]->SetFillColor(kWhite);
	  std::string legheader = "AlCap " + target + ", " + arm;
	  legs[i_dataset]->SetHeader(legheader.c_str());
	  //	std::cout << "New TLegend: " << leg << std::endl;
	  //	std::cout << "Set Header: " << legheader << std::endl;
	  //	std::cout << "Set Text Size: " << leg->GetTextSize() << std::endl;

	}	

	if (hFoldedSpectrum) {
	  hFoldedSpectrum->Draw("HIST E SAME");
	}
	
	std::stringstream leglabel;
	leglabel.str("");
	leglabel << particle;
	legs[i_dataset]->AddEntry(hFoldedSpectrum, leglabel.str().c_str(), "l");
      }

      if (!(dataset == "si16b" && arm == "SiL")) {
	legs[i_dataset]->Draw();
      }
      //    infile->Close();
      //    pad->Update();
      //    TLatex* latex = new TLatex();
      //    latex->DrawLatexNDC(0.55, 0.80, "AlCap Preliminary");
    }
  }
  c->cd();

  if (savedir != "") {
    std::string savename = savedir + "AlCapData_AllRawSpectra";
    std::string pdfname = savename + ".pdf";
    c->SaveAs(pdfname.c_str());
    std::string pngname = savename + ".png";
    c->SaveAs(pngname.c_str());
  }

}
