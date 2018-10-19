#include "utils.C"

void andy_roofit() {

  const int rebin_factor = 4;

  TFile* file = new TFile("~/data/out/v104/Al100_nam_subset.root");
  TH2* hGeEnergyTime = ((TH2*)file->Get("GeSpectrum/hTimeEnergy"));
  TH1* hGeEnergyPrompt = hGeEnergyTime->ProjectionY("_py", hGeEnergyTime->GetXaxis()->FindBin(-500), hGeEnergyTime->GetXaxis()->FindBin(500));
  hGeEnergyPrompt->Rebin(rebin_factor);

  //  TFile* file = new TFile("Al100_Xray.root");
  //  TH1* hGeEnergyPrompt = getCalSpec((TH1*)file->Get("hGeS_500ns"));
  //  hGeEnergyPrompt->Rebin(rebin_factor);

  RooWorkspace* ws = new RooWorkspace("ws", kTRUE);
  ws->factory("Polynomial::pol1_bkg(edep[343.7, 355], {bkg_offset[-10, 100], bkg_slope[-0.1, 0.1]})");
  ws->factory("Gaussian::peak1_pdf(edep[343.7, 355], mean1[345, 347], sigma1[0.1, 10])");
  ws->factory("Gaussian::peak2_pdf(edep[343.7, 355], mean2[351, 352], sigma2[0.1, 10])");
  ws->factory("SUM::sum(amp1[0,10000]*peak1_pdf, amp2[0,10000]*peak2_pdf, nbkg[0, 10000]*pol1_bkg)");

  RooDataHist data("data", "Ge Spectrum",ws::edep, hGeEnergyPrompt);
  /*
  RooStats::ModelConfig* model_config = new RooStats::ModelConfig("model_config", ws);
  model_config->SetPdf(ws::sum);
  model_config->SetParametersOfInterest(RooArgSet(ws::amp1, ws::amp2));
  model_config->SetObservables(RooArgSet(ws::edep));
  model_config->SetNuisanceParameters(RooArgSet(ws::nbkg));
    
  RooStats::ProfileLikelihoodCalculator* pllCal = new RooStats::ProfileLikelihoodCalculator(data, *model_config);
  pllCal->SetConfidenceLevel(0.683);
  RooStats::LikelihoodInterval* interval = pllCal->GetInterval();
  RooArgSet* POIs = model_config->GetParametersOfInterest();
  */
  ws::sum.fitTo(data);
  
  RooPlot* Eframe = ws::edep.frame();
  data.plotOn(Eframe);
  ws::sum.plotOn(Eframe);
  Eframe->Draw();

  std::cout << "Output parameters:" << std::endl;
  ws::amp1.Print();
  ws::mean1.Print();
  ws::sigma1.Print();
  ws::amp2.Print();
  ws::mean2.Print();
  ws::sigma2.Print();
  ws::nbkg.Print();
  ws::bkg_offset.Print();
  ws::bkg_slope.Print();
}
