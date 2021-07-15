void Lifetime(const char *target = "al50", const char *particle = "proton", const char *arm= "SiR") {
	ROOT::Math::MinimizerOptions::SetDefaultMaxFunctionCalls(10000);
	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit");
	const Int_t min = 400;
	const Int_t max = 5000;
	const Int_t nbins = (max-min)/40;
	TFile *fData = new TFile(Form("%s/%s.root", getenv("R15b_DATA"), target), "READ");
	Double_t e3, t1, t2, timeToPrevTME, timeToNextTME;
	TString *channel = new TString("");
	TString *sig2 = new TString("");
	TH1D *hpSiL = new TH1D("hpSiL", ";;Counts/40ns", nbins, min, max);
	TH1D *hpSiR = new TH1D("hpSiR", ";t_{2}[ns];Counts/40ns", nbins, min, max);
	TH1D *hdSiL = new TH1D("hdSiL", ";;Counts/40ns", nbins, min, max);
	TH1D *hdSiR = new TH1D("hdSiR", ";;Counts/40ns", nbins, min, max);
	TH1D *htSiL = new TH1D("htSiL", ";;Counts/40ns", nbins, min, max);
	TH1D *htSiR = new TH1D("htSiR", ";;Counts/40ns", nbins, min, max);
	TH1D *haSiL = new TH1D("haSiL", ";;Counts/40ns", nbins, min, max);
	TH1D *haSiR = new TH1D("haSiR", ";;Counts/40ns", nbins, min, max);
	TTree *tree = (TTree *)fData->Get("tree");
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("t2", &t2);
	tree->SetBranchAddress("e3", &e3);
	tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
	tree->SetBranchAddress("channel", &channel);
	tree->SetBranchAddress("sig2", &sig2);
	for(Long64_t i=0; i < tree->GetEntries(); i++) {
		tree->GetEntry(i);
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		if(abs(t2-t1) > 500) continue;
		if(t2>10e3) continue;
//		if(!TMath::IsNaN(e3) ) continue;
		if(sig2->Contains("proton") ) {
			if(channel->Contains("SiL") ) {
				hpSiL->Fill(t2);
			} else if(channel->Contains("SiR") ) {
				hpSiR->Fill(t2);
			}
		} else if(sig2->Contains("deuteron") ) {
			if(channel->Contains("SiL") ) {
				hdSiL->Fill(t2);
			} else if(channel->Contains("SiR") ) {
				hdSiR->Fill(t2);
			}
		} else if(sig2->Contains("triton") ) {
			if(channel->Contains("SiL") ) {
				htSiL->Fill(t2);
			} else if(channel->Contains("SiR") ) {
				htSiR->Fill(t2);
			}
		} else if (sig2->Contains("alpha") ) {
			if(channel->Contains("SiL") ) {
				haSiL->Fill(t2);
			} else if(channel->Contains("SiR") ) {
				haSiR->Fill(t2);
			}
		}
	}
	if(strcmp(arm, "SiR") == 0) {
		if(strcmp(particle, "proton") == 0 ) {
			TF1 *fit = new TF1("fit", "expo(0)+pol0(2)", min, max);
			fit->SetLineColor(kRed);
			fit->SetParameter(0, 4.31791e+00);
			fit->SetParLimits(1, fit->GetParameter(1)*1.1, fit->GetParameter(1)*0.9);
			fit->SetParameter(2, 8.51382e-01);
			fit->SetParLimits(2, 0, 10);
			hpSiR->Fit("fit", "SR+");
			Double_t par[3];
			fit->GetParameters(par);
			TH1D *hpSiRRes = new TH1D("hpSiRRes", "Lifetime Residual;t[ns];#frac{Entries-Fit}{Entries}[%]", nbins, min, max);
			for(int i=1; i<hpSiR->GetSize()- 1; ++i) {
				if(hpSiR->GetBinContent(i) > 0) {
					Double_t res = 100 * (hpSiR->GetBinContent(i) - fit->Eval(min+i*40-40/2) )/hpSiR->GetBinContent(i);
					hpSiRRes->SetBinContent(i-1, res);
				}
			}
			TCanvas *pc = new TCanvas("pr", "pr");
//			TPad *p1 = new TPad("p1", "p1",0,0.3,1,1, kWhite, 0, 0);
//			p1->SetBottomMargin(0.05);
//			p1->Draw();
//			TPad *p2 = new TPad("p2", "p2",0,0.0,1,0.3 , kWhite, 0, 0);
//			p2->SetTopMargin(0.00001);
//			p2->SetBottomMargin(0.2);
//			p2->Draw();
//
//			p1->cd();
			gPad->SetLogy();
			hpSiR->Draw("E"); hpSiR->SetMarkerStyle(kCircle);
//			hpSiR->GetXaxis()->SetLabelSize(0);
//			hpSiR->GetYaxis()->SetMaxDigits(3);
//			hpSiR->GetYaxis()->SetTitleSize(.05);
//			hpSiR->GetYaxis()->SetTitleOffset(.8);
//			hpSiR->GetYaxis()->SetLabelFont(63);
//			hpSiR->GetYaxis()->SetLabelSize(16);

			TLegend *legend = new TLegend(0.522923, 0.66879, 0.848138, 0.861996);
			legend->SetHeader("AlCap Al 50#mum proton");
			legend->AddEntry(hpSiR, "Data", "pe");
			legend->AddEntry(fit, Form("#tau_{#muAl} %.2f#pm%.2f ns", -1./par[1], 1./(par[1]*par[1]) * fit->GetParError(1) ), "l");
			legend->Draw();
//			p2->cd();
//			p2->SetGridx();
//			p2->SetGridy();
//			hpSiRRes->Draw();
//			hpSiRRes->GetXaxis()->SetMaxDigits(3);
//			hpSiRRes->GetXaxis()->SetLabelFont(63);
//			hpSiRRes->GetXaxis()->SetLabelSize(15);
//			hpSiRRes->GetXaxis()->SetTitleSize(.1);
//			hpSiRRes->GetXaxis()->SetTitleOffset(.7);
//			hpSiRRes->SetFillColor(38);
//			hpSiRRes->SetLineColor(kWhite);
//			hpSiRRes->GetYaxis()->SetNdivisions(10, 2, 0);
//			hpSiRRes->GetYaxis()->SetLabelFont(63);
//			hpSiRRes->GetYaxis()->SetLabelSize(16);
//			hpSiRRes->GetYaxis()->SetTitleSize(.1);
//			hpSiRRes->GetYaxis()->SetTitleOffset(.4);
//			hpSiRRes->GetYaxis()->CenterTitle();
//			pc->cd();
			const char *FigsDir = getenv("R15b_OUT");
			pc->SaveAs(Form("%s/AlCapData_Al50Dataset_%sTime_RightArm.pdf", FigsDir, particle) );
		} else if(strcmp(particle, "deuteron") == 0) {
			TF1 *fit = new TF1("fit", "expo(0)+pol0(2)", min, max);
			fit->SetLineColor(kBlue);
			fit->SetParameter(0, 4.31791e+00);
			fit->SetParLimits(1, fit->GetParameter(1)*1.1, fit->GetParameter(1)*0.9);
			fit->SetParameter(2, 8.51382e-01);
			fit->SetParLimits(2, 0, 10);
			hdSiR->Fit("fit", "SR+");
			Double_t par[3];
			fit->GetParameters(par);
			TH1D *hdSiRRes = new TH1D("hdSiRRes", "Lifetime Residual;t[ns];#frac{Entries-Fit}{Entries}[%]", nbins, min, max);
			for(int i=1; i<hdSiR->GetSize()- 1; ++i) {
				if(hdSiR->GetBinContent(i) > 0) {
					Double_t res = 100 * (hdSiR->GetBinContent(i) - fit->Eval(min+i*40-40/2) )/hdSiR->GetBinContent(i);
					hdSiRRes->SetBinContent(i-1, res);
				}
			}
			TCanvas *dc = new TCanvas("dr", "dr");
			TPad *dp1 = new TPad("dp1", "dp1",0,0.3,1,1, kWhite, 0, 0);
			dp1->SetBottomMargin(0.05);
			dp1->Draw();
			TPad *dp2 = new TPad("dp2", "dp2",0,0.0,1,0.3 , kWhite, 0, 0);
			dp2->SetTopMargin(0.00001);
			dp2->SetBottomMargin(0.2);
			dp2->Draw();

			dp1->cd();
			gPad->SetLogy();
			hdSiR->Draw("E1");
			hdSiR->GetXaxis()->SetLabelSize(0);
			hdSiR->GetYaxis()->SetMaxDigits(3);
			hdSiR->GetYaxis()->SetTitleSize(.05);
			hdSiR->GetYaxis()->SetTitleOffset(.8);
			hdSiR->GetYaxis()->SetLabelFont(63);
			hdSiR->GetYaxis()->SetLabelSize(16);

			TLegend *legend = new TLegend(0.522923, 0.66879, 0.848138, 0.861996);
			legend->SetHeader("#bf{AlCap} #it{Preliminary}");
			legend->AddEntry("", "50#mum (Right 2#sigma)","");
			legend->AddEntry(hdSiR, "deuteron", "e");
			legend->AddEntry(fit, Form("#tau_{#muAl} %.2f#pm%.2f ns", -1./par[1], 1./(par[1]*par[1]) * fit->GetParError(1) ), "l");
			legend->Draw();
			dp2->cd();
			dp2->SetGridx();
			dp2->SetGridy();
			hdSiRRes->Draw();
			hdSiRRes->GetXaxis()->SetMaxDigits(3);
			hdSiRRes->GetXaxis()->SetLabelFont(63);
			hdSiRRes->GetXaxis()->SetLabelSize(15);
			hdSiRRes->GetXaxis()->SetTitleSize(.1);
			hdSiRRes->GetXaxis()->SetTitleOffset(.7);
			hdSiRRes->SetFillColor(38);
			hdSiRRes->SetLineColor(kWhite);
			hdSiRRes->GetYaxis()->SetNdivisions(10, 2, 0);
			hdSiRRes->GetYaxis()->SetLabelFont(63);
			hdSiRRes->GetYaxis()->SetLabelSize(16);
			hdSiRRes->GetYaxis()->SetTitleSize(.1);
			hdSiRRes->GetYaxis()->SetTitleOffset(.4);
			hdSiRRes->GetYaxis()->CenterTitle();
			dc->cd();
			const char *FigsDir = getenv("R15b_OUT");
			dc->SaveAs(Form("%s/AlCapData_Al50Dataset_%sTime_RightArm.pdf", FigsDir, particle) );
		} else if(strcmp(particle, "triton") == 0) {
			TF1 *fit = new TF1("fit", "expo(0)+pol0(2)", min, max);
			fit->SetLineColor(kBlue);
			fit->SetParameter(0, 4.31791e+00);
			fit->SetParLimits(1, fit->GetParameter(1)*1.1, fit->GetParameter(1)*0.9);
			fit->SetParameter(2, 8.51382e-01);
			fit->SetParLimits(2, 0, 10);
			htSiR->Fit("fit", "SR+");
			Double_t par[3];
			fit->GetParameters(par);
			TH1D *htSiRRes = new TH1D("htSiRRes", "Lifetime Residual;t[ns];#frac{Entries-Fit}{Entries}[%]", nbins, min, max);
			for(int i=1; i<htSiR->GetSize()- 1; ++i) {
				if(htSiR->GetBinContent(i) > 0) {
					Double_t res = 100 * (htSiR->GetBinContent(i) - fit->Eval(min+i*40-40/2) )/htSiR->GetBinContent(i);
					htSiRRes->SetBinContent(i-1, res);
				}
			}
			TCanvas *tc = new TCanvas("tr", "tr");
			TPad *tp1 = new TPad("tp1", "tp1",0,0.3,1,1, kWhite, 0, 0);
			tp1->SetBottomMargin(0.05);
			tp1->Draw();
			TPad *tp2 = new TPad("tp2", "tp2",0,0.0,1,0.3 , kWhite, 0, 0);
			tp2->SetTopMargin(0.00001);
			tp2->SetBottomMargin(0.2);
			tp2->Draw();

			tp1->cd();
			gPad->SetLogy();
			htSiR->Draw("E1");
			htSiR->GetXaxis()->SetLabelSize(0);
			htSiR->GetYaxis()->SetMaxDigits(3);
			htSiR->GetYaxis()->SetTitleSize(.05);
			htSiR->GetYaxis()->SetTitleOffset(.8);
			htSiR->GetYaxis()->SetLabelFont(63);
			htSiR->GetYaxis()->SetLabelSize(16);

			TLegend *legend = new TLegend(0.522923, 0.66879, 0.848138, 0.861996);
			legend->SetHeader("#bf{AlCap} #it{Preliminary}");
			legend->AddEntry("", "50#mum (Right 2#sigma)","");
			legend->AddEntry(htSiR, "triton", "e");
			legend->AddEntry(fit, Form("#tau_{#muAl} %.2f#pm%.2f ns", -1./par[1], 1./(par[1]*par[1]) * fit->GetParError(1) ), "l");
			legend->Draw();
			tp2->cd();
			tp2->SetGridx();
			tp2->SetGridy();
			htSiRRes->Draw();
			htSiRRes->GetXaxis()->SetMaxDigits(3);
			htSiRRes->GetXaxis()->SetLabelFont(63);
			htSiRRes->GetXaxis()->SetLabelSize(15);
			htSiRRes->GetXaxis()->SetTitleSize(.1);
			htSiRRes->GetXaxis()->SetTitleOffset(.7);
			htSiRRes->SetFillColor(38);
			htSiRRes->SetLineColor(kWhite);
			htSiRRes->GetYaxis()->SetNdivisions(10, 2, 0);
			htSiRRes->GetYaxis()->SetLabelFont(63);
			htSiRRes->GetYaxis()->SetLabelSize(16);
			htSiRRes->GetYaxis()->SetTitleSize(.1);
			htSiRRes->GetYaxis()->SetTitleOffset(.4);
			htSiRRes->GetYaxis()->CenterTitle();
			tc->cd();
			const char *FigsDir = getenv("R15b_OUT");
			tc->SaveAs(Form("%s/AlCapData_Al50Dataset_%sTime_RightArm.pdf", FigsDir, particle) );
		} else if(strcmp(particle, "alpha") == 0) {
			TF1 *fit = new TF1("fit", "expo(0)+pol0(2)", min, max);
			fit->SetLineColor(kBlue);
			fit->SetParameter(0, 4.31791e+00);
			fit->SetParLimits(1, fit->GetParameter(1)*1.1, fit->GetParameter(1)*0.9);
			fit->SetParameter(2, 8.51382e-01);
			fit->SetParLimits(2, 0, 10);
			haSiR->Fit("fit", "SR+");
			Double_t par[3];
			fit->GetParameters(par);
			TH1D *haSiRRes = new TH1D("haSiRRes", "Lifetime Residual;t[ns];#frac{Entries-Fit}{Entries}[%]", nbins, min, max);
			for(int i=1; i<haSiR->GetSize()- 1; ++i) {
				if(haSiR->GetBinContent(i) > 0) {
					Double_t res = 100 * (haSiR->GetBinContent(i) - fit->Eval(min+i*40-40/2) )/haSiR->GetBinContent(i);
					haSiRRes->SetBinContent(i-1, res);
				}
			}
			TCanvas *ac = new TCanvas("ar", "ar");
			TPad *ap1 = new TPad("ap1", "ap1",0,0.3,1,1, kWhite, 0, 0);
			ap1->SetBottomMargin(0.05);
			ap1->Draw();
			TPad *ap2 = new TPad("ap2", "ap2",0,0.0,1,0.3 , kWhite, 0, 0);
			ap2->SetTopMargin(0.00001);
			ap2->SetBottomMargin(0.2);
			ap2->Draw();

			ap1->cd();
			gPad->SetLogy();
			haSiR->Draw("E1");
			haSiR->GetXaxis()->SetLabelSize(0);
			haSiR->GetYaxis()->SetMaxDigits(3);
			haSiR->GetYaxis()->SetTitleSize(.05);
			haSiR->GetYaxis()->SetTitleOffset(.8);
			haSiR->GetYaxis()->SetLabelFont(63);
			haSiR->GetYaxis()->SetLabelSize(16);

			TLegend *legend = new TLegend(0.522923, 0.66879, 0.848138, 0.861996);
			legend->SetHeader("#bf{AlCap} #it{Preliminary}");
			legend->AddEntry("", "50#mum (Right 2#sigma)","");
			legend->AddEntry(haSiR, "alpha", "e");
			legend->AddEntry(fit, Form("#tau_{#muAl} %.2f#pm%.2f ns", -1./par[1], 1./(par[1]*par[1]) * fit->GetParError(1) ), "l");
			legend->Draw();
			ap2->cd();
			ap2->SetGridx();
			ap2->SetGridy();
			haSiRRes->Draw();
			haSiRRes->GetXaxis()->SetMaxDigits(3);
			haSiRRes->GetXaxis()->SetLabelFont(63);
			haSiRRes->GetXaxis()->SetLabelSize(15);
			haSiRRes->GetXaxis()->SetTitleSize(.1);
			haSiRRes->GetXaxis()->SetTitleOffset(.7);
			haSiRRes->SetFillColor(38);
			haSiRRes->SetLineColor(kWhite);
			haSiRRes->GetYaxis()->SetNdivisions(10, 2, 0);
			haSiRRes->GetYaxis()->SetLabelFont(63);
			haSiRRes->GetYaxis()->SetLabelSize(16);
			haSiRRes->GetYaxis()->SetTitleSize(.1);
			haSiRRes->GetYaxis()->SetTitleOffset(.4);
			haSiRRes->GetYaxis()->CenterTitle();
			ac->cd();
			const char *FigsDir = getenv("R15b_OUT");
			ac->SaveAs(Form("%s/AlCapData_Al50Dataset_%sTime_RightArm.pdf", FigsDir, particle) );
		}
	} else if(strcmp(arm, "SiL") == 0) {
		if(strcmp(particle, "proton") == 0 ) {
			TF1 *fit = new TF1("fit", "expo(0)+pol0(2)", min, max);
			fit->SetLineColor(kBlue);
			fit->SetParameter(0, 7.31791e+00);
			fit->SetParLimits(1, fit->GetParameter(1)*1.1, fit->GetParameter(1)*0.9);
			fit->SetParameter(2, 4.51382e-01);
			fit->SetParLimits(2, 0, 10);
			hpSiL->Fit("fit", "SR+");
			Double_t par[3];
			fit->GetParameters(par);
			TH1D *hpSiLRes = new TH1D("hpSiLRes", "Lifetime Residual;t[ns];#frac{Entries-Fit}{Entries}[%]", nbins, min, max);
			for(int i=1; i<hpSiL->GetSize()- 1; ++i) {
				if(hpSiL->GetBinContent(i) > 0) {
					Double_t res = 100 * (hpSiL->GetBinContent(i) - fit->Eval(min+i*40-40/2) )/hpSiL->GetBinContent(i);
					hpSiLRes->SetBinContent(i-1, res);
				}
			}
			TCanvas *pc = new TCanvas("pr", "pr");
			TPad *p1 = new TPad("p1", "p1",0,0.3,1,1, kWhite, 0, 0);
			p1->SetBottomMargin(0.05);
			p1->Draw();
			TPad *p2 = new TPad("p2", "p2",0,0.0,1,0.3 , kWhite, 0, 0);
			p2->SetTopMargin(0.00001);
			p2->SetBottomMargin(0.2);
			p2->Draw();

			p1->cd();
			gPad->SetLogy();
			hpSiL->Draw("E1");
			hpSiL->GetXaxis()->SetLabelSize(0);
			hpSiL->GetYaxis()->SetMaxDigits(3);
			hpSiL->GetYaxis()->SetTitleSize(.05);
			hpSiL->GetYaxis()->SetTitleOffset(.8);
			hpSiL->GetYaxis()->SetLabelFont(63);
			hpSiL->GetYaxis()->SetLabelSize(16);

			TLegend *legend = new TLegend(0.522923, 0.66879, 0.848138, 0.861996);
			legend->SetHeader("#bf{AlCap} #it{Preliminary}");
			legend->AddEntry("", "50#mum (Left 2#sigma)","");
			legend->AddEntry(hpSiL, "proton", "e");
			legend->AddEntry(fit, Form("#tau_{#muAl} %.2f#pm%.2f ns", -1./par[1], 1./(par[1]*par[1]) * fit->GetParError(1) ), "l");
			legend->Draw();
			p2->cd();
			p2->SetGridx();
			p2->SetGridy();
			hpSiLRes->Draw();
			hpSiLRes->GetXaxis()->SetMaxDigits(3);
			hpSiLRes->GetXaxis()->SetLabelFont(63);
			hpSiLRes->GetXaxis()->SetLabelSize(15);
			hpSiLRes->GetXaxis()->SetTitleSize(.1);
			hpSiLRes->GetXaxis()->SetTitleOffset(.7);
			hpSiLRes->SetFillColor(38);
			hpSiLRes->SetLineColor(kWhite);
			hpSiLRes->GetYaxis()->SetNdivisions(10, 2, 0);
			hpSiLRes->GetYaxis()->SetLabelFont(63);
			hpSiLRes->GetYaxis()->SetLabelSize(16);
			hpSiLRes->GetYaxis()->SetTitleSize(.1);
			hpSiLRes->GetYaxis()->SetTitleOffset(.4);
			hpSiLRes->GetYaxis()->CenterTitle();
			pc->cd();
			const char *FigsDir = getenv("R15b_OUT");
			pc->SaveAs(Form("%s/AlCapData_Al50Dataset_%sTime_LeftArm.pdf", FigsDir, particle) );
		} else if(strcmp(particle, "deuteron") == 0) {
			TF1 *fit = new TF1("fit", "expo(0)+pol0(2)", min, max);
			fit->SetLineColor(kBlue);
			fit->SetParameter(0, 7.31791e+00);
			fit->SetParLimits(1, fit->GetParameter(1)*1.1, fit->GetParameter(1)*0.9);
			fit->SetParameter(2, 4.51382e-01);
			fit->SetParLimits(2, 0, 10);
			hdSiL->Fit("fit", "SR+");
			Double_t par[3];
			fit->GetParameters(par);
			TH1D *hdSiLRes = new TH1D("hdSiLRes", "Lifetime Residual;t[ns];#frac{Entries-Fit}{Entries}[%]", nbins, min, max);
			for(int i=1; i<hdSiL->GetSize()- 1; ++i) {
				if(hdSiL->GetBinContent(i) > 0) {
					Double_t res = 100 * (hdSiL->GetBinContent(i) - fit->Eval(min+i*40-40/2) )/hdSiL->GetBinContent(i);
					hdSiLRes->SetBinContent(i-1, res);
				}
			}
			TCanvas *dc = new TCanvas("dr", "dr");
			TPad *dp1 = new TPad("dp1", "dp1",0,0.3,1,1, kWhite, 0, 0);
			dp1->SetBottomMargin(0.05);
			dp1->Draw();
			TPad *dp2 = new TPad("dp2", "dp2",0,0.0,1,0.3 , kWhite, 0, 0);
			dp2->SetTopMargin(0.00001);
			dp2->SetBottomMargin(0.2);
			dp2->Draw();

			dp1->cd();
			gPad->SetLogy();
			hdSiL->Draw("E1");
			hdSiL->GetXaxis()->SetLabelSize(0);
			hdSiL->GetYaxis()->SetMaxDigits(3);
			hdSiL->GetYaxis()->SetTitleSize(.05);
			hdSiL->GetYaxis()->SetTitleOffset(.8);
			hdSiL->GetYaxis()->SetLabelFont(63);
			hdSiL->GetYaxis()->SetLabelSize(16);

			TLegend *legend = new TLegend(0.522923, 0.66879, 0.848138, 0.861996);
			legend->SetHeader("#bf{AlCap} #it{Preliminary}");
			legend->AddEntry("", "50#mum (Left 2#sigma)","");
			legend->AddEntry(hdSiL, "deuteron", "e");
			legend->AddEntry(fit, Form("#tau_{#muAl} %.2f#pm%.2f ns", -1./par[1], 1./(par[1]*par[1]) * fit->GetParError(1) ), "l");
			legend->Draw();
			dp2->cd();
			dp2->SetGridx();
			dp2->SetGridy();
			hdSiLRes->Draw();
			hdSiLRes->GetXaxis()->SetMaxDigits(3);
			hdSiLRes->GetXaxis()->SetLabelFont(63);
			hdSiLRes->GetXaxis()->SetLabelSize(15);
			hdSiLRes->GetXaxis()->SetTitleSize(.1);
			hdSiLRes->GetXaxis()->SetTitleOffset(.7);
			hdSiLRes->SetFillColor(38);
			hdSiLRes->SetLineColor(kWhite);
			hdSiLRes->GetYaxis()->SetNdivisions(10, 2, 0);
			hdSiLRes->GetYaxis()->SetLabelFont(63);
			hdSiLRes->GetYaxis()->SetLabelSize(16);
			hdSiLRes->GetYaxis()->SetTitleSize(.1);
			hdSiLRes->GetYaxis()->SetTitleOffset(.4);
			hdSiLRes->GetYaxis()->CenterTitle();
			dc->cd();
			const char *FigsDir = getenv("R15b_OUT");
			dc->SaveAs(Form("%s/AlCapData_Al50Dataset_%sTime_LeftArm.pdf", FigsDir, particle) );
		} else if(strcmp(particle, "triton") == 0) {
			TF1 *fit = new TF1("fit", "expo(0)+pol0(2)", min, max);
			fit->SetLineColor(kBlue);
			fit->SetParameter(0, 7.31791e+00);
			fit->SetParLimits(1, fit->GetParameter(1)*1.1, fit->GetParameter(1)*0.9);
			fit->SetParameter(2, 4.51382e-01);
			fit->SetParLimits(2, 0, 10);
			htSiL->Fit("fit", "SR+");
			Double_t par[3];
			fit->GetParameters(par);
			TH1D *htSiLRes = new TH1D("htSiLRes", "Lifetime Residual;t[ns];#frac{Entries-Fit}{Entries}[%]", nbins, min, max);
			for(int i=1; i<htSiL->GetSize()- 1; ++i) {
				if(htSiL->GetBinContent(i) > 0) {
					Double_t res = 100 * (htSiL->GetBinContent(i) - fit->Eval(min+i*40-40/2) )/htSiL->GetBinContent(i);
					htSiLRes->SetBinContent(i-1, res);
				}
			}
			TCanvas *tc = new TCanvas("tr", "tr");
			TPad *tp1 = new TPad("tp1", "tp1",0,0.3,1,1, kWhite, 0, 0);
			tp1->SetBottomMargin(0.05);
			tp1->Draw();
			TPad *tp2 = new TPad("tp2", "tp2",0,0.0,1,0.3 , kWhite, 0, 0);
			tp2->SetTopMargin(0.00001);
			tp2->SetBottomMargin(0.2);
			tp2->Draw();

			tp1->cd();
			gPad->SetLogy();
			htSiL->Draw("E1");
			htSiL->GetXaxis()->SetLabelSize(0);
			htSiL->GetYaxis()->SetMaxDigits(3);
			htSiL->GetYaxis()->SetTitleSize(.05);
			htSiL->GetYaxis()->SetTitleOffset(.8);
			htSiL->GetYaxis()->SetLabelFont(63);
			htSiL->GetYaxis()->SetLabelSize(16);

			TLegend *legend = new TLegend(0.522923, 0.66879, 0.848138, 0.861996);
			legend->SetHeader("#bf{AlCap} #it{Preliminary}");
			legend->AddEntry("", "50#mum (Left 2#sigma)","");
			legend->AddEntry(htSiL, "triton", "e");
			legend->AddEntry(fit, Form("#tau_{#muAl} %.2f#pm%.2f ns", -1./par[1], 1./(par[1]*par[1]) * fit->GetParError(1) ), "l");
			legend->Draw();
			tp2->cd();
			tp2->SetGridx();
			tp2->SetGridy();
			htSiLRes->Draw();
			htSiLRes->GetXaxis()->SetMaxDigits(3);
			htSiLRes->GetXaxis()->SetLabelFont(63);
			htSiLRes->GetXaxis()->SetLabelSize(15);
			htSiLRes->GetXaxis()->SetTitleSize(.1);
			htSiLRes->GetXaxis()->SetTitleOffset(.7);
			htSiLRes->SetFillColor(38);
			htSiLRes->SetLineColor(kWhite);
			htSiLRes->GetYaxis()->SetNdivisions(10, 2, 0);
			htSiLRes->GetYaxis()->SetLabelFont(63);
			htSiLRes->GetYaxis()->SetLabelSize(16);
			htSiLRes->GetYaxis()->SetTitleSize(.1);
			htSiLRes->GetYaxis()->SetTitleOffset(.4);
			htSiLRes->GetYaxis()->CenterTitle();
			tc->cd();
			const char *FigsDir = getenv("R15b_OUT");
			tc->SaveAs(Form("%s/AlCapData_Al50Dataset_%sTime_LeftArm.pdf", FigsDir, particle) );
		} else if(strcmp(particle, "alpha") == 0) {
			TF1 *fit = new TF1("fit", "expo(0)+pol0(2)", min, max);
			fit->SetLineColor(kBlue);
			fit->SetParameter(0, 7.31791e+00);
			fit->SetParLimits(1, fit->GetParameter(1)*1.1, fit->GetParameter(1)*0.9);
			fit->SetParameter(2, 4.51382e-01);
			fit->SetParLimits(2, 0, 10);
			haSiL->Fit("fit", "SR+");
			Double_t par[3];
			fit->GetParameters(par);
			TH1D *haSiLRes = new TH1D("haSiLRes", "Lifetime Residual;t[ns];#frac{Entries-Fit}{Entries}[%]", nbins, min, max);
			for(int i=1; i<haSiL->GetSize()- 1; ++i) {
				if(haSiL->GetBinContent(i) > 0) {
					Double_t res = 100 * (haSiL->GetBinContent(i) - fit->Eval(min+i*40-40/2) )/haSiL->GetBinContent(i);
					haSiLRes->SetBinContent(i-1, res);
				}
			}
			TCanvas *ac = new TCanvas("ar", "ar");
			TPad *ap1 = new TPad("ap1", "ap1",0,0.3,1,1, kWhite, 0, 0);
			ap1->SetBottomMargin(0.05);
			ap1->Draw();
			TPad *ap2 = new TPad("ap2", "ap2",0,0.0,1,0.3 , kWhite, 0, 0);
			ap2->SetTopMargin(0.00001);
			ap2->SetBottomMargin(0.2);
			ap2->Draw();

			ap1->cd();
			gPad->SetLogy();
			haSiL->Draw("E1");
			haSiL->GetXaxis()->SetLabelSize(0);
			haSiL->GetYaxis()->SetMaxDigits(3);
			haSiL->GetYaxis()->SetTitleSize(.05);
			haSiL->GetYaxis()->SetTitleOffset(.8);
			haSiL->GetYaxis()->SetLabelFont(63);
			haSiL->GetYaxis()->SetLabelSize(16);

			TLegend *legend = new TLegend(0.522923, 0.66879, 0.848138, 0.861996);
			legend->SetHeader("#bf{AlCap} #it{Preliminary}");
			legend->AddEntry("", "50#mum (Left 2#sigma)","");
			legend->AddEntry(haSiL, "alpha", "e");
			legend->AddEntry(fit, Form("#tau_{#muAl} %.2f#pm%.2f ns", -1./par[1], 1./(par[1]*par[1]) * fit->GetParError(1) ), "l");
			legend->Draw();
			ap2->cd();
			ap2->SetGridx();
			ap2->SetGridy();
			haSiLRes->Draw();
			haSiLRes->GetXaxis()->SetMaxDigits(3);
			haSiLRes->GetXaxis()->SetLabelFont(63);
			haSiLRes->GetXaxis()->SetLabelSize(15);
			haSiLRes->GetXaxis()->SetTitleSize(.1);
			haSiLRes->GetXaxis()->SetTitleOffset(.7);
			haSiLRes->SetFillColor(38);
			haSiLRes->SetLineColor(kWhite);
			haSiLRes->GetYaxis()->SetNdivisions(10, 2, 0);
			haSiLRes->GetYaxis()->SetLabelFont(63);
			haSiLRes->GetYaxis()->SetLabelSize(16);
			haSiLRes->GetYaxis()->SetTitleSize(.1);
			haSiLRes->GetYaxis()->SetTitleOffset(.4);
			haSiLRes->GetYaxis()->CenterTitle();
			ac->cd();
			const char *FigsDir = getenv("R15b_OUT");
			ac->SaveAs(Form("%s/AlCapData_Al50Dataset_%sTime_LeftArm.pdf", FigsDir, particle) );
		}
	}
}
