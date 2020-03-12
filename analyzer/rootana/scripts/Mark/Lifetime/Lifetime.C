using namespace std;
void Lifetime(const char *filename = "al100.root", const char *particle = "proton")
{
	Int_t min = 500;
	Int_t max = 3500;
	Int_t nbins = (max-min)/40;
	TFile *fData = new TFile(filename, "READ");
	Double_t t1, t2, timeToPrevTME, timeToNextTME;
	TString *channel = new TString("");
	TString *sig3= new TString("");
	TH1D *hMeasDataLeft = new TH1D("Left detector", "Proton arrival times;t [ns];Counts/40ns", nbins, min, max);
	TH1D *hMeasDataRight = new TH1D("Right detector", "Proton arrival times;t [ns]; Counts/40ns", nbins, min, max);
	TTree *tree = (TTree *)fData->Get("tree");
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("t2", &t2);
	tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
	tree->SetBranchAddress("channel", &channel);
	tree->SetBranchAddress("sig3", &sig3);
	for(Long64_t i=0; i < tree->GetEntries(); i++) {
		tree->GetEntry(i);
		if(!sig3->Contains(particle) ) continue;
		if(abs(t2-t1) > 200) continue;
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		if(abs(t2)>10e3) continue;
		if(t2<500) continue;
		if(channel->Contains("SiL") ) {
			hMeasDataLeft->Fill(t2);
		} else if(channel->Contains("SiR") ) {
			hMeasDataRight->Fill(t2);
		}
	}

	TF1 *fit = new TF1("fit", "expo(0)", min, max);
	fit->SetLineColor(kRed);
	fit->SetParameter(0, 4.31791e+00);
	fit->SetParameter(1, -0.00115);
	fit->SetParLimits(1, fit->GetParameter(1)*1.1, fit->GetParameter(1)*0.9);

	fit->SetParName(0, "Al constant");
	fit->SetParName(1, "Al slope");
	Double_t par[3];
	{
		TCanvas *c = new TCanvas("c", "c");
		gPad->SetLogy();
		hMeasDataRight->Fit("fit", "SR+");
		fit->GetParameters(par);

		TLegend *legendR = new TLegend(.15, .15, .4, .4, "AlCap #bf{#it{100#mum Charged particles} }");
		legendR->AddEntry(hMeasDataRight, "#bf{#muAl #rightarrow protons (Right)}", "LE");
		legendR->AddEntry(fit, Form("#bf{#tau_{#muAl} = %.1f(%.1f) ns}", -1./par[1], 1./(par[1]*par[1]) * fit->GetParError(1) ), "L");
		legendR->Draw("SAME");
	}{
		TCanvas *d = new TCanvas("d", "d");
		gPad->SetLogy();
		hMeasDataLeft->Fit("fit", "SR+");
		fit->GetParameters(par);

		TLegend *legendR = new TLegend(.15, .15, .4, .4, "AlCap #bf{#it{100#mum Charged particles} }");
		legendR->AddEntry(hMeasDataLeft, "#bf{#muAl #rightarrow protons (Left)}", "LE");
		legendR->AddEntry(fit, Form("#bf{#tau_{#muAl} = %.1f(%.1f) ns}", -1./par[1], 1./(par[1]*par[1]) * fit->GetParError(1) ), "L");
		legendR->Draw("SAME");
	}
	alcaphistogram(hMeasDataLeft);
	alcaphistogram(hMeasDataRight);
}
