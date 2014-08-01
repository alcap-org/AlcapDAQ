#include "IslandAmplitude.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "TMath.h"

#include <iostream>
#include <utility>
#include <algorithm>
using std::cout;
using std::endl;

//extern SourceAnalPulseMap gAnalysedPulseMap;


////////////////////////////////////////////////////////////

IslandAmplitude::IslandAmplitude(modules::options* opts)
  : BaseModule("IslandAmplitude",opts)
  , fDirName("IslandAmplitude")

{
  // Do something with opts here.
}

/////////////////////////////////////////////////////////////////

IslandAmplitude::~IslandAmplitude() { }

////////////////////////////////////////////////////////////////

// Called before the main event loop
// Return non-zero to indicate a problem

int IslandAmplitude::BeforeFirstEntry(TGlobalData* data,TSetupData *setup)
  {
  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----IslandAmplitude::BeforeFirstEntry(): I'm debugging!"<<endl;
     
  }

  fNProcessed = 0;

  if(!TDirectory::CurrentDirectory()->cd(fDirName.c_str()))
     TDirectory::CurrentDirectory()->mkdir(fDirName.c_str())->cd();

  fHistDir = TDirectory::CurrentDirectory();

  StringPulseIslandMap& islands = data->fPulseIslandToChannelMap;



  for(StringPulseIslandMap::iterator mIt = islands.begin(); 
      mIt != islands.end(); ++mIt)
    {
      std::string bankname = mIt->first;
      std::string detname = setup->GetDetectorName(bankname);
      int max_adc_value = pow(2, setup->GetNBits(bankname));     
      std::string histname = "hDQ_IslandAmplitudes_" + detname + "" + bankname;
      std::string histtitle = "Amplitude of Pulses in " + detname;
      TH1F* hDQ = new TH1F(histname.c_str(), histtitle.c_str(), 
                         max_adc_value, 0, max_adc_value);
      hDQ->GetXaxis()->SetTitle("Amplitude (adc)");
      hDQ->GetYaxis()->SetTitle("Counts");
      fAmpHist[bankname] = hDQ;

      if(detname == "muSc")
        fAmpNorm=hDQ;

      histname += "_EvtNorm";
      histtitle +=" (event normalized)";
      hDQ = new TH1F(histname.c_str(), histtitle.c_str(), 
		     500, 0, 2000);
      fAmpHistNorm[bankname] = hDQ;
    }
  TDirectory::CurrentDirectory()->cd("..");


  return 0;
}

//////////////////////////////////////////////////////////////

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int IslandAmplitude::ProcessEntry(TGlobalData* data,const TSetupData *setup)
{
  ++fNProcessed;

  StringPulseIslandMap& banks = data->fPulseIslandToChannelMap;
  int entry_norm =-1;

  for(StringPulseIslandMap::iterator mIter = banks.begin(); mIter != banks.end(); ++mIter)
    {
      std::string detname = setup->GetDetectorName(mIter->first);
      if(detname == "muSc")
	entry_norm = mIter->second.size();
    }

  for(StringPulseIslandMap::iterator mIter = banks.begin(); mIter != banks.end(); ++mIter)
    {
      std::string bankname = mIter->first;
      std::string detname = setup->GetDetectorName(bankname);
      PulseIslandList& pulses = mIter->second;

      TH1F* hist = fAmpHist.find(bankname)->second;
      TH1F* histnorm = fAmpHistNorm.find(bankname)->second;

      for(PIL_iter pIt = pulses.begin(); pIt != pulses.end(); ++pIt)
	{


	  //
	  double amp = 0;
	  std::vector<int> samples = (*pIt)->GetSamples();
	  std::vector<int>::iterator pos;
	  int polarity = setup->GetTriggerPolarity(bankname);
	  int pedestal = setup->GetPedestal(bankname);

	  if(polarity == 1)
	    pos = std::max_element(samples.begin(), samples.end());
	  else if(polarity == -1)
	    pos = std::min_element(samples.begin(), samples.end());
	  
	  amp = polarity * (*pos - pedestal);
	  //

	  hist->Fill(amp);
	  if(entry_norm != 0)
	    histnorm->Fill(amp, 1.0/entry_norm);
	}
    }


  return 0;
}


//////////////////////////////////////////////////////////////////////

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int IslandAmplitude::AfterLastEntry(TGlobalData* gData,TSetupData *setup){

  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----IslandAmplitude::AfterLastEntry(): I'm debugging!"<<endl;
  }

  TDirectory*pwd = TDirectory::CurrentDirectory();
  fHistDir->cd();

  double run_norm = fAmpNorm->Integral(0,-1);
  for(mapSH_t::iterator it = fAmpHist.begin(); it != fAmpHist.end(); ++it)
    {
      TH1F* h = it->second;
      TObject* obj = h->Clone((std::string(h->GetName()) + "_RunNorm").c_str());
      TH1* hn = static_cast<TH1*>(obj);
      hn->SetTitle((std::string(h->GetTitle()) + " (run normalized)").c_str());
      hn->Scale(1.0/run_norm);
    }

  for(mapSH_t::iterator it = fAmpHistNorm.begin(); it != fAmpHistNorm.end(); ++it)
    it->second->Scale(1.0/fNProcessed);

  pwd->cd();

  return 0;
}

//////////////////////////////////////////////////////////////////////

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(IslandAmplitude);
