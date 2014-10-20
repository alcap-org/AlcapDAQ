void guess_parameters(const TH1* h, Double_t par[7]) {
  const Double_t e_xray = 346.828, e_gamma = 351.932;
  par[5] = 0.; par[2] = 1.; par[4] = 1.; // Pedestal, sigma aluminum, sigma lead
  par[6] = h->Integral()/(Double_t)(h->GetXaxis()->GetLast() - h->GetXaxis()->GetFirst());
  par[0] = h->GetBinContent(h->FindFixBin(e_xray)) - par[5];
  par[3] = h->GetBinContent(h->FindFixBin(e_gamma)) - par[5];
  par[1] = e_xray;
}

void count_muons() {
  const Double_t rt2pi = TMath::Sqrt(2.*TMath::Pi());
  const char dataset[] = "Al50b";
  const double dE = 5.104; // Pb214 gamma minus Al muonic 2p-1s
  const unsigned int nfiles = 32;
  
  Double_t nxrays_all[nfiles], exrays_all[nfiles];
  Double_t nxrays_prompt[nfiles], exrays_prompt[nfiles];
  Double_t nenter[nfiles];
  Double_t chi2[nfiles], ndf[nfiles];

  char formula[256];
  printf(formula); printf("\n");
  TF1* fit = new TF1("pkfunc", formula);
  TF1* eff = new TF1("efffunc", "[0]*x^[1]"); eff->SetParameters(0.08051, -0.8579);
  unsigned int nhists = 0;
  for (unsigned int i = 0; i < nfiles; ++i) {
    char fname[256];
    sprintf(fname, "%s_%u.root", dataset, i);
    TFile* f = new TFile(fname, "READ");
    TH1* hAll = (TH1*)f->Get("all");
    TH1* hPrompt = (TH1*)f->Get("prompt");
    TH1* hEnter = (TH1*)f->Get("enter");
    hAll->GetXaxis()->SetRangeUser(343., 354.);
    hPrompt->GetXaxis()->SetRangeUser(343., 354.);
    Double_t parAll[7], parPrompt[7];
    guess_parameters(hAll, parAll);
    guess_parameters(hPrompt, parPrompt);

    fit->SetParameters(parAll); fit->SetParLimits(2, 0.2, 5.);
    TFitResultPtr resAll = hAll->Fit(fit, "SNQ");
    fit->SetParameters(parPrompt); fit->SetParLimits(4, 0.2, 5.);
    TFitResultPtr resPrompt = hPrompt->Fit(fit, "SNQ");

    nxrays_all[i] = resAll->Value(0)*resAll->Value(2)*rt2pi;
    nxrays_prompt[i] = resPrompt->Value(0)*resPrompt->Value(2)*rt2pi;
    nenter[i] = hEnter->Integral(248, -1);

    exrays_all[i] = nxrays_all[i]*TMath::Sqrt(TMath::Power(resAll->ParError(0)/resAll->Value(0), 2.) + TMath::Power(resAll->ParError(2)/resAll->Value(2), 2.));
    exrays_prompt[i] = nxrays_prompt[i]*TMath::Sqrt(TMath::Power(resPrompt->ParError(0)/resPrompt->Value(0), 2.) + TMath::Power(resPrompt->ParError(2)/resPrompt->Value(2), 2.));
  }

  Double_t n[nfiles];
  Double_t nper_all[nfiles], eper_all[nfiles];
  Double_t nper_prompt[nfiles], eper_prompt[nfiles];
  for (unsigned int i = 0; i < nfiles; ++i) {
    const Double_t emission_branching_ratio = 0.798;
    n[i] = (Double_t)i;
    nper_all[i] = nxrays_all[i]/eff->Eval(nxrays_all[i])/emission_branching_ratio/nenter[i];
    eper_all[i] = exrays_all[i]/nxrays_all[i]*nper_all[i];
    nper_prompt[i] = nxrays_prompt[i]/eff->Eval(nxrays_prompt[i])/emission_branching_ratio/nenter[i];
    eper_prompt[i] = exrays_prompt[i]/nxrays_prompt[i]*nper_prompt[i];
    printf("    %g %g\n", eper_all[i], eper_prompt[i]);
  }
  TGraphErrors* gr_all = new TGraphErrors(nfiles, n, nper_all, 0x0, eper_all);
  TGraphErrors* gr_prompt = new TGraphErrors(nfiles, n, nper_prompt, 0x0, eper_prompt);
  gr_all->SetMarkerStyle(2); gr_all->SetMarkerColor(kBlack);
  gr_prompt->SetMarkerStyle(2); gr_prompt->SetMarkerColor(kBlack);
  new TCanvas(); gr_all->Draw("AP");
  new TCanvas(); gr_prompt->Draw("AP");
}
