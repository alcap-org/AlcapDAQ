#include "scripts/PlotTME_Si16b_EvdE/ExtractBand.C"

void ProtonBandExtraction() {
  double x_1 = 0, y_1 = 2000, x_2 = 4000, y_2 = 0;
  double gradient = (y_2 - y_1) / (x_2 - x_1);
  double offset = y_1;
  TF1* electron_spot_cut_eqn = new TF1("electron_spot_cut_eqn", "[0]*x + [1]", 0, 15000);
  electron_spot_cut_eqn->SetLineColor(kBlack);
  electron_spot_cut_eqn->SetParameter(0, gradient);
  electron_spot_cut_eqn->SetParameter(1, offset);
  Cut* electron_spot_cut = new Cut(electron_spot_cut_eqn, true);

  double punch_through_yoffset = 300;
  TF1* punch_through_cut_eqn = new TF1("punch_through_cut_eqn", "[0]", 0, 25000);
  punch_through_cut_eqn->SetLineColor(kBlack);
  punch_through_cut_eqn->SetParameter(0, punch_through_yoffset);
  Cut* punch_through_cut = new Cut(punch_through_cut_eqn, true);

  // Cut to remove the remaining deuteron band                                                                                                      
  TF1* deuteron_cut_eqn = new TF1("deuteron_cut_eqn", "[0]*TMath::Exp([1]*x) + [2]", 0, 25000);
  deuteron_cut_eqn->SetLineColor(kBlack);
  deuteron_cut_eqn->SetParameter(0, 4500);
  deuteron_cut_eqn->SetParameter(1, -0.0004);
  deuteron_cut_eqn->SetParameter(2, 500);
  Cut* deuteron_cut = new Cut(deuteron_cut_eqn, false);

  std::vector<Cut*> cuts;
  cuts.push_back(electron_spot_cut); cuts.push_back(punch_through_cut); cuts.push_back(deuteron_cut);

  //  ExtractBand("all-tin-absorber.root", cuts);
  ExtractBand("~/data/out/v22/Si16b.root", cuts, "PlotTME_Si16b_EvdE", "SiRHits", "SiR1Energy+SiR2Energy", "SiR1Energy");
}
