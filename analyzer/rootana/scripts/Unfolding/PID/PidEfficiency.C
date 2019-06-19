#include <TMatrix.h>
void PidEfficiency(const char * arm = "SiR", Int_t sigma=3) {
	std::cout << "Efficiency calculation for " << sigma << " sigma cut" << std::endl;
	TFile *fCuts = new TFile("al50-cuts.root", "READ");
	TChain *tree = new TChain("oTree");
	tree->Add("~/data/transfer/transfer.proton.localhisto.7m.Al50.1300um53um.root");
	tree->Add("~/data/transfer/transfer.deuteron.localhisto.7m.Al50.1300um53um.root");
	tree->Add("~/data/transfer/transfer.triton.localhisto.7m.Al50.1300um53um.root");
	TCutG *cutg_p = (TCutG *)fCuts->Get(Form("hLg_%s_EvDeltaE_proton_%dsigma", arm, sigma) );
	TCutG *cutg_d = (TCutG *)fCuts->Get(Form("hLg_%s_EvDeltaE_deuteron_%dsigma", arm, sigma) );
	TCutG *cutg_t = (TCutG *)fCuts->Get(Form("hLg_%s_EvDeltaE_triton_%dsigma", arm, sigma) );

	TMatrixD eff(3,3);
	TMatrixD Atrue(3,1);
	TMatrixD Ameas(3,1);

	Double_t e1, e2, e3, t1, t2;
	TString *channel = new TString("");
	TString *pid= new TString("");
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("e2", &e2);
	tree->SetBranchAddress("e3", &e3);
	tree->SetBranchAddress("channel", &channel);
	tree->SetBranchAddress("pid", &pid);

	for(Long64_t i=0; i < tree->GetEntries(); i++) {
		tree->GetEntry(i);
		e1 = e1 * 0.001;
		e2 = e2 * 0.001;
		e3 = e3 * 0.001;
		if(e3 != 0) continue;
		if(channel->Contains(arm) ) {
			if(cutg_p->IsInside(0.7071 * (TMath::Log10(e1+e2) - TMath::Log10(e1) ), 0.7071 * (TMath::Log10(e1+e2) + TMath::Log10(e1) ) ) ) {
				if(pid->Contains("p") ) eff[0][0]++;
				if(pid->Contains("d") ) eff[1][0]++;
				if(pid->Contains("t") ) eff[2][0]++;
				Ameas[0][0]++;
			}
			if(cutg_d->IsInside(0.7071 * (TMath::Log10(e1+e2) - TMath::Log10(e1) ), 0.7071 * (TMath::Log10(e1+e2) + TMath::Log10(e1) ) ) ) {
				if(pid->Contains("p") ) eff[0][1]++;
				if(pid->Contains("d") ) eff[1][1]++;
				if(pid->Contains("t") ) eff[2][1]++;
				Ameas[1][0]++;
			}
			if(cutg_t->IsInside(0.7071 * (TMath::Log10(e1+e2) - TMath::Log10(e1) ), 0.7071 * (TMath::Log10(e1+e2) + TMath::Log10(e1) ) ) ) {
				if(pid->Contains("p") ) eff[0][2]++;
				if(pid->Contains("d") ) eff[1][2]++;
				if(pid->Contains("t") ) eff[2][2]++;
				Ameas[2][0]++;
			}
			if((0.7071 * (TMath::Log10(e1+e2) - TMath::Log10(e1) ) ) < 1.246 ) { //left 1.176, right 1.246
				if(pid->Contains("p") ) Atrue[0][0]++;
			}
			if((0.7071 * (TMath::Log10(e1+e2) - TMath::Log10(e1) ) ) < 1.02 ) { //left 1.02
				if(pid->Contains("d") ) Atrue[1][0]++;
			}
			if((0.7071 * (TMath::Log10(e1+e2) - TMath::Log10(e1) ) ) < 0.81 ) { //left 0.81
				if(pid->Contains("t") ) Atrue[2][0]++;
			}
		}
	}
	std::cout << "Ameas" << std::endl;
	Ameas.Print();
	std::cout << "Atrue" << std::endl;
	Atrue.Print();

	eff[0][0] = eff[0][0] / Atrue[0][0];
	eff[1][1] = eff[1][1] / Atrue[1][0];
	eff[2][2] = eff[2][2] / Atrue[2][0];

	eff[1][0] = eff[1][0] / Atrue[0][0];
	eff[2][0] = eff[2][0] / Atrue[0][0];
	eff[0][1] = eff[0][1] / Atrue[1][0];
	eff[2][1] = eff[2][1] / Atrue[1][0];
	eff[0][2] = eff[0][2] / Atrue[2][0];
	eff[1][2] = eff[1][2] / Atrue[2][0];
	std::cout << "eff" << std::endl;
	eff.Print();
	std::cout << "result" << std::endl;
	(eff.Invert() * Ameas).Print();

//	tree->Draw("0.7071 * (TMath::Log10(e1/1000+e2/1000) + TMath::Log10(e1/1000) ) : 0.7071 * (TMath::Log10(e1/1000+e2/1000) - TMath::Log10(e1/1000) )", "e3==0 && channel.Contains(\"SiL\")");
//	cutg_p->Draw("SAME");
//	cutg_d->Draw("SAME");
//	cutg_t->Draw("SAME");
}
