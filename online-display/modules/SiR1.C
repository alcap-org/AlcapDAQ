void SiR1(std::string hist_type)
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(2,2);
  /*****************************************************************/
  std::string ch_name[]={"D702", "D703", "D704", "D705"};
  for(int i = 0; i < 4; i++) {
    TH1* hist = get_histogram(ch_name[i], hist_type);
    if (!hist) continue;
    AlCapCanvas->cd(i+1);
    if(hist_type.compare("Shapes") == 0) {
      hist->Draw("COLZ");
      gPad->SetLogz();
    } else {
      hist->Draw();
      gPad->SetLogy();
    }
  }
}
