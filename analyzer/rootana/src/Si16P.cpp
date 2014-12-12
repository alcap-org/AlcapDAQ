#include "Si16P.h"
#include "RegisterModule.inc"
#include "TPulseIsland.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "IdGenerator.h"
#include "SetupNavigator.h"
#include "ExportPulse.h"
#include "PulseCandidateFinder.h"

#include "TH1I.h"
#include "TH2D.h"
#include "TF1.h"
#include "TDirectory.h"

#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <climits>
#include <string>
#include <sstream>

const IDs::channel Si16P::fMuSc  (IDs::kMuSc,   IDs::kNotApplicable);
const IDs::channel Si16P::fSiR11S(IDs::kSiR1_1, IDs::kSlow);
const IDs::channel Si16P::fSiR12S(IDs::kSiR1_2, IDs::kSlow);
const IDs::channel Si16P::fSiR13S(IDs::kSiR1_3, IDs::kSlow);
const IDs::channel Si16P::fSiR14S(IDs::kSiR1_4, IDs::kSlow);
const IDs::channel Si16P::fSiR2S (IDs::kSiR2,   IDs::kSlow);
const IDs::channel Si16P::fSiL11S(IDs::kSiL1_1, IDs::kSlow);
const IDs::channel Si16P::fSiL12S(IDs::kSiL1_2, IDs::kSlow);
const IDs::channel Si16P::fSiL13S(IDs::kSiL1_3, IDs::kSlow);
const IDs::channel Si16P::fSiL14S(IDs::kSiL1_4, IDs::kSlow);
const IDs::channel Si16P::fSiL2S (IDs::kSiL2,   IDs::kSlow);

const IDs::generator Si16P::fGenerator("FirstComplete", "{constant_fraction=0.60}");
const double         Si16P::fConstantFraction(0.60);

Si16P::Si16P(modules::options* opts) :
  BaseModule("Si16P",opts),
  fHist_PIDRight(NULL), fHist_PIDLeft(NULL),
  fMBAmp_MuSc  (SetupNavigator::Instance()->GetPedestal(fMuSc),   TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fMuSc  .str()))),
  fMBAmp_SiR11S(SetupNavigator::Instance()->GetPedestal(fSiR11S), TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiR11S.str()))),
  fMBAmp_SiR12S(SetupNavigator::Instance()->GetPedestal(fSiR12S), TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiR12S.str()))),
  fMBAmp_SiR13S(SetupNavigator::Instance()->GetPedestal(fSiR13S), TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiR13S.str()))),
  fMBAmp_SiR14S(SetupNavigator::Instance()->GetPedestal(fSiR14S), TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiR14S.str()))),
  fMBAmp_SiR2S (SetupNavigator::Instance()->GetPedestal(fSiR2S),  TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiR2S .str()))),
  fMBAmp_SiL11S(SetupNavigator::Instance()->GetPedestal(fSiL11S), TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiL11S.str()))),
  fMBAmp_SiL12S(SetupNavigator::Instance()->GetPedestal(fSiL12S), TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiL12S.str()))),
  fMBAmp_SiL13S(SetupNavigator::Instance()->GetPedestal(fSiL13S), TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiL13S.str()))),
  fMBAmp_SiL14S(SetupNavigator::Instance()->GetPedestal(fSiL14S), TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiL14S.str()))),
  fMBAmp_SiL2S (SetupNavigator::Instance()->GetPedestal(fSiL2S),  TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiL2S .str()))),
  fCFTime_MuSc  (SetupNavigator::Instance()->GetPedestal(fMuSc),
		 TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fMuSc.str())),
		 TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fMuSc.str())),
		 0.,
		 Si16P::fConstantFraction),
  fCFTime_SiR11S(SetupNavigator::Instance()->GetPedestal(fSiR11S),
		 TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiR11S.str())),
		 TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fSiR11S.str())),
		 SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(Si16P::fSiR11S, Si16P::fGenerator)),
		 Si16P::fConstantFraction),
  fCFTime_SiR12S(SetupNavigator::Instance()->GetPedestal(fSiR12S),
		 TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiR12S.str())),
		 TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fSiR12S.str())),
		 SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(Si16P::fSiR12S, Si16P::fGenerator)),
		 Si16P::fConstantFraction),
  fCFTime_SiR13S(SetupNavigator::Instance()->GetPedestal(fSiR13S),
		 TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiR13S.str())),
		 TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fSiR13S.str())),
		 SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(Si16P::fSiR13S, Si16P::fGenerator)),
		 Si16P::fConstantFraction),
  fCFTime_SiR14S(SetupNavigator::Instance()->GetPedestal(fSiR14S),
		 TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiR14S.str())),
		 TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fSiR14S.str())),
		 SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(Si16P::fSiR14S, Si16P::fGenerator)),
		 Si16P::fConstantFraction),
  fCFTime_SiR2S (SetupNavigator::Instance()->GetPedestal(fSiR2S),
		 TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiR2S.str())),
		 TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fSiR2S.str())),
		 SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(Si16P::fSiR2S, Si16P::fGenerator)),
		 Si16P::fConstantFraction),
  fCFTime_SiL11S(SetupNavigator::Instance()->GetPedestal(fSiL11S),
		 TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiL11S.str())),
		 TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fSiL11S.str())),
		 SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(Si16P::fSiL11S, Si16P::fGenerator)),
		 Si16P::fConstantFraction),
  fCFTime_SiL12S(SetupNavigator::Instance()->GetPedestal(fSiL12S),
		 TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiL12S.str())),
		 TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fSiL12S.str())),
		 SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(Si16P::fSiL12S, Si16P::fGenerator)),
		 Si16P::fConstantFraction),
  fCFTime_SiL13S(SetupNavigator::Instance()->GetPedestal(fSiL13S),
		 TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiL13S.str())),
		 TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fSiL13S.str())),
		 SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(Si16P::fSiL13S, Si16P::fGenerator)),
		 Si16P::fConstantFraction),
  fCFTime_SiL14S(SetupNavigator::Instance()->GetPedestal(fSiL14S),
		 TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiL14S.str())),
		 TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fSiL14S.str())),
		 SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(Si16P::fSiL14S, Si16P::fGenerator)),
		 Si16P::fConstantFraction),
  fCFTime_SiL2S (SetupNavigator::Instance()->GetPedestal(fSiL2S),
		 TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiL2S.str())),
		 TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fSiL2S.str())),
		 SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(Si16P::fSiL2S, Si16P::fGenerator)),
		 Si16P::fConstantFraction),
  fADC2E_SiR11S(new TF1("adc2e_sir11s", "[0]+[1]*x")),
  fADC2E_SiR12S(new TF1("adc2e_sir12s", "[0]+[1]*x")),
  fADC2E_SiR13S(new TF1("adc2e_sir13s", "[0]+[1]*x")),
  fADC2E_SiR14S(new TF1("adc2e_sir14s", "[0]+[1]*x")),
  fADC2E_SiR2S (new TF1("adc2e_sir2s",  "[0]+[1]*x")),
  fADC2E_SiL11S(new TF1("adc2e_sil11s", "[0]+[1]*x")),
  fADC2E_SiL12S(new TF1("adc2e_sil12s", "[0]+[1]*x")),
  fADC2E_SiL13S(new TF1("adc2e_sil13s", "[0]+[1]*x")),
  fADC2E_SiL14S(new TF1("adc2e_sil14s", "[0]+[1]*x")),
  fADC2E_SiL2S (new TF1("adc2e_sil2s",  "[0]+[1]*x")),
  fdTMuSc(opts->GetDouble("tcut_musc")),       fdTPID(opts->GetDouble("tcut_pid")),
  fdTScatter(opts->GetDouble("tcut_scatter")), fdTDetectorPileup(opts->GetDouble("tcut_detpp")),
  fdTSiLow(opts->GetDouble("tcut_si_low")),    fdTSiHigh(opts->GetDouble("tcut_si_high")),
  fMuonCut(opts->GetDouble("adccut_musc")) {

  const int nbins_dE = std::pow(2., 12);
  const int nbins_E  = std::pow(2., 12);

  // Set ADC->E functions' parameters
  const std::pair<double,double> adc2epar_sir11s = SetupNavigator::Instance()->GetEnergyCalibrationConstants(Si16P::fSiR11S);
  const std::pair<double,double> adc2epar_sir12s = SetupNavigator::Instance()->GetEnergyCalibrationConstants(Si16P::fSiR12S);
  const std::pair<double,double> adc2epar_sir13s = SetupNavigator::Instance()->GetEnergyCalibrationConstants(Si16P::fSiR13S);
  const std::pair<double,double> adc2epar_sir14s = SetupNavigator::Instance()->GetEnergyCalibrationConstants(Si16P::fSiR14S);
  const std::pair<double,double> adc2epar_sir2s  = SetupNavigator::Instance()->GetEnergyCalibrationConstants(Si16P::fSiR2S );
  const std::pair<double,double> adc2epar_sil11s = SetupNavigator::Instance()->GetEnergyCalibrationConstants(Si16P::fSiL11S);
  const std::pair<double,double> adc2epar_sil12s = SetupNavigator::Instance()->GetEnergyCalibrationConstants(Si16P::fSiL12S);
  const std::pair<double,double> adc2epar_sil13s = SetupNavigator::Instance()->GetEnergyCalibrationConstants(Si16P::fSiL13S);
  const std::pair<double,double> adc2epar_sil14s = SetupNavigator::Instance()->GetEnergyCalibrationConstants(Si16P::fSiL14S);
  const std::pair<double,double> adc2epar_sil2s  = SetupNavigator::Instance()->GetEnergyCalibrationConstants(Si16P::fSiL2S );
  fADC2E_SiR11S->SetParameters(adc2epar_sir11s.first, adc2epar_sir11s.second);
  fADC2E_SiR12S->SetParameters(adc2epar_sir12s.first, adc2epar_sir12s.second);
  fADC2E_SiR13S->SetParameters(adc2epar_sir13s.first, adc2epar_sir13s.second);
  fADC2E_SiR14S->SetParameters(adc2epar_sir14s.first, adc2epar_sir14s.second);
  fADC2E_SiR2S ->SetParameters(adc2epar_sir2s .first, adc2epar_sir2s .second);
  fADC2E_SiL11S->SetParameters(adc2epar_sil11s.first, adc2epar_sil11s.second);
  fADC2E_SiL12S->SetParameters(adc2epar_sil12s.first, adc2epar_sil12s.second);
  fADC2E_SiL13S->SetParameters(adc2epar_sil13s.first, adc2epar_sil13s.second);
  fADC2E_SiL14S->SetParameters(adc2epar_sil14s.first, adc2epar_sil14s.second);
  fADC2E_SiL2S ->SetParameters(adc2epar_sil2s .first, adc2epar_sil2s .second);

  TDirectory* cwd = TDirectory::CurrentDirectory();
  dir->cd();
  fHist_PIDRight = new TH2D("hPIDRight", "PID (Right);E+dE (keV);dE (keV);Counts",
			    nbins_E,  fADC2E_SiR2S->Eval(0.),  fADC2E_SiR2S->Eval(nbins_E),
			    nbins_dE, fADC2E_SiR11S->Eval(0.), fADC2E_SiR11S->Eval(nbins_dE));
  fHist_PIDLeft  = new TH2D("hPIDLeft",  "PID (Right);E+dE (keV);dE (keV);Counts",
			    nbins_E,  fADC2E_SiL2S->Eval(0.),  fADC2E_SiL2S->Eval(nbins_E),
			    nbins_dE, fADC2E_SiL11S->Eval(0.), fADC2E_SiL11S->Eval(nbins_dE));
  cwd->cd();
  ThrowIfInputsInsane(opts);
}

Si16P::~Si16P(){
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int Si16P::BeforeFirstEntry(TGlobalData* gData, const TSetupData *setup){
  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int Si16P::ProcessEntry(TGlobalData* gData, const TSetupData *setup){

  const std::map< std::string, std::vector<TPulseIsland*> >& TPIMap = gData->fPulseIslandToChannelMap;

  static const std::string bank_musc   = TSetupData::Instance()->GetBankName(Si16P::fMuSc.str());
  static const std::string bank_sir11s = TSetupData::Instance()->GetBankName(Si16P::fSiR11S.str());
  static const std::string bank_sir12s = TSetupData::Instance()->GetBankName(Si16P::fSiR12S.str());
  static const std::string bank_sir13s = TSetupData::Instance()->GetBankName(Si16P::fSiR13S.str());
  static const std::string bank_sir14s = TSetupData::Instance()->GetBankName(Si16P::fSiR14S.str());
  static const std::string bank_sir2s  = TSetupData::Instance()->GetBankName(Si16P::fSiR2S.str());
  static const std::string bank_sil11s = TSetupData::Instance()->GetBankName(Si16P::fSiL11S.str());
  static const std::string bank_sil12s = TSetupData::Instance()->GetBankName(Si16P::fSiL12S.str());
  static const std::string bank_sil13s = TSetupData::Instance()->GetBankName(Si16P::fSiL13S.str());
  static const std::string bank_sil14s = TSetupData::Instance()->GetBankName(Si16P::fSiL14S.str());
  static const std::string bank_sil2s  = TSetupData::Instance()->GetBankName(Si16P::fSiL2S.str());

  const std::vector<double> muScTimes   = CalculateTimes(Si16P::fMuSc,   TPIMap.at(bank_musc));
  const std::vector<double> siR11STimes = CalculateTimes(Si16P::fSiR11S, TPIMap.at(bank_sir11s));
  const std::vector<double> siR12STimes = CalculateTimes(Si16P::fSiR12S, TPIMap.at(bank_sir12s));
  const std::vector<double> siR13STimes = CalculateTimes(Si16P::fSiR13S, TPIMap.at(bank_sir13s));
  const std::vector<double> siR14STimes = CalculateTimes(Si16P::fSiR14S, TPIMap.at(bank_sir14s));
  const std::vector<double> siR2STimes  = CalculateTimes(Si16P::fSiR2S,  TPIMap.at(bank_sir2s));
  const std::vector<double> siL11STimes = CalculateTimes(Si16P::fSiL11S, TPIMap.at(bank_sil11s));
  const std::vector<double> siL12STimes = CalculateTimes(Si16P::fSiL12S, TPIMap.at(bank_sil12s));
  const std::vector<double> siL13STimes = CalculateTimes(Si16P::fSiL13S, TPIMap.at(bank_sil13s));
  const std::vector<double> siL14STimes = CalculateTimes(Si16P::fSiL14S, TPIMap.at(bank_sil14s));
  const std::vector<double> siL2STimes  = CalculateTimes(Si16P::fSiL2S,  TPIMap.at(bank_sil2s));

  const std::vector<double> muScEnergies   = CalculateEnergies(Si16P::fMuSc,   TPIMap.at(bank_musc));
  const std::vector<double> siR11SEnergies = CalculateEnergies(Si16P::fSiR11S, TPIMap.at(bank_sir11s));
  const std::vector<double> siR12SEnergies = CalculateEnergies(Si16P::fSiR12S, TPIMap.at(bank_sir12s));
  const std::vector<double> siR13SEnergies = CalculateEnergies(Si16P::fSiR13S, TPIMap.at(bank_sir13s));
  const std::vector<double> siR14SEnergies = CalculateEnergies(Si16P::fSiR14S, TPIMap.at(bank_sir14s));
  const std::vector<double> siR2SEnergies  = CalculateEnergies(Si16P::fSiR2S,  TPIMap.at(bank_sir2s));
  const std::vector<double> siL11SEnergies = CalculateEnergies(Si16P::fSiL11S, TPIMap.at(bank_sil11s));
  const std::vector<double> siL12SEnergies = CalculateEnergies(Si16P::fSiL12S, TPIMap.at(bank_sil12s));
  const std::vector<double> siL13SEnergies = CalculateEnergies(Si16P::fSiL13S, TPIMap.at(bank_sil13s));
  const std::vector<double> siL14SEnergies = CalculateEnergies(Si16P::fSiL14S, TPIMap.at(bank_sil14s));
  const std::vector<double> siL2SEnergies  = CalculateEnergies(Si16P::fSiL2S,  TPIMap.at(bank_sil2s));

  //*************************//
  //**** Find Candidates ****//
  //*************************//
  for (unsigned int iMuSc = 0; iMuSc < muScTimes.size(); ++iMuSc) {
    // Reject electrons
    if (muScEnergies[iMuSc] < fMuonCut)
      continue;

    // Reject e/mu entering soon before or after
    if ( (iMuSc != 0                    && muScTimes[iMuSc]   - muScTimes[iMuSc-1] < fdTMuSc) || 
	 (iMuSc != muScTimes.size() - 1 && muScTimes[iMuSc+1] - muScTimes[iMuSc]   < fdTMuSc) )
      continue;

    // Get first hit after muSc
    unsigned int iSiR11S = std::upper_bound(siR11STimes.begin(), siR11STimes.end(), muScTimes[iMuSc]) - siR11STimes.begin();
    unsigned int iSiR12S = std::upper_bound(siR12STimes.begin(), siR12STimes.end(), muScTimes[iMuSc]) - siR12STimes.begin();
    unsigned int iSiR13S = std::upper_bound(siR13STimes.begin(), siR13STimes.end(), muScTimes[iMuSc]) - siR13STimes.begin();
    unsigned int iSiR14S = std::upper_bound(siR14STimes.begin(), siR14STimes.end(), muScTimes[iMuSc]) - siR14STimes.begin();
    unsigned int iSiR2S  = std::upper_bound(siR2STimes.begin(),  siR2STimes.end(),  muScTimes[iMuSc]) - siR2STimes.begin();
    unsigned int iSiL11S = std::upper_bound(siL11STimes.begin(), siL11STimes.end(), muScTimes[iMuSc]) - siL11STimes.begin();
    unsigned int iSiL12S = std::upper_bound(siL12STimes.begin(), siL12STimes.end(), muScTimes[iMuSc]) - siL12STimes.begin();
    unsigned int iSiL13S = std::upper_bound(siL13STimes.begin(), siL13STimes.end(), muScTimes[iMuSc]) - siL13STimes.begin();
    unsigned int iSiL14S = std::upper_bound(siL14STimes.begin(), siL14STimes.end(), muScTimes[iMuSc]) - siL14STimes.begin();
    unsigned int iSiL2S  = std::upper_bound(siL2STimes.begin(),  siL2STimes.end(),  muScTimes[iMuSc]) - siL2STimes.begin();

    // Check for scattering
    if ( (iSiR11S > 0 && muScTimes[iMuSc] - siR11STimes[iSiR11S-1] < fdTScatter) || (iSiR11S < siR11STimes.size() && siR11STimes[iSiR11S] - muScTimes[iMuSc] < fdTScatter) ||  
	 (iSiR12S > 0 && muScTimes[iMuSc] - siR12STimes[iSiR12S-1] < fdTScatter) || (iSiR12S < siR12STimes.size() && siR12STimes[iSiR12S] - muScTimes[iMuSc] < fdTScatter) ||  
	 (iSiR13S > 0 && muScTimes[iMuSc] - siR13STimes[iSiR13S-1] < fdTScatter) || (iSiR13S < siR13STimes.size() && siR13STimes[iSiR13S] - muScTimes[iMuSc] < fdTScatter) ||  
	 (iSiR14S > 0 && muScTimes[iMuSc] - siR14STimes[iSiR14S-1] < fdTScatter) || (iSiR14S < siR14STimes.size() && siR14STimes[iSiR14S] - muScTimes[iMuSc] < fdTScatter) ||  
	 (iSiR2S  > 0 && muScTimes[iMuSc] - siR2STimes[iSiR2S-1]   < fdTScatter) || (iSiR2S  < siR2STimes.size()  && siR2STimes[iSiR2S]   - muScTimes[iMuSc] < fdTScatter) ||
	 (iSiL11S > 0 && muScTimes[iMuSc] - siL11STimes[iSiL11S-1] < fdTScatter) || (iSiL11S < siL11STimes.size() && siL11STimes[iSiL11S] - muScTimes[iMuSc] < fdTScatter) ||  
	 (iSiL12S > 0 && muScTimes[iMuSc] - siL12STimes[iSiL12S-1] < fdTScatter) || (iSiL12S < siL12STimes.size() && siL12STimes[iSiL12S] - muScTimes[iMuSc] < fdTScatter) ||  
	 (iSiL13S > 0 && muScTimes[iMuSc] - siL13STimes[iSiL13S-1] < fdTScatter) || (iSiL13S < siL13STimes.size() && siL13STimes[iSiL13S] - muScTimes[iMuSc] < fdTScatter) ||  
	 (iSiL14S > 0 && muScTimes[iMuSc] - siL14STimes[iSiL14S-1] < fdTScatter) || (iSiL14S < siL14STimes.size() && siL14STimes[iSiL14S] - muScTimes[iMuSc] < fdTScatter) ||  
	 (iSiL2S  > 0 && muScTimes[iMuSc] - siL2STimes[iSiL2S-1]   < fdTScatter) || (iSiL2S  < siL2STimes.size()  && siL2STimes[iSiL2S]   - muScTimes[iMuSc] < fdTScatter) )
      continue;

    // Check for hit in thick silicon and then one-and-only-one in-time hit in thin silicon.
    if ( siR2STimes[iSiR2S] - muScTimes[iMuSc] > fdTSiLow  &&
	 siR2STimes[iSiR2S] - muScTimes[iMuSc] < fdTSiHigh ) {
      iSiR11S = std::upper_bound(siR11STimes.begin(), siR11STimes.end(), siR2STimes[iSiR2S]) - siR11STimes.begin();
      iSiR12S = std::upper_bound(siR12STimes.begin(), siR12STimes.end(), siR2STimes[iSiR2S]) - siR12STimes.begin();
      iSiR13S = std::upper_bound(siR13STimes.begin(), siR13STimes.end(), siR2STimes[iSiR2S]) - siR13STimes.begin();
      iSiR14S = std::upper_bound(siR14STimes.begin(), siR14STimes.end(), siR2STimes[iSiR2S]) - siR14STimes.begin();
      double e[2][4] = { {-1., -1, -1., -1.}, {-1., -1., -1., -1.} };
      // Check for one-and-only-one in-time thin silicon hit
      if ( iSiR11S != 0 && siR2STimes[iSiR2S] - siR11STimes[iSiR11S-1] < fdTPID ) e[0][0] = siR11SEnergies[iSiR11S-1];
      if ( iSiR12S != 0 && siR2STimes[iSiR2S] - siR12STimes[iSiR12S-1] < fdTPID ) e[0][1] = siR12SEnergies[iSiR12S-1];
      if ( iSiR13S != 0 && siR2STimes[iSiR2S] - siR13STimes[iSiR13S-1] < fdTPID ) e[0][2] = siR13SEnergies[iSiR13S-1];
      if ( iSiR14S != 0 && siR2STimes[iSiR2S] - siR14STimes[iSiR14S-1] < fdTPID ) e[0][3] = siR14SEnergies[iSiR14S-1];
      if ( iSiR11S != siR11STimes.size() && siR11STimes[iSiR11S] - siR2STimes[iSiR2S] < fdTPID ) e[1][0] = siR11SEnergies[iSiR11S];
      if ( iSiR12S != siR12STimes.size() && siR12STimes[iSiR12S] - siR2STimes[iSiR2S] < fdTPID ) e[1][1] = siR12SEnergies[iSiR12S];
      if ( iSiR13S != siR13STimes.size() && siR13STimes[iSiR13S] - siR2STimes[iSiR2S] < fdTPID ) e[1][2] = siR13SEnergies[iSiR13S];
      if ( iSiR14S != siR14STimes.size() && siR14STimes[iSiR14S] - siR2STimes[iSiR2S] < fdTPID ) e[1][3] = siR14SEnergies[iSiR14S];
      unsigned int count = 0;
      double de = -1.;
      for (unsigned int i = 0; i < 2; ++i) {
	for (unsigned int j = 0; j < 4; ++j) {
	  if (e[i][j] > 0.) {
	    de = e[i][j];
	    ++count;
	  }
	}
      }
      if (count == 1) {
	fHist_PIDRight->Fill(de+siR2SEnergies[iSiR2S], de);
      }
    }
    if ( siL2STimes[iSiL2S] - muScTimes[iMuSc] > fdTSiLow  &&
	 siL2STimes[iSiL2S] - muScTimes[iMuSc] < fdTSiHigh ) {
      iSiL11S = std::upper_bound(siL11STimes.begin(), siL11STimes.end(), siL2STimes[iSiL2S]) - siL11STimes.begin();
      iSiL12S = std::upper_bound(siL12STimes.begin(), siL12STimes.end(), siL2STimes[iSiL2S]) - siL12STimes.begin();
      iSiL13S = std::upper_bound(siL13STimes.begin(), siL13STimes.end(), siL2STimes[iSiL2S]) - siL13STimes.begin();
      iSiL14S = std::upper_bound(siL14STimes.begin(), siL14STimes.end(), siL2STimes[iSiL2S]) - siL14STimes.begin();
      double e[2][4] = { {-1., -1, -1., -1.}, {-1., -1., -1., -1.} };
      // Check for one-and-only-one in-time thin silicon hit
      if ( iSiL11S != 0 && siL2STimes[iSiL2S] - siL11STimes[iSiL11S-1] < fdTPID ) e[0][0] = siL11SEnergies[iSiL11S-1];
      if ( iSiL12S != 0 && siL2STimes[iSiL2S] - siL11STimes[iSiL12S-1] < fdTPID ) e[0][1] = siL12SEnergies[iSiL12S-1];
      if ( iSiL13S != 0 && siL2STimes[iSiL2S] - siL11STimes[iSiL13S-1] < fdTPID ) e[0][2] = siL13SEnergies[iSiL13S-1];
      if ( iSiL14S != 0 && siL2STimes[iSiL2S] - siL11STimes[iSiL14S-1] < fdTPID ) e[0][3] = siL14SEnergies[iSiL14S-1];
      if ( iSiL11S != siL11STimes.size() && siL11STimes[iSiL11S] - siL2STimes[iSiL2S] < fdTPID ) e[1][0] = siL11SEnergies[iSiL11S];
      if ( iSiL12S != siL12STimes.size() && siL12STimes[iSiL12S] - siL2STimes[iSiL2S] < fdTPID ) e[1][1] = siL12SEnergies[iSiL12S];
      if ( iSiL13S != siL13STimes.size() && siL13STimes[iSiL13S] - siL2STimes[iSiL2S] < fdTPID ) e[1][2] = siL13SEnergies[iSiL13S];
      if ( iSiL14S != siL14STimes.size() && siL14STimes[iSiL14S] - siL2STimes[iSiL2S] < fdTPID ) e[1][3] = siL14SEnergies[iSiL14S];
      unsigned int count = 0;
      double de = -1.;
      for (unsigned int i = 0; i < 2; ++i) {
	for (unsigned int j = 0; j < 4; ++j) {
	  if (e[i][j] > 0.) {
	    de = e[i][j];
	    ++count;
	  }
	}
      }
      if (count == 1) {
	fHist_PIDLeft->Fill(de+siL2SEnergies[iSiL2S], de);
      }
    }
  }
  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int Si16P::AfterLastEntry(TGlobalData* gData, const TSetupData *setup){
  return 0;
}


std::vector<double> Si16P::CalculateTimes(const IDs::channel& ch, const std::vector<TPulseIsland*>& tpis) {
  std::vector<double> t;
  if      (ch == Si16P::fMuSc)   for (unsigned int i = 0; i < tpis.size(); ++i) t.push_back(fCFTime_MuSc  (tpis[i]));
  else if (ch == Si16P::fSiR11S) for (unsigned int i = 0; i < tpis.size(); ++i) t.push_back(fCFTime_SiR11S(tpis[i]));
  else if (ch == Si16P::fSiR12S) for (unsigned int i = 0; i < tpis.size(); ++i) t.push_back(fCFTime_SiR12S(tpis[i]));
  else if (ch == Si16P::fSiR13S) for (unsigned int i = 0; i < tpis.size(); ++i) t.push_back(fCFTime_SiR13S(tpis[i]));
  else if (ch == Si16P::fSiR14S) for (unsigned int i = 0; i < tpis.size(); ++i) t.push_back(fCFTime_SiR14S(tpis[i]));
  else if (ch == Si16P::fSiR2S)  for (unsigned int i = 0; i < tpis.size(); ++i) t.push_back(fCFTime_SiR2S (tpis[i]));
  else if (ch == Si16P::fSiL11S) for (unsigned int i = 0; i < tpis.size(); ++i) t.push_back(fCFTime_SiL11S(tpis[i]));
  else if (ch == Si16P::fSiL12S) for (unsigned int i = 0; i < tpis.size(); ++i) t.push_back(fCFTime_SiL12S(tpis[i]));
  else if (ch == Si16P::fSiL13S) for (unsigned int i = 0; i < tpis.size(); ++i) t.push_back(fCFTime_SiL13S(tpis[i]));
  else if (ch == Si16P::fSiL14S) for (unsigned int i = 0; i < tpis.size(); ++i) t.push_back(fCFTime_SiL14S(tpis[i]));
  else if (ch == Si16P::fSiL2S)  for (unsigned int i = 0; i < tpis.size(); ++i) t.push_back(fCFTime_SiL2S (tpis[i]));
  else throw std::logic_error("Si16P: Invalid channel to calculate times for.");
  return t;
}

std::vector<double> Si16P::CalculateEnergies(const IDs::channel& ch, const std::vector<TPulseIsland*>& tpis) {
  std::vector<double> e;
  if      (ch == Si16P::fMuSc)   for (unsigned int i = 0; i < tpis.size(); ++i) e.push_back(                    fMBAmp_MuSc  (tpis[i]));
  else if (ch == Si16P::fSiR11S) for (unsigned int i = 0; i < tpis.size(); ++i) e.push_back(fADC2E_SiR11S->Eval(fMBAmp_SiR11S(tpis[i])));
  else if (ch == Si16P::fSiR12S) for (unsigned int i = 0; i < tpis.size(); ++i) e.push_back(fADC2E_SiR12S->Eval(fMBAmp_SiR12S(tpis[i])));
  else if (ch == Si16P::fSiR13S) for (unsigned int i = 0; i < tpis.size(); ++i) e.push_back(fADC2E_SiR13S->Eval(fMBAmp_SiR13S(tpis[i])));
  else if (ch == Si16P::fSiR14S) for (unsigned int i = 0; i < tpis.size(); ++i) e.push_back(fADC2E_SiR14S->Eval(fMBAmp_SiR14S(tpis[i])));
  else if (ch == Si16P::fSiR2S)  for (unsigned int i = 0; i < tpis.size(); ++i) e.push_back(fADC2E_SiR2S ->Eval(fMBAmp_SiR2S (tpis[i])));
  else if (ch == Si16P::fSiL11S) for (unsigned int i = 0; i < tpis.size(); ++i) e.push_back(fADC2E_SiL11S->Eval(fMBAmp_SiL11S(tpis[i])));
  else if (ch == Si16P::fSiL12S) for (unsigned int i = 0; i < tpis.size(); ++i) e.push_back(fADC2E_SiL12S->Eval(fMBAmp_SiL12S(tpis[i])));
  else if (ch == Si16P::fSiL13S) for (unsigned int i = 0; i < tpis.size(); ++i) e.push_back(fADC2E_SiL13S->Eval(fMBAmp_SiL13S(tpis[i])));
  else if (ch == Si16P::fSiL14S) for (unsigned int i = 0; i < tpis.size(); ++i) e.push_back(fADC2E_SiL14S->Eval(fMBAmp_SiL14S(tpis[i])));
  else if (ch == Si16P::fSiL2S)  for (unsigned int i = 0; i < tpis.size(); ++i) e.push_back(fADC2E_SiL2S ->Eval(fMBAmp_SiL2S (tpis[i])));
  else throw std::logic_error("Si16P: Invalid channel to calculate energies for.");
  return e;
}

void Si16P::ThrowIfInputsInsane(modules::options* opts) {
}

ALCAP_REGISTER_MODULE(Si16P,tcut_musc,tcut_si_low,tcut_si_high,tcut_pid,tcut_scatter,tcut_detpp,adccut_musc);
