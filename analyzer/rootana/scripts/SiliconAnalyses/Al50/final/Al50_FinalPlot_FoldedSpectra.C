void Al50_FinalPlot_FoldedSpectra() {

  TFile* file = new TFile("~m-wong/data/R15b/al50.root", "READ");
  TTree* tree = (TTree*) file->Get("tree");
  
  double min_energy = 0;
  double max_energy = 25;
  double energy_width = 0.1;
  int n_energy_bins = (max_energy - min_energy) / energy_width;

  TH1F* hProtons = new TH1F("hprotons", "", n_energy_bins,min_energy,max_energy);
  tree->Draw("e1+e2>>hprotons", "TMath::IsNaN(e3) && sig2.Contains(\"proton\") && t2>400 && t2<10e3 && abs(t2-t1)<200 && timeToPrevTME>10e3 && timeToNextTME>10e3 && channel.Contains(\"SiR\")", "goff");

  TH1F* hDeuterons = new TH1F("hdeuterons", "", n_energy_bins,min_energy,max_energy);
  tree->Draw("e1+e2>>hdeuterons", "TMath::IsNaN(e3) && sig2.Contains(\"deuteron\") && t2>400 && t2<10e3 && abs(t2-t1)<200 && timeToPrevTME>10e3 && timeToNextTME>10e3 && channel.Contains(\"SiR\")", "goff");

  TH1F* hTritons = new TH1F("htritons", "", n_energy_bins,min_energy,max_energy);
  tree->Draw("e1+e2>>htritons", "TMath::IsNaN(e3) && sig2.Contains(\"triton\") && t2>400 && t2<10e3 && abs(t2-t1)<200 && timeToPrevTME>10e3 && timeToNextTME>10e3 && channel.Contains(\"SiR\")", "goff");

  max_energy = 30;
  n_energy_bins = (max_energy - min_energy) / energy_width;
  TH1F* hAlphas = new TH1F("halphas", "", n_energy_bins,min_energy,max_energy);
  tree->Draw("e1+e2>>halphas", "TMath::IsNaN(e3) && sig2.Contains(\"alpha\") && t2>400 && t2<10e3 && abs(t2-t1)<200 && timeToPrevTME>10e3 && timeToNextTME>10e3 && channel.Contains(\"SiR\")", "goff");

  TFile* outfile = new TFile("~/data/results/Al50/raw_spectra_fromMarkTree.root", "RECREATE");
  hProtons->Write();
  hDeuterons->Write();
  hTritons->Write();
  hAlphas->Write();
  outfile->Write();
  outfile->Close();


}
