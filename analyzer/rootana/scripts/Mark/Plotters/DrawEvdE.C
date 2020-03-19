void Construct(TH2D *hSiL, TH2D *hSiR, TH2D *hLg_SiL_EvDeltaE, TH2D *hLg_SiR_EvDeltaE) {
	TFile *fData = new TFile(Form("%s/al50.root", getenv("R15b_DATA") ), "READ");
	TTree *tree = (TTree *)fData->Get("tree");
        Double_t t1, t2, t3, e1, e2, e3, timeToPrevTME, timeToNextTME;
	Int_t a1, a2, a3;
        TString *channel = new TString();
        TString *sig1 = new TString();
        TString *sig2 = new TString();
        TString *sig3 = new TString();
        TString *sig4 = new TString();
        tree->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
        tree->SetBranchAddress("timeToNextTME", &timeToNextTME);
        tree->SetBranchAddress("a1", &a1);
        tree->SetBranchAddress("e1", &e1);
        tree->SetBranchAddress("t1", &t1);
        tree->SetBranchAddress("a2", &a2);
        tree->SetBranchAddress("e2", &e2);
        tree->SetBranchAddress("t2", &t2);
        tree->SetBranchAddress("a3", &a3);
        tree->SetBranchAddress("e3", &e3);
        tree->SetBranchAddress("t3", &t3);
        tree->SetBranchAddress("channel", &channel);

        for(Long64_t i=0; i < tree->GetEntries(); ++i) {
                tree->GetEntry(i);
		if(timeToPrevTME < 10e3 || timeToNextTME < 10e3) continue;
		if(t2<500) continue;
		if(t2>10e3) continue;
		if(t2-t1-13 > 60 || t2-t1-13 < -60) continue;
		if(!TMath::IsNaN(e3) ) continue;
		//e1 *= 1000;
		//e2 *= 1000;
		if(a2>3975) continue;
		if(channel->Contains("SiR") ) {
			if(channel->Contains("SiR1_1") && a1>3850. ) continue;
			if(channel->Contains("SiR1_2") && a1>3972. ) continue;
			if(channel->Contains("SiR1_3") && a1>3956. ) continue;
			if(channel->Contains("SiR1_4") && a1>3986. ) continue;
			hSiR->Fill(e1+e2, e1);
			hLg_SiR_EvDeltaE->Fill(0.7071 * (TMath::Log10(e1+e2) - TMath::Log10(e1) ), 0.7071 * (TMath::Log10(e1+e2) + TMath::Log10(e1) ) );
		} else {
			hSiL->Fill(e1+e2, e1);
			hLg_SiL_EvDeltaE->Fill(0.7071 * (TMath::Log10(e1+e2) - TMath::Log10(e1) ), 0.7071 * (TMath::Log10(e1+e2) + TMath::Log10(e1) ) );
		}
	}

}

void DrawEvdE() {
	const char *al50FigsDir = getenv("R15b_OUT");
        TH2D *hSiL = new TH2D("hSiL", "SiL;E [MeV];#DeltaE [MeV]", 200, 0, 20, 200, 0, 8);
        TH2D *hSiR = new TH2D("hSiR", "SiR;E [MeV];#DeltaE [MeV]", 200, 0, 20, 200, 0, 8);
	TH2D *hLg_SiL_EvDeltaE = new TH2D("hLg_SiL_EvDeltaE", ";LgE-Lg#DeltaE / #sqrt{2};LgE+Lg#DeltaE / #sqrt{2}", 250, -0.1, 1.5, 100, .26, 1.4);
	TH2D *hLg_SiR_EvDeltaE = new TH2D("hLg_SiR_EvDeltaE", ";LgE-Lg#DeltaE / #sqrt{2};LgE+Lg#DeltaE / #sqrt{2}", 250, -0.1, 1.5, 100, .26, 1.4);
	Construct(hSiL, hSiR, hLg_SiL_EvDeltaE, hLg_SiR_EvDeltaE);
	TFile *fCuts = new TFile("al50-cuts.root", "READ");
	TCutG *p_lg_SiR = (TCutG *)fCuts->Get("hLg_SiR_EvDeltaE_proton_2sigma");
	TCutG *d_lg_SiR = (TCutG *)fCuts->Get("hLg_SiR_EvDeltaE_deuteron_2sigma");
	TCutG *t_lg_SiR = (TCutG *)fCuts->Get("hLg_SiR_EvDeltaE_triton_2sigma");
	TCutG *a_lg_SiR = (TCutG *)fCuts->Get("hLg_SiR_EvDeltaE_alpha_2sigma");
	TCutG *p_lg_SiL = (TCutG *)fCuts->Get("hLg_SiL_EvDeltaE_proton_2sigma");
	TCutG *d_lg_SiL = (TCutG *)fCuts->Get("hLg_SiL_EvDeltaE_deuteron_2sigma");
	TCutG *t_lg_SiL = (TCutG *)fCuts->Get("hLg_SiL_EvDeltaE_triton_2sigma");
	TCutG *a_lg_SiL = (TCutG *)fCuts->Get("hLg_SiL_EvDeltaE_alpha_2sigma");
	TCutG *pSiR = (TCutG *)fCuts->Get("r_hLg_SiR_EvDeltaE_proton_2sigma");
	TCutG *dSiR = (TCutG *)fCuts->Get("r_hLg_SiR_EvDeltaE_deuteron_2sigma");
	TCutG *tSiR = (TCutG *)fCuts->Get("r_hLg_SiR_EvDeltaE_triton_2sigma");
	TCutG *aSiR = (TCutG *)fCuts->Get("r_hLg_SiR_EvDeltaE_alpha_2sigma");
	TCutG *pSiL = (TCutG *)fCuts->Get("r_hLg_SiL_EvDeltaE_proton_2sigma");
	TCutG *dSiL = (TCutG *)fCuts->Get("r_hLg_SiL_EvDeltaE_deuteron_2sigma");
	TCutG *tSiL = (TCutG *)fCuts->Get("r_hLg_SiL_EvDeltaE_triton_2sigma");
	TCutG *aSiL = (TCutG *)fCuts->Get("r_hLg_SiL_EvDeltaE_alpha_2sigma");

{
	TCanvas *lg_r = new TCanvas("lg_r", "r");
	hLg_SiR_EvDeltaE->Draw();
	p_lg_SiR->Draw("SAME"); p_lg_SiR->SetLineColor(kRed);
	d_lg_SiR->Draw("SAME"); d_lg_SiR->SetLineColor(kBlue);
	t_lg_SiR->Draw("SAME"); t_lg_SiR->SetLineColor(kGreen);
	a_lg_SiR->Draw("SAME"); a_lg_SiR->SetLineColor(kMagenta);
	TLegend *legend = new TLegend(0.64, 0.57, 0.87, 0.75);
	legend->SetHeader("#bf{AlCap} #it{Preliminary}");
	legend->AddEntry("", "50#mum (Right 2#sigma)","");
	legend->AddEntry(p_lg_SiR,"proton","l");
	legend->AddEntry(d_lg_SiR,"deuteron","l");
	legend->AddEntry(d_lg_SiR,"triton","l");
	legend->AddEntry(a_lg_SiR,"alpha","l");
	legend->Draw();
	lg_r->SaveAs(Form("%s/AlCapData_Al50Dataset_TwoLayer_Log_EvdE_RightArm.pdf", al50FigsDir) );
}
{
	TCanvas *lg_l = new TCanvas("lg_l", "l");
	hLg_SiL_EvDeltaE->Draw();
	p_lg_SiL->Draw("SAME"); p_lg_SiL->SetLineColor(kRed);
	d_lg_SiL->Draw("SAME"); d_lg_SiL->SetLineColor(kBlue);
	t_lg_SiL->Draw("SAME"); t_lg_SiL->SetLineColor(kGreen);
	a_lg_SiL->Draw("SAME"); a_lg_SiL->SetLineColor(kMagenta);
	TLegend *legend = new TLegend(0.64, 0.57, 0.87, 0.75);
	legend->SetHeader("#bf{AlCap} #it{Preliminary}");
	legend->AddEntry("", "50#mum (Left 2#sigma)","");
	legend->AddEntry(p_lg_SiL,"proton","l");
	legend->AddEntry(d_lg_SiL,"deuteron","l");
	legend->AddEntry(d_lg_SiL,"triton","l");
	legend->AddEntry(a_lg_SiL,"alpha","l");
	legend->Draw();
	lg_l->SaveAs(Form("%s/AlCapData_Al50Dataset_TwoLayer_Log_EvdE_LeftArm.pdf", al50FigsDir) );
}
{
	TCanvas *r = new TCanvas("r", "r");
	hSiR->Draw();
	pSiR->Draw("SAME"); pSiR->SetLineColor(kRed);
	dSiR->Draw("SAME"); dSiR->SetLineColor(kBlue);
	tSiR->Draw("SAME"); tSiR->SetLineColor(kGreen);
	aSiR->Draw("SAME"); aSiR->SetLineColor(kMagenta);
	TLegend *legend = new TLegend(0.64, 0.66, 0.85, 0.85);
	legend->SetHeader("#bf{AlCap} #it{Preliminary}");
	legend->AddEntry("", "50#mum (Right 2#sigma)","");
	legend->AddEntry(pSiR,"proton","l");
	legend->AddEntry(dSiR,"deuteron","l");
	legend->AddEntry(dSiR,"triton","l");
	legend->AddEntry(aSiR,"alpha","l");
	legend->Draw();
	r->SaveAs(Form("%s/AlCapData_Al50Dataset_TwoLayer_EvdE_RightArm.pdf", al50FigsDir) );
}
{
	TCanvas *l = new TCanvas("l", "l");
	hSiL->Draw();
	pSiL->Draw("SAME"); pSiL->SetLineColor(kRed);
	dSiL->Draw("SAME"); dSiL->SetLineColor(kBlue);
	tSiL->Draw("SAME"); tSiL->SetLineColor(kGreen);
	aSiL->Draw("SAME"); aSiL->SetLineColor(kMagenta);
	TLegend *legend = new TLegend(0.64, 0.66, 0.85, 0.85);
	legend->SetHeader("#bf{AlCap} #it{Preliminary}");
	legend->AddEntry("", "50#mum (Left 2#sigma)","");
	legend->AddEntry(pSiL,"proton","l");
	legend->AddEntry(dSiL,"deuteron","l");
	legend->AddEntry(dSiL,"triton","l");
	legend->AddEntry(aSiL,"alpha","l");
	legend->Draw();
	l->SaveAs(Form("%s/AlCapData_Al50Dataset_TwoLayer_EvdE_LeftArm.pdf", al50FigsDir) );
}
}
