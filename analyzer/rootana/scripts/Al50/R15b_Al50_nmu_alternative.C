#include "./src/plotters/SimplePulse.h"

#include "TChain.h"
#include "TCut.h"
#include "TFile.h"

void R15b_Al50_nmu_alternative(const char* ifnameglob="", const char* ofname="") {
  TChain* c = new TChain("TMETree/TMETree");
  c->Add(ifnameglob);
  if (c->GetEntries() <= 0) return;
  TFile* f = new TFile(ofname, "RECREATE");
  // TCut pp  = "timeToPrevTME > 10e3 && timeToNextTME > 10e3";
  TCut pp  = "anyDoubleCountedPulses==0";
  TCut pws[2] = { "-200 < GeLoGain.tTME && GeLoGain.tTME < 200",
                  "-200 < GeHiGain.tTME && GeHiGain.tTME < 200" };
  TCut pwc[2] = { "200  < GeLoGain.tTME && GeLoGain.tTME < 4000",
                  "200  < GeHiGain.tTME && GeHiGain.tTME < 4000" };
  TCut oot[2] = { "4000 < GeLoGain.tTME",
                  "4000 < GeHiGain.tTME" };

  c->Draw("GeLoGain.E>>hGeLoGain_energy(8777, 0, 3e3)",            pp,           "goff");
  c->Draw("GeLoGain.E>>hGeLoGain_energy_prompt(8777, 0, 3e3)",     pp && pws[0], "goff");
  c->Draw("GeLoGain.E>>hGeLoGain_energy_semiprompt(8777, 0, 3e3)", pp && pwc[0], "goff");
  c->Draw("GeLoGain.E>>hGeLoGain_energy_delayed(8777, 0, 3e3)",    pp && oot[0], "goff");
  c->Draw("GeHiGain.E>>hGeHiGain_energy(8777, 0, 3e3)",            pp,           "goff");
  c->Draw("GeHiGain.E>>hGeHiGain_energy_prompt(8777, 0, 3e3)",     pp && pws[1], "goff");
  c->Draw("GeHiGain.E>>hGeHiGain_energy_semiprompt(8777, 0, 3e3)", pp && pwc[1], "goff");
  c->Draw("GeHiGain.E>>hGeHiGain_energy_delayed(8777, 0, 3e3)",    pp && oot[1], "goff");
  f->Write();
}
