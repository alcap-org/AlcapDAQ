//Some global variable for convenience
Bool_t debug = kTRUE;
Double_t window = .06;
Double_t ymin = .24; //y-limits chosen for protons
Double_t ymax = .70;
std::map<const char*, std::vector<Double_t> > results;
std::map<const char*, std::vector<Double_t> > results_err;
std::map<int, TH1D *> Slice(TH2D *h) {
    std::map<int, TH1D *> map;
    for(int i=0; i < 18; i++) { //up to 10MeV
        //store projections of different Lg #DeltaE+E segments in map, divided into equal portions of 1MeV
        map[i] = h->ProjectionY(Form("%.3f_%.3f_py", (i+1)/2.+.5, (i+1)/2.+1.), h->GetXaxis()->FindBin(TMath::Log10((i+1)/2.+.5) )+1, h->GetXaxis()->FindBin(TMath::Log10((i+1)/2.+1.) ) );
    }
    return map;
}
void Fit(TH1D *h) {
    ////
    h->GetXaxis()->SetTitle("Lg #DeltaE / #sqrt{2}");
    h->GetXaxis()->SetTitleOffset(1.2);
    h->GetYaxis()->SetTitle(Form("Counts/%f", h->GetBinWidth(0) ) );
    h->Rebin(5);
    ////
    Double_t hmax = h->GetXaxis()->GetBinCenter(h->GetMaximumBin() );
    //This assumes the proton band produces the highest peak in the 2D plot
    //TF1 *fit = new TF1("fit", "gaus(0)+gaus(3)+pol0(6)", ymin, ymax); //rough fit window chosen
    TF1 *fit = new TF1("fit", "gaus(0)+gaus(3)", ymin, ymax); //rough fit window chosen
    fit->SetParameters(400, hmax, 0.02, 400, hmax+0.17, 0.02); //Initial fit parameters
    fit->SetParName(0, "Pro. Constant");
    fit->SetParName(1, "Pro. Mean");
    fit->SetParLimits(1, .35, .50);
    fit->SetParName(2, "Pro. Sigma");
    fit->SetParName(3, "Deu. Constant");
    fit->SetParName(4, "Deu. Mean");
    fit->SetParLimits(4, .55, .75);
    fit->SetParName(5, "Deu. Sigma");
    TFitResultPtr r = h->Fit(fit, "QRS");
    TMatrixDSym cov = r->GetCovarianceMatrix();
    //Get the covariance between sigma and constant parameters after fitting
    Double_t sigmaAndConstantCovariance = cov.GetMatrixArray()[2];
    Double_t area = TMath::Sqrt(2*3.142) * fit->GetParameter(0) * fit->GetParameter(2) / h->GetBinWidth(0);
    Double_t area_error = area * TMath::Sqrt(TMath::Power(fit->GetParError(0) / fit->GetParameter(0), 2) + TMath::Power(fit->GetParError(2) / fit->GetParameter(2), 2) + 2*(fit->GetParError(0) / fit->GetParameter(0) )*(fit->GetParError(2) / fit->GetParameter(2) )*sigmaAndConstantCovariance);
    TLatex latex;
    latex.DrawLatex(.35, 20, Form("Area: %.2f#pm%.2f", area, area_error) );
    std::cout << "========" << h->GetName() << "========" << std::endl;
    if(debug) {
        std::cout << "Bin width:\t" << h->GetBinWidth(0)  << std::endl;
        std::cout << "Constant:\t" << fit->GetParameter(0) << "\t±" << fit->GetParError(0) << std::endl;
        std::cout << "Mean:\t\t" << fit->GetParameter(1) << "\t±" << fit->GetParError(1) << std::endl;
        std::cout << "Sigma:\t\t" << fit->GetParameter(2) << "\t±" << fit->GetParError(2) << std::endl;
        Double_t int_err;
        std::cout << "Entries:\t" << h->IntegralAndError(h->FindBin(hmax-window), h->FindBin(hmax+window), int_err ) << "\t\t±" << int_err << "\t(plain sum over bins)" << std::endl;
        std::cout << "Integral:\t " << area << "\t±" << area_error << "\t(c*s*sqrt(2pi) )" << std::endl;
    }
//    std::cout << "Background:\t" << fit->GetParameter(3) * (2*window) / h->GetBinWidth(0) << "\t\t±" << fit->GetParError(3) << "\t(as reference)" << std::endl;
    results["bin_width"].push_back(h->GetBinWidth(0) );
    results["constant"].push_back(fit->GetParameter(0) );
    results["mean"].push_back(fit->GetParameter(1) );
    results["sigma"].push_back(fit->GetParameter(2) );
    results["area"].push_back(area);
    results_err["bin_width"].push_back(0.);
    results_err["constant"].push_back(fit->GetParError(0) );
    results_err["mean"].push_back(fit->GetParError(1) );
    results_err["sigma"].push_back(fit->GetParError(2) );
    results_err["area"].push_back(area_error);
}
TCutG * DefineCut() {
    Int_t mapsize = results["mean"].size();
    Int_t sigma = 3;
    TCutG *cutg = new TCutG("box", 4*mapsize+1);
    for(int i=0; i < mapsize; i++) {
        cutg->SetPoint(2*i, TMath::Log10((i+1)/2.+.5), results["mean"][i] - sigma*results["sigma"][i]);
        cutg->SetPoint(2*i+1, TMath::Log10((i+1)/2.+1.), results["mean"][i] - sigma*results["sigma"][i]);
    }
    for(int i=mapsize; i < 2*mapsize; i++) {
        int c = 2*mapsize-i;
        cutg->SetPoint(2*i, TMath::Log10(c/2.+1.), results["mean"][c-1] + sigma*results["sigma"][c-1]);
        cutg->SetPoint(2*i+1, TMath::Log10(c/2.+.5), results["mean"][c-1] + sigma*results["sigma"][c-1]);
    }
    cutg->SetPoint(4*mapsize, TMath::Log10((0+1)/2.+.5), results["mean"][0] - sigma*results["sigma"][0]);
    cutg->SetLineColor(kRed);
    return cutg;
}
/**
    Get those Lg10 EvDeltaE plots from the TMETree processed ROOT file
    TCutg definition fine tuned and used for selecting protons for energy and time spectrum
    Plots of the Lg10 EvDeltaE plot and its projection is for checks
    The other TCanvas contains slices from 0-10 MeV (rotate 45 counter clockwise)
    The slices are put into a std::map and a gaus+pol0 fit is performed
    A rough integral or plain sum over bins are done as another check on the gaussian integral
    The difference between the Entries var and Integral var is(should be) the Background var (pol0 * fit window size and corrected with the bin size)
*/
void Pid(Bool_t plotLeft=kTRUE) {
    gStyle->SetOptFit(1);
    TFile *f = new TFile("al100-wPP-wCoincidence-gt400ns.root", "READ");
    TH2D *h = 0;
    if(plotLeft) {
        h = (TH2D *)f->Get("hLg_SiL_EvDeltaE_wPP_gt400ns");
    } else {
        h = (TH2D *)f->Get("hLg_SiR_EvDeltaE_wPP_gt400ns");
    }
    //Slicing via y-projection and storing in the std::map
    std::map<int, TH1D*> map = Slice(h);
    std::map<int, TH1D*>::iterator it;

//    TH1D *hSpectrum = new TH1D("hSpectrum", "Proton spectrum;E [MeV];Counts / 0.5MeV", 22, 0, 11);
    TCanvas *c = new TCanvas("c", "c", 1440, 680);
    c->Divide(map.size()/4, 5);
    int i=0;
    Double_t sum=0;
    for(it=map.begin(); it != map.end(); it++) {
        c->cd(i+1);
        TH1D *hclone = (TH1D *) it->second->Clone();
        //Fitting of slices
        Fit(hclone);
        i++;
    }
    TCutG *cutg = DefineCut();
    TCanvas *ref = new TCanvas("ref", "ref", 1440, 680);
    h->Draw();
    cutg->Draw("SAME");
}
