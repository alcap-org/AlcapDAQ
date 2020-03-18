void convertCuts_noCutoffs() {

  //  std::string filename = "~/data/results/Si16b/mark-al50-cuts.root";
  //  std::string filename = "~/data/results/Si16b/mark-al100-cuts.root";
  //  std::string filename = "~/data/results/Si16b/mark-al50-sir3-cuts.root";
  std::string filename = "~/data/results/Si16b/mark-si16b-cuts.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  //  std::string outfilename = "~/data/results/Si16b/mark-al50-cuts-keV.root";
  //  std::string outfilename = "~/data/results/Si16b/mark-al100-cuts-keV.root";
  //  std::string outfilename = "~/data/results/Si16b/mark-al50-sir3-cuts-keV.root";
  std::string outfilename = "~/data/results/Si16b/mark-si16b-cuts-keV.root";
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");

  TIter next(file->GetListOfKeys());
  TKey *key;

  while ((key = (TKey*)next())) {
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if (!cl->InheritsFrom("TCutG")) { continue; }
    TCutG *cut = (TCutG*)key->ReadObj();

    std::string new_name = cut->GetName();
    new_name += "_keV";
    std::cout << new_name << std::endl;
    TCutG* new_cut = new TCutG(new_name.c_str(), cut->GetN());

    for (int i_point = 0; i_point < cut->GetN(); ++i_point) {
      double x, y;
      cut->GetPoint(i_point, x, y);
            
      new_cut->SetPoint(i_point, x*1000, y*1000);
    }
    new_cut->Write();
  }
  
  outfile->Write();
  outfile->Close();
}
