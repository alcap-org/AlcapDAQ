void guess_parameters(const TH1* h, Double_t par[8]) {
  const Double_t e_xray = 346.828, e_gamma = 351.932;
  par[7] = 0.; par[2] = 1.; par[5] = 1.; // Pedestal, sigma aluminum, sigma lead
  par[6] = h->Integral()/(Double_t)(h->GetXaxis()->GetLast() - h->GetXaxis()->GetFirst());
  par[0] = h->GetBinContent(h->FindFixBin(e_xray)) - par[6];
  par[3] = h->GetBinContent(h->FindFixBin(e_gamma)) - par[6];
  par[1] = e_xray; par[4] = e_gamma;
  if (par[0] < 0.)
    par[0] = 1.;
  if (par[3] < 0.)
    par[3] = 1.;
}

Double_t count_entering_muons(TH1* h) {
  Double_t n = 0.;
  for (Int_t i = 1; i <= h->GetNbinsX(); ++i)
    n += (Double_t)(i-1)*(h->GetBinContent(i));
  return n;
}

unsigned int get_number_of_files(const char* ds) {
  char str[64]; sprintf(str, "%s_", ds);
  unsigned int n = 0;
  TSystemDirectory dir(".", ".");
  TList* files = dir.GetListOfFiles();
  TIter next(files);
  TSystemFile* f;
  while ((f=(TSystemFile*)next()))
    if (TString(f->GetName()).BeginsWith(str))
      ++n;
  return n;
}

void count_muons(const char dataset[], bool pp) {
  const Double_t rt2pi = TMath::Sqrt(2.*TMath::Pi());
  const double E_xray = 346.828;
  const double dE = 5.104; // Pb214 gamma minus Al muonic 2p-1s
  const unsigned int nfiles = get_number_of_files(dataset);
  
  Double_t nxrays_all[nfiles], exrays_all[nfiles];
  Double_t nxrays_prompt[nfiles], exrays_prompt[nfiles];
  Double_t nenter[nfiles];
  Double_t chi2All[nfiles], chi2Prompt[nfiles];

  char formula[256];
  sprintf(formula, "gaus(0)+gaus(3)+pol1(6)");
  TF1* fit = new TF1("pkfunc", formula);
  TF1* eff = new TF1("efffunc", "[0]*x^[1]"); eff->SetParameters(0.08051, -0.8579);
  unsigned int nhists = 0;
  for (unsigned int i = 0; i < nfiles; ++i) {
    char fname[256];
    sprintf(fname, "%s_%u.root", dataset, i);
    TFile* f = new TFile(fname, "READ");
    TH1 *hAll, *hPrompt, *hEnter;
    if (pp) {
      hAll = (TH1*)f->Get("pp_all");
      hPrompt = (TH1*)f->Get("pp_prompt");
      hEnter = (TH1*)f->Get("pp_enter");
    } else {
      hAll = (TH1*)f->Get("all");
      hPrompt = (TH1*)f->Get("prompt");
      hEnter = (TH1*)f->Get("enter");
    }
    hAll->GetXaxis()->SetRangeUser(343., 354.);
    hPrompt->GetXaxis()->SetRangeUser(343., 354.);
    Double_t parAll[8], parPrompt[8];
    guess_parameters(hAll, parAll);
    guess_parameters(hPrompt, parPrompt);

    fit->SetParameters(parAll);
    fit->SetParLimits(0, 0., 50.*parAll[0]);
    fit->SetParLimits(1, 344., 349.);
    fit->SetParLimits(2, 0.5, 2.0);
    fit->SetParLimits(3, 0., 50.*parAll[3]);
    fit->SetParLimits(4, 349., 354.);
    fit->SetParLimits(5, 0.5, 2.0);
    TFitResultPtr resAll = hAll->Fit(fit, "SNQ");
    fit->SetParameters(parPrompt);
    fit->SetParLimits(0, 0., 50.*parPrompt[0]);
    fit->SetParLimits(1, 344., 349.);
    fit->SetParLimits(2, 0.5, 2.0);
    fit->SetParLimits(3, 0., 50.*parPrompt[3]);
    fit->SetParLimits(4, 349., 354.);
    fit->SetParLimits(5, 0.5, 2.0);
    TFitResultPtr resPrompt = hPrompt->Fit(fit, "SNQ");

    nxrays_all[i] = resAll->Value(0)*resAll->Value(2)*rt2pi/hAll->GetXaxis()->GetBinWidth(1);
    nxrays_prompt[i] = resPrompt->Value(0)*resPrompt->Value(2)*rt2pi/hPrompt->GetXaxis()->GetBinWidth(1);
    nenter[i] = count_entering_muons(hEnter);

    exrays_all[i] = nxrays_all[i]*TMath::Sqrt(TMath::Power(resAll->ParError(0)/resAll->Value(0), 2.) + TMath::Power(resAll->ParError(2)/resAll->Value(2), 2.));
    exrays_prompt[i] = nxrays_prompt[i]*TMath::Sqrt(TMath::Power(resPrompt->ParError(0)/resPrompt->Value(0), 2.) + TMath::Power(resPrompt->ParError(2)/resPrompt->Value(2), 2.));

    chi2All[i] = resAll->Chi2()/resAll->Ndf();
    chi2Prompt[i] = resPrompt->Chi2()/resPrompt->Ndf();
  }

  Double_t n[nfiles];
  Double_t nper_all[nfiles], eper_all[nfiles];
  Double_t nper_prompt[nfiles], eper_prompt[nfiles];
  const Double_t emission_branching_ratio = 0.798;
  for (unsigned int i = 0; i < nfiles; ++i) {
    n[i] = (Double_t)i;
    nper_all[i] = nxrays_all[i]/eff->Eval(E_xray)/emission_branching_ratio/nenter[i];
    eper_all[i] = exrays_all[i]/nxrays_all[i]*nper_all[i];
    nper_prompt[i] = nxrays_prompt[i]/eff->Eval(E_xray)/emission_branching_ratio/nenter[i];
    eper_prompt[i] = exrays_prompt[i]/nxrays_prompt[i]*nper_prompt[i];
  }

  ////////////////////////////
  // Get fit for total dataset
  ////////////////////////////
  char fname[256];
  sprintf(fname, "%s.root", dataset);
  TFile* f = new TFile(fname, "READ");
  TH1 *hAll, *hPrompt, *hEnter;
  if (pp) {
    hAll = (TH1*)f->Get("pp_total_all");
    hPrompt = (TH1*)f->Get("pp_total_prompt");
    hEnter = (TH1*)f->Get("pp_total_enter");
  } else {
    hAll = (TH1*)f->Get("total_all");
    hPrompt = (TH1*)f->Get("total_prompt");
    hEnter = (TH1*)f->Get("total_enter");
  }
  const Double_t nper = count_entering_muons(hEnter);
  hAll->GetXaxis()->SetRangeUser(343., 354.);
  hPrompt->GetXaxis()->SetRangeUser(343., 354.);
  Double_t parAll[8], parPrompt[8];
  guess_parameters(hAll, parAll);
  guess_parameters(hPrompt, parPrompt);
  fit->SetParameters(parAll);
  fit->SetParLimits(0, 0., 50.*parAll[0]);
  fit->SetParLimits(1, 344., 349.);
  fit->SetParLimits(2, 0.5, 2.0);
  fit->SetParLimits(3, 0., 50.*parAll[3]);
  fit->SetParLimits(4, 349., 354.);
  fit->SetParLimits(5, 0.5, 2.0);
  TFitResultPtr resAll = hAll->Fit(fit, "SQ");
  fit->SetParameters(parPrompt);
  fit->SetParLimits(0, 0., 50.*parPrompt[0]);
  fit->SetParLimits(1, 344., 349.);
  fit->SetParLimits(2, 0.5, 2.0);
  fit->SetParLimits(3, 0., 50.*parPrompt[3]);
  fit->SetParLimits(4, 349., 354.);
  fit->SetParLimits(5, 0.5, 2.0);
  TFitResultPtr resPrompt = hPrompt->Fit(fit, "SQ");
  const Double_t allXRays = resAll->Value(0)*resAll->Value(2)*rt2pi/hAll->GetXaxis()->GetBinWidth(1);
  const Double_t eallXRays = allXRays*TMath::Sqrt(TMath::Power(resAll->ParError(0)/resAll->Value(0), 2.) + TMath::Power(resAll->ParError(2)/resAll->Value(2), 2.));
  const Double_t promptXRays = resPrompt->Value(0)*resPrompt->Value(2)*rt2pi/hPrompt->GetXaxis()->GetBinWidth(1);
  const Double_t epromptXRays = promptXRays*TMath::Sqrt(TMath::Power(resPrompt->ParError(0)/resPrompt->Value(0), 2.) + TMath::Power(resPrompt->ParError(2)/resPrompt->Value(2), 2.));
  const Double_t allNPer = allXRays/eff->Eval(E_xray)/emission_branching_ratio/nper;
  const Double_t eallNPer = eallXRays/allXRays*allNPer;
  const Double_t promptNPer = promptXRays/eff->Eval(E_xray)/emission_branching_ratio/nper;
  const Double_t epromptNPer = epromptXRays/promptXRays*promptNPer;

  //////////////////////////////
  // Get fit for dataset average
  //////////////////////////////
  TF1* linefit = new TF1("linefit", "[0]");
  TLegend* leg;
  TFitResultPtr res;
  char lab[128];

  TGraphErrors* gr_all = new TGraphErrors(nfiles, n, nper_all, 0x0, eper_all);
  TGraphErrors* gr_prompt = new TGraphErrors(nfiles, n, nper_prompt, 0x0, eper_prompt);
  gr_all->SetMarkerStyle(2); gr_all->SetMarkerColor(kRed); gr_all->SetLineColor(kRed);
  gr_prompt->SetMarkerStyle(2); gr_prompt->SetMarkerColor(kGreen); gr_prompt->SetLineColor(kGreen);
  leg = new TLegend(0.1,0.7,0.9,0.9); leg->SetNColumns(4);
  leg->AddEntry(gr_all, "", ""); leg->AddEntry(gr_all, "Fit (here)", ""); leg->AddEntry(gr_all, "Total (entire dataset)", ""); leg->AddEntry(gr_all, "#chi^{2}/ndf", "");
  leg->SetTextSize(0.03);
  linefit->SetParameter(0, 1.);
  res = gr_all->Fit(linefit, "SQ"); gr_all->GetFunction("linefit")->SetLineColor(kRed);
  sprintf(lab, "No time cuts"); leg->AddEntry(gr_all, lab, "lp");
  sprintf(lab, "%g +/- %.2g", res->Value(0), res->ParError(0)); leg->AddEntry(gr_all, lab, "");
  sprintf(lab, "%g +/- %.2g", allNPer, eallNPer); leg->AddEntry(gr_all, lab, "");
  sprintf(lab, "%g/%d", res->Chi2(),res->Ndf()); leg->AddEntry(gr_all, lab, "");
  linefit->SetParameter(0, 1.);
  res = gr_prompt->Fit(linefit, "SQ"); gr_prompt->GetFunction("linefit")->SetLineColor(kGreen);
  sprintf(lab, "Prompt"); leg->AddEntry(gr_prompt, lab, "lp");
  sprintf(lab, "%g +/- %.2g", res->Value(0), res->ParError(0)); leg->AddEntry(gr_all, lab, "");
  sprintf(lab, "%g +/- %.2g", promptNPer, epromptNPer); leg->AddEntry(gr_all, lab, "");
  sprintf(lab, "%g/%d", res->Chi2(),res->Ndf()); leg->AddEntry(gr_all, lab, "");
  char title[256];
  if (pp)
    sprintf(title, "Stops per entering muon (PP %s)", dataset);
  else
    sprintf(title, "Stops per entering muon (%s)", dataset);
  TMultiGraph* mg1 = new TMultiGraph("mg1", title);
  mg1->Add(gr_all); mg1->Add(gr_prompt);
  new TCanvas();
  mg1->Draw("AP"); mg1->GetXaxis()->SetTitle("Run Group (Groups of 10)"); mg1->GetYaxis()->SetTitle("Stops per Muon");
  leg->Draw("SAME");

  TGraph *gr_chi2All = new TGraph(nfiles, n, chi2All);
  TGraph *gr_chi2Prompt = new TGraph(nfiles, n, chi2Prompt);
  gr_chi2All->SetMarkerStyle(2); gr_chi2All->SetMarkerColor(kRed);
  gr_chi2Prompt->SetMarkerStyle(2); gr_chi2Prompt->SetMarkerColor(kGreen);
  if (pp)
    sprintf(title, "Reduced Chi2 of XRay Peak Fits (PP %s)", dataset);
  else
    sprintf(title, "Reduced Chi2 of XRay Peak Fits (%s)", dataset);
  TMultiGraph* mg2 = new TMultiGraph("mg2", title);
  mg2->Add(gr_chi2All); mg2->Add(gr_chi2Prompt);
  new TCanvas();
  mg2->Draw("AP"); mg2->GetXaxis()->SetTitle("Run Group (Groups of 10)"); mg2->GetYaxis()->SetTitle("Reduced #chi^{2}");
  leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->SetTextSize(0.04);
  leg->AddEntry(gr_chi2All, "No time cut");
  leg->AddEntry(gr_chi2Prompt, "Prompt");
  leg->Draw("SAME");
}
