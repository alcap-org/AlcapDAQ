{
  const Double_t rt2pi = TMath::Sqrt(2.*TMath::Pi());
  const unsigned int nfiles = 30;
  const char* files[nfiles] = { "add1.root",  "add2.root",  "add3.root",  "add4.root",  "add5.root",
				"add6.root",  "add7.root",  "add8.root",  "add9.root",  "add10.root",
				"add11.root", "add12.root", "add13.root", "add14.root", "add15.root",
				"add16.root", "add17.root", "add18.root", "add19.root", "add20.root",
				"add21.root", "add22.root", "add23.root", "add24.root", "add25.root",
				"add26.root", "add27.root", "add28.root", "add29.root", "add30.root" };
  Double_t nmuons[nfiles];
  TSpectrum* pkfind = new TSpectrum(10);
  //  TF1* fit = new TF1("fitfunc", "gaus(0)+gaus(3)+[6]*x+[7]");
  TF1* fit = new TF1("pkfunc", "[0]*exp(-0.5*((x-[1])/[2])**2)+[3]*exp(-0.5*((x-([1]+5.104))/[4])**2)+[5]*x+[6]"); // 5.104 is Pb214 gamma minus Al 2p-1s
  for (unsigned int i = 0; i < nfiles; ++i) {
    char loc[256];
    sprintf(loc, "out/v37/%s", files[i]);
    TFile* f = new TFile(loc);
    TH1* h = (TH1*)f->Get("GeSpectrum/hEnergy");
    h->GetXaxis()->SetRangeUser(50.,1800.);
    TH1* b = pkfind->Background(h);
    /* h->Add(b, -1.); */
    h->GetXaxis()->SetRangeUser(340., 355.);
    /* if (pkfind->Search(h, 7., "", 0.3) != 3) { */
    /*   printf("Wrong number of peaks found (%s)!\n", files[i]); */
    /*   nmuons[i] = 0.; */
    /*   new TCanvas(); */
    /*   h->DrawClone(); */
    /*   continue; */
    /* } */
    /* Float_t* mean = pkfind->GetPositionX(); */
    /* Float_t* amp = pkfind->GetPositionY(); */
    Float_t mean = 345.;
    Float_t amp[2] = { 30., 40.};
    Float_t sigma[2] = { 1., 1.};
    fit->SetParameters(amp[0], mean, sigma[0], amp[1], sigma[1], 0., 0.);
    fit->SetParLimits(1, 343., 348.);
    fit->SetParLimits(2, 0., 3.);
    fit->SetParLimits(4, 0., 3.);
    fit->SetParLimits(5, -10., 0.);
    h->Fit(fit,"B");
    nmuons[i] = fit->GetParameter(0)*fit->GetParameter(2)*rt2pi;
    new TCanvas();
    h->DrawClone();
  }
  Double_t total_muons = 0.;
  for (unsigned int i = 0; i < nfiles; ++i) {
    printf("Number muons: %g\n", nmuons[i]);
    total_muons += nmuons[i];
  }
  printf("Total muons: %g\n", total_muons);
}
