// Plot the MC and data EvdE plots on top of eachother.
{
  TFile* frl = new TFile("data/pselal50.root");
  TFile* fmc = new TFile("data/mcdeve.root");
  TTree* trrl[5][2] = {
    { (TTree*)frl->Get("PID_LP"),        (TTree*)frl->Get("PID_RP") },
    { (TTree*)frl->Get("PID_LD"),        (TTree*)frl->Get("PID_RD") },
    { (TTree*)frl->Get("PID_LT"),        (TTree*)frl->Get("PID_RT") },
    { (TTree*)frl->Get("PID_LA"),        (TTree*)frl->Get("PID_RA") },
    { (TTree*)frl->Get("PID_LU"),        (TTree*)frl->Get("PID_RU") },

  };
  TTree* trmc[2] = {
    (TTree*)fmc->Get("proton_L"), (TTree*)fmc->Get("proton_R")
  };
  trrl[0][0]->Draw("e1:e2>>hrllevde(500,0,20e3,500,0,2e3)",         "e2 > 0");
  trrl[0][1]->Draw("e1:e2>>hrlrevde(500,0,20e3,500,0,2e3)",         "e2 > 0");
  trrl[1][0]->Draw("e1:e2>>+hrllevde",         "e2 > 0");
  trrl[1][1]->Draw("e1:e2>>+hrlrevde",         "e2 > 0");
  trrl[2][0]->Draw("e1:e2>>+hrllevde",         "e2 > 0");
  trrl[2][1]->Draw("e1:e2>>+hrlrevde",         "e2 > 0");
  trrl[3][0]->Draw("e1:e2>>+hrllevde",         "e2 > 0");
  trrl[3][1]->Draw("e1:e2>>+hrlrevde",         "e2 > 0");
  trrl[4][0]->Draw("e1:e2>>+hrllevde",         "e2 > 0");
  trrl[4][1]->Draw("e1:e2>>+hrlrevde",         "e2 > 0");
  trmc[0]->Draw("e1:e2>>hmclevde_stop(500,0,20e3,100,0,2e3)",    "stop==2");
  trmc[1]->Draw("e1:e2>>hmcrevde_stop(500,0,20e3,100,0,2e3)",    "stop==2");
  trmc[0]->Draw("e1:e2>>hmclevde_nonstop(500,0,20e3,100,0,2e3)", "stop==3");
  trmc[1]->Draw("e1:e2>>hmcrevde_nonstop(500,0,20e3,100,0,2e3)", "stop==3");

  TH2* hrlevde[2]         = { (TH2*)gDirectory->Get("hrllevde"),
                              (TH2*)gDirectory->Get("hrlrevde") };
  TH2* hmcevde_stop[2]    = { (TH2*)gDirectory->Get("hmclevde_stop"),
                              (TH2*)gDirectory->Get("hmcrevde_stop") };
  TH2* hmcevde_nonstop[2] = { (TH2*)gDirectory->Get("hmclevde_nonstop"),
                              (TH2*)gDirectory->Get("hmcrevde_nonstop") };

  hrlevde[0]        ->SetMarkerColor(kBlack);
  hrlevde[1]        ->SetMarkerColor(kBlack);
  hmcevde_stop[0]   ->SetMarkerColor(kRed);
  hmcevde_stop[1]   ->SetMarkerColor(kRed);
  hmcevde_nonstop[0]->SetMarkerColor(kBlue);
  hmcevde_nonstop[1]->SetMarkerColor(kBlue);

  hmcevde_stop[0]->SetTitle("MC+Data SiL;SiL3 [keV];SiL1 [keV]");
  hmcevde_stop[0]->Draw();
  hmcevde_nonstop[0]->Draw("SAME");
  hrlevde[0]->Draw("SAME");
}
