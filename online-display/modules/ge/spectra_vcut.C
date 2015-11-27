void spectra_vcut()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(1,2);

  // noprot: No veto protection
  // cpprot: Charged particle veto protection 
  TH1* hge1_spec_noprot = ((TH2*)getObject("h2Ge1SpectraMuCentered"))->
    ProjectionY();
  TH1* hge2_spec_noprot = ((TH2*)getObject("h2Ge2SpectraMuCentered"))->
    ProjectionY();
  TH1* hge1_spec_cpprot = ((TH2*)getObject("h2Ge1SpectraMuCenteredCPProtected"))->
    ProjectionY();
  TH1* hge2_spec_cpprot = ((TH2*)getObject("h2Ge2SpectraMuCenteredCPProtected"))->
    ProjectionY();
  hge1_spec_noprot->Add(hge1_spec_cpprot, -1.);
  hge2_spec_noprot->Add(hge2_spec_cpprot, -1.);
  
  hge1_spec_noprot->SetLineColor(kRed);   hge2_spec_noprot->SetLineColor(kRed);
  hge1_spec_cpprot->SetLineColor(kBlack); hge2_spec_cpprot->SetLineColor(kBlack);

  hge1_spec_noprot->SetStats(0); hge2_spec_noprot->SetStats(0);
  hge1_spec_cpprot->SetStats(0); hge2_spec_cpprot->SetStats(0);
  
  TLegend* leg = new TLegend(0.7, 0.5, 0.9, 0.9);
  leg->SetHeader("Veto Protection");
  leg->AddEntry(hge1_spec_cpprot, "Veto protected");
  leg->AddEntry(hge1_spec_noprot, "Vetoed");

  AlCapCanvas->cd(1);
  hge1_spec_cpprot->Draw(); hge1_spec_noprot->Draw("SAME");
  leg->Draw("SAME");
  AlCapCanvas->cd(2);
  hge2_spec_cpprot->Draw(); hge2_spec_noprot->Draw("SAME");
  leg->Draw("SAME");
  
}
