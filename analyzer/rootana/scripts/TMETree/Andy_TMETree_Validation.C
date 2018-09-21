void Andy_TMETree_Validation() {
  TFile* before_file = new TFile("/data/ssd2/R15bAnalysis/tmetree/Si16b/out10363.root", "READ");
  TTree* before_tmetree = (TTree*) before_file->Get("TMETree/TMETree");

  TFile* after_file = new TFile("~/data/out/local/out10363_tme-tree.root", "READ");
  TTree* after_tmetree = (TTree*) after_file->Get("TMETree/TMETree");


  std::string drawcmd = "centralMuonTPIID>>";
  std::string before_drawcmd = drawcmd + "hbefore";
  before_tmetree->Draw(before_drawcmd.c_str(), "centralMuonChannel==\"SiT-4-S\"");

  TH1F* hafter = (TH1F*) hbefore->Clone("hafter");
  hafter->Reset();
  std::string after_drawcmd = drawcmd + "hafter";
  after_tmetree->Draw(after_drawcmd.c_str(), "centralMuonChannel==\"SiT-4-S\"", "SAMES");

  TH1F* hsub = (TH1F*) hbefore->Clone("hsub");
  hsub->Add(hafter, -1);
  hsub->Draw();
}
