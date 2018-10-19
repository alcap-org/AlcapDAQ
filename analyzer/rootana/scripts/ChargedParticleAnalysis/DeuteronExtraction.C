#include "ExtractBand.C"

void DeuteronExtraction() {

  // Cut to remove the remaining deuteron band                                                                                                      
  TF1* deuteron_cut_eqn = new TF1("deuteron_cut_eqn", "[0]*TMath::Exp([1]*x) + [2]", 0, 25000);
  deuteron_cut_eqn->SetLineColor(kBlack);
  deuteron_cut_eqn->SetParameter(0, 4500);
  deuteron_cut_eqn->SetParameter(1, -0.0004);
  deuteron_cut_eqn->SetParameter(2, 500);
  Cut* deuteron_cut = new Cut(deuteron_cut_eqn, true);

  TF1* deuteron_upper_cut_eqn = new TF1("deuteron_upper_cut_eqn", "[0]*TMath::Exp([1]*x) + [2]", 0, 25000);
  deuteron_upper_cut_eqn->SetLineColor(kBlack);
  deuteron_upper_cut_eqn->SetParameter(0, 5500);
  deuteron_upper_cut_eqn->SetParameter(1, -0.00035);
  deuteron_upper_cut_eqn->SetParameter(2, 750);
  Cut* deuteron_upper_cut = new Cut(deuteron_upper_cut_eqn, false);

  std::vector<Cut*> cuts;
  cuts.push_back(deuteron_cut); cuts.push_back(deuteron_upper_cut);

  //  ExtractBand("all-tin-absorber.root", cuts);
  //  ExtractBand("~/data/out/v94/total.root", cuts); // Andy Al50 analysis
  ExtractBand("~/data/out/v96/Al100.root", cuts); // Andy Al100 analysis
}
