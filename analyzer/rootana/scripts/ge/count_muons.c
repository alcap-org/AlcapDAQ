/* void guess_parameters(const TH1* h, Double_t par[7]) { */
/*   const Double_t e_xray = 346.828, e_gamma = 351.932; */
/*   par[5] = 0.; par[2] = 1.; par[4] = 1.; // Pedestal, sigma aluminum, sigma lead */
/*   par[6] = h->Integral()/(Double_t)(h->GetXaxis()->GetLast() - h->GetXaxis()->GetFirst()); */
/*   par[0] = h->GetBinContent(h->FindFixBin(e_xray)) - par[5]; */
/*   par[3] = h->GetBinContent(h->FindFixBin(e_gamma)) - par[5]; */
/*   par[1] = e_xray; */
/* } */

void guess_parameters(const TH1* h, Double_t par[8]) {
  const Double_t e_xray = 346.828, e_gamma = 351.932;
  par[7] = 0.; par[2] = 1.; par[5] = 1.; // Pedestal, sigma aluminum, sigma lead
  par[6] = h->Integral()/(Double_t)(h->GetXaxis()->GetLast() - h->GetXaxis()->GetFirst());
  par[0] = h->GetBinContent(h->FindFixBin(e_xray)) - par[6];
  par[3] = h->GetBinContent(h->FindFixBin(e_gamma)) - par[6];
  par[1] = e_xray; par[4] = e_gamma;
  if (par[0] < 0)
    par[0] = 1.;
  if (par[3] < 0)
    par[3] = 1.;
}

/* void guess_parameters(const TH1* h, Double_t par[5]) { */
/*   const Double_t e_xray = 346.828; */
/*   par[4] = 0.; par[2] = 1.; // Pedestal, sigma aluminum, sigma lead */
/*   par[3] = h->Integral()/(Double_t)(h->GetXaxis()->GetLast() - h->GetXaxis()->GetFirst()); */
/*   par[0] = h->GetBinContent(h->FindFixBin(e_xray)) - par[3]; */
/*   par[1] = e_xray; */
/* } */


void count_muons() {
  const Double_t rt2pi = TMath::Sqrt(2.*TMath::Pi());
  const char dataset[] = "Al50b";
  const double E_xray = 346.828;
  const double dE = 5.104; // Pb214 gamma minus Al muonic 2p-1s
  const unsigned int nfiles = 16;
  
  Double_t nxrays_all[nfiles], exrays_all[nfiles];
  Double_t nxrays_prompt[nfiles], exrays_prompt[nfiles];
  Double_t nenter[nfiles];
  Double_t chi2All[nfiles], chi2Prompt[nfiles];

  char formula[256];
  sprintf(formula, "gaus(0)+gaus(3)+pol1(6)");
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
    Double_t parAll[8], parPrompt[8];
    guess_parameters(hAll, parAll);
    guess_parameters(hPrompt, parPrompt);

    TCanvas* c = new TCanvas(); c->Divide(2);
    c->cd(1);
    fit->SetParameters(parAll);
    fit->SetParLimits(0, 0., 50.*parAll[0]);
    fit->SetParLimits(2, 0.5, 2.0);
    fit->SetParLimits(3, 0., 50.*parAll[3]);
    fit->SetParLimits(5, 0.5, 2.0);
    TFitResultPtr resAll = hAll->Fit(fit, "S");
    c->cd(2);
    fit->SetParameters(parPrompt);
    fit->SetParLimits(0, 0., 50.*parPrompt[0]);
    fit->SetParLimits(2, 0.5, 2.0);
    fit->SetParLimits(3, 0., 50.*parPrompt[3]);
    fit->SetParLimits(5, 0.5, 2.0);
    TFitResultPtr resPrompt = hPrompt->Fit(fit, "S");

    nxrays_all[i] = resAll->Value(0)*resAll->Value(2)*rt2pi/hAll->GetXaxis()->GetBinWidth(1);
    nxrays_prompt[i] = resPrompt->Value(0)*resPrompt->Value(2)*rt2pi/hPrompt->GetXaxis()->GetBinWidth(1);
    nenter[i] = hEnter->Integral(248, -1);

    exrays_all[i] = nxrays_all[i]*TMath::Sqrt(TMath::Power(resAll->ParError(0)/resAll->Value(0), 2.) + TMath::Power(resAll->ParError(2)/resAll->Value(2), 2.));
    exrays_prompt[i] = nxrays_prompt[i]*TMath::Sqrt(TMath::Power(resPrompt->ParError(0)/resPrompt->Value(0), 2.) + TMath::Power(resPrompt->ParError(2)/resPrompt->Value(2), 2.));

    chi2All[i] = resAll->Chi2()/resAll->Ndf();
    chi2Prompt[i] = resPrompt->Chi2()/resPrompt->Ndf();
  }

  Double_t n[nfiles];
  Double_t nper_all[nfiles], eper_all[nfiles];
  Double_t nper_prompt[nfiles], eper_prompt[nfiles];
  for (unsigned int i = 0; i < nfiles; ++i) {
    const Double_t emission_branching_ratio = 0.798;
    n[i] = (Double_t)i;
    nper_all[i] = nxrays_all[i]/eff->Eval(E_xray)/emission_branching_ratio/nenter[i];
    eper_all[i] = exrays_all[i]/nxrays_all[i]*nper_all[i];
    nper_prompt[i] = nxrays_prompt[i]/eff->Eval(E_xray)/emission_branching_ratio/nenter[i];
    eper_prompt[i] = exrays_prompt[i]/nxrays_prompt[i]*nper_prompt[i];
    printf("    %g %g\n    %g %g\n", nper_all[i], nper_prompt[i], eper_all[i], eper_prompt[i]);
  }

  TF1* linefit = new TF1("linefit", "[0]");
  TLegend* leg;
  TFitResultPtr res;
  char lab[64];

  TGraphErrors* gr_all = new TGraphErrors(nfiles, n, nper_all, 0x0, eper_all);
  TGraphErrors* gr_prompt = new TGraphErrors(nfiles, n, nper_prompt, 0x0, eper_prompt);
  gr_all->SetMarkerStyle(2); gr_all->SetMarkerColor(kRed);
  gr_prompt->SetMarkerStyle(2); gr_prompt->SetMarkerColor(kGreen);
  leg = new TLegend(0.5,0.7,0.9,0.9);
  linefit->SetParameter(0, 1.);
  res = gr_all->Fit(linefit, "S"); gr_all->GetFunction("linefit")->SetLineColor(kRed);
  sprintf(lab, "No time cuts (y=%g, #chi^{2}=%g)", res->Value(0), res->Chi2()/res->Ndf());
  leg->AddEntry(gr_all, lab);
  linefit->SetParameter(0, 1.);
  res = gr_prompt->Fit(linefit, "S"); gr_prompt->GetFunction("linefit")->SetLineColor(kGreen);
  sprintf(lab, "Prompt (y=%g, #chi^{2}=%g)", res->Value(0), res->Chi2()/res->Ndf());
  leg->AddEntry(gr_prompt, lab);
  TMultiGraph* mg1 = new TMultiGraph("mg1", "Stops per entering muon (PP Al50b)");
  mg1->Add(gr_all); mg1->Add(gr_prompt);
  new TCanvas();
  mg1->Draw("AP"); mg1->GetXaxis()->SetTitle("Run Group (Groups of 10)"); mg1->GetYaxis()->SetTitle("Stops per Muon");
  leg->Draw("SAME");

  TGraph *gr_chi2All = new TGraph(nfiles, n, chi2All);
  TGraph *gr_chi2Prompt = new TGraph(nfiles, n, chi2Prompt);
  gr_chi2All->SetMarkerStyle(2); gr_chi2All->SetMarkerColor(kRed);
  gr_chi2Prompt->SetMarkerStyle(2); gr_chi2Prompt->SetMarkerColor(kGreen);
  TMultiGraph* mg2 = new TMultiGraph("mg2", "Reduced Chi2 of XRay Peak Fits (PP Al50b)");
  mg2->Add(gr_chi2All); mg2->Add(gr_chi2Prompt);
  new TCanvas();
  mg2->Draw("AP"); mg2->GetXaxis()->SetTitle("Run Group (Groups of 10)"); mg2->GetYaxis()->SetTitle("Reduced #chi^{2}");
  leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(gr_chi2All, "No time cut");
  leg->AddEntry(gr_chi2Prompt, "Prompt");
  leg->Draw("SAME");
}
