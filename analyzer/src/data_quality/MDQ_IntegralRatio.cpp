//////////////////////////////////////////////////////////////////////////////
/// \defgroup MDQ_IntegralRatio
/// \ingroup lldq
/// \author Daimien Alexander
///
/// \brief
/// Generates plots for comparison of integrals for the purposes of pulse
/// shape discrimination 
///
/// \details
/// Creates a number of histograms.
/// @{

/// \var DQ_Integral_XY
/// \brief
/// Map of bank names to histograms plotting long gate integrals
/// vs short gate integrals.


/// \var DQ_Integral_Ratio_PH
/// \brief
/// Map of bank names to histograms plotting the ratio of short 
/// integral over long gate integral vs pulse height

/// \var DQ_Neutron
/// \brief
/// Map of bank names to histograms with a pulse height distribution 
/// of signals with an integral ratio similar to a neutron

/// \var DQ_Gamma
/// \brief
/// Map of Bank names to histograms of pulse height distribution
/// of signals with an integral ratio similar to that of a gamma
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <sstream>
#include <algorithm>
#include <cmath>

#include "midas.h"

#include "TH1.h"
#include "TH2.h"
#include "TDirectory.h"
#include "TFile.h"

#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using std::string;
using std::map;
using std::vector;

INT MDQ_IntegralRatio_init(void);
INT MDQ_IntegralRatio(EVENT_HEADER*, void*);
//Int MDQ_IntegralRatio_eor(INT);

extern TGlobalData* gData;
extern TSetupData* gSetup;

map <std::string, TH2D*> DQ_Integral_XY;
map <std::string, TH2F*> DQ_IntegralRatio_PH;
map <std::string, TH2F*> DQ_IntegralRatio_E;
map <std::string, TH1F*> DQ_NeutronEnMeVee;
map <std::string, TH1F*> DQ_NeutronEnMeVnr;
map <std::string, TH1F*> DQ_GammaEnMeVee;
map <std::string, TH1F*> DQ_GammaPH;
map <std::string, TH1F*> DQ_NeutronPH;

ANA_MODULE MDQ_IntegralRatio_module = 
  {
    "MDQ_IntegralRatio",        /*module name         */
    "Damien Alexander",         /*author name         */
    MDQ_IntegralRatio,          /*event routine       */
    NULL,                       /*BOR routine         */
    NULL,                       /*EOR routine         */
    MDQ_IntegralRatio_init,     /*init routine        */
    NULL,                       /*exit routine        */
    NULL,                       /*parameter structure */
    0,                          /*structure size      */
    NULL,                       /*initial parameters  */
  };

INT MDQ_IntegralRatio_init()
{
  //check for directory, create if missing
  //place in DataQuality for now


  //initialize histograms
  std::map<std::string, std::string> bankDetMap = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bankDetMap.begin(); mapIter != bankDetMap.end(); mapIter++)
    {
      std::string bankname = mapIter->first;
      std::string detname = gSetup->GetDetectorName(bankname);

      
      if(!gSetup->IsNeutron(detname)) continue;
      

      std::string histname = "h" + bankname + "_DQ_Integral";
      std::string histtitle = "Long Integral vs Short Integral for " + bankname;
      TH2D* hDQ_integralhist = new TH2D(histname.c_str(), histtitle.c_str(), 500, 2000, 100000, 500, 50, 8000);
      hDQ_integralhist->GetXaxis()->SetTitle("Long Integral (ADC counts)");
      hDQ_integralhist->GetYaxis()->SetTitle("Short Integral (ADC counts)");
      DQ_Integral_XY[bankname] = hDQ_integralhist;

      int max_adc_value = std::pow(2, gSetup->GetNBits(bankname));

      histname = "h" + bankname + "_DQ_IntegralRatio_PH";
      histtitle = "Integral Ratio vs pulse height for " + bankname;
      TH2F* hDQ_integralPH = new TH2F(histname.c_str(), histtitle.c_str(), max_adc_value/8, 0, max_adc_value, 500, -0, 0.4);
      hDQ_integralPH->GetYaxis()->SetTitle("Integral Ratio");
      hDQ_integralPH->GetXaxis()->SetTitle("pulse height (ADC counts)");
      DQ_IntegralRatio_PH[bankname] = hDQ_integralPH;

      histname = "h" + bankname + "_DQ_IntegralRatio_E";
      histtitle = "Integral Ratio vs energy for " + bankname;
      TH2F* hDQ_integralE = new TH2F(histname.c_str(), histtitle.c_str(), max_adc_value/8, 0, 6.5, 500, -0, 0.4);
      hDQ_integralE->GetYaxis()->SetTitle("Integral Ratio");
      hDQ_integralE->GetXaxis()->SetTitle("Energy (MeVee)");
      DQ_IntegralRatio_E[bankname] = hDQ_integralE;

      /*  int max_energy;
	  if(detname == "NdetD")
	  max_energy = (max_adc_value+1) * 0.0003999;
	  if(detname == "NdetU")
	  max_energy = (max_adc_value+1) * 0.0004015;

      */

      histname = "h" + bankname + "_DQ_NeutronPH";
      histtitle = "Neutron Pulse Heights for " + bankname;
      TH1F* hDQ_neutronPH = new TH1F(histname.c_str(), histtitle.c_str(), max_adc_value, 0, max_adc_value);
      hDQ_neutronPH->GetXaxis()->SetTitle("Pulse Height (ADC counts)");
      hDQ_neutronPH->GetYaxis()->SetTitle("count");
      DQ_NeutronPH[bankname] = hDQ_neutronPH;

      histname = "h" + bankname + "_DQ_NeutronEnMeVee";
      histtitle = "Neutron Energy in MeVee for " + bankname;
      TH1F* hDQ_neutronEnMeVee = new TH1F(histname.c_str(), histtitle.c_str(), 200, 0, 12);
      hDQ_neutronEnMeVee->GetXaxis()->SetTitle("Neutron Energy  (MeVee)");
      hDQ_neutronEnMeVee->GetYaxis()->SetTitle("count");
      DQ_NeutronEnMeVee[bankname] = hDQ_neutronEnMeVee;

      histname = "h" + bankname + "_DQ_NeutronEnMeVnr";
      histtitle = "Neutron Energy in MeVnr for " + bankname;
      TH1F* hDQ_neutronEnMeVnr = new TH1F(histname.c_str(), histtitle.c_str(), 200, 0, 12);
      hDQ_neutronEnMeVnr->GetXaxis()->SetTitle("Neutron Energy  (MeVnr)");
      hDQ_neutronEnMeVnr->GetYaxis()->SetTitle("count");
      DQ_NeutronEnMeVnr[bankname] = hDQ_neutronEnMeVnr;

      histname = "h" + bankname + "_DQ_GammaPH";
      histtitle = "Gamma Pulse Heights for " + bankname;
      TH1F* hDQ_gammaPH = new TH1F(histname.c_str(), histtitle.c_str(), max_adc_value, 0, max_adc_value);
      hDQ_gammaPH->GetXaxis()->SetTitle("Pulse Height (ADC counts)");
      hDQ_gammaPH->GetYaxis()->SetTitle("count");
      DQ_GammaPH[bankname] = hDQ_gammaPH;

      histname = "h" + bankname + "_DQ_GammaEnMeVee";
      histtitle = "Gamma Energy for " + bankname;
      TH1F* hDQ_gammaEnMeVee = new TH1F(histname.c_str(), histtitle.c_str(), 200, 0, 12);
      hDQ_gammaEnMeVee->GetXaxis()->SetTitle("Gamma Energy (MeV)");
      hDQ_gammaEnMeVee->GetYaxis()->SetTitle("count");
      DQ_GammaEnMeVee[bankname] = hDQ_gammaEnMeVee;


      //Return to root directory
      //gDirectory->Cd("/MidasHists/");

    }

  return SUCCESS;
}


INT MDQ_IntegralRatio(EVENT_HEADER *pheader, void *pevent)
{
  
  std::map<std::string, std::vector<TPulseIsland*> > TPI_map = 
    gData->fPulseIslandToChannelMap;

  for(std::map<std::string, std::vector<TPulseIsland*> >::iterator mIter = 
	TPI_map.begin();  mIter != TPI_map.end(); ++mIter)
    {
      std::string bankname = mIter->first;
      std::string detname = gSetup->GetDetectorName(bankname);
      std::vector<TPulseIsland*> thePulses = mIter->second;
      
      if(!gSetup->IsNeutron(detname))
	continue;
      
      for(std::vector<TPulseIsland*>::iterator pIter = thePulses.begin(); pIter != thePulses.end(); ++pIter)
	{
	  //get pulse samples
	  const std::vector<int>& samples = (*pIter)->GetSamples();
	  
	  int trigger_polarity = gSetup->GetTriggerPolarity(bankname);
	  std::vector<int>::const_iterator pulse_time;
	  trigger_polarity == 1 ? pulse_time = std::max_element(samples.begin(), samples.end()) : pulse_time = std::min_element(samples.begin(), samples.end());

	  /*
	    int max_sample = (*pIter)->GetPeakSample();;
	    int pulse_time = max_sample;
	  */
	  int max_sample = std::distance(samples.begin(), pulse_time);
	  int pedestal = (*pIter)->GetPedestal(16);

	  if(samples.at(max_sample) == 0)  continue;

	  int pulse_height = trigger_polarity * (samples.at(max_sample) - pedestal);

	  //std::cout << pulse_height << std::endl;

	  //Get clock tick and determine cut times
	  //double clock_tick = (*pIter)->GetClockTickInNs();
	  int tLstart = max_sample - 5;
	  int tSstart = max_sample + 9;
	  int tstop = max_sample + 80;

	  //integrate and get ratio
	  double lInt = 0, sInt=0;
	  for(std::vector<int>::const_iterator sIter = samples.begin(); sIter != samples.end(); ++sIter)
	    {
	      if((std::distance(samples.begin(), sIter) > tLstart) && (std::distance(samples.begin(), sIter) < tstop))
		{
		  lInt += trigger_polarity*((*sIter)- pedestal);
		  if(std::distance(samples.begin(), sIter) > tSstart)
		    sInt += trigger_polarity*((*sIter)- pedestal);
		}
	   
	    }

	  double ratio = sInt/lInt;
	  //std::cout << "long integral " << lInt << "    short integral " << sInt << "    ratio " << ratio << std::endl;

	  //determine amplitude from pulse heights.  to be added later.

	  double energyMevee =1.;
	  double energyMevnr=-1.;
	  if(detname == "NdetD")  energyMevee = (pulse_height * 0.0003999) + 0.008234;
	  if(detname == "NdetU")  energyMevee = (pulse_height * 0.0004015) + 0.009037;

	  /*
	  energyMevee=0.83*energyMevnr - (2.82*(1.0 - exp(-0.25 * pow(energyMevnr,0.93)))); //from bicron data sheet (via Cecil 1978 paper)
          need to invert this or parametrize this to get energy in Mevnr
	  */
	  //fill hists
	  DQ_Integral_XY[bankname]->Fill(lInt, sInt);
	  DQ_IntegralRatio_PH[bankname]->Fill(pulse_height, ratio);
	  DQ_IntegralRatio_E[bankname]->Fill(energyMeVee, ratio);

	  // Damien's initial PSD based on AmBe data
	  if(((detname == "NdetD") && (ratio < 0.11)) || ((detname == "NdetU") && (ratio < 0.12))) {
	    DQ_GammaPH[bankname]->Fill(pulse_height);
	    DQ_GammaEnMeVee[bankname]->Fill(energyMevee);
	  }
	  else if(((detname == "NdetD")&&( ratio > 0.12))||((detname == "NdetU") && (ratio > 0.15))) {
	    DQ_NeutronPH[bankname]->Fill(pulse_height);
	    DQ_NeutronEnMeVee[bankname]->Fill(energyMevee);
	    DQ_NeutronEnMeVnr[bankname]->Fill(energyMevnr);
	  }
	} // pIter
    } //mIter
  
  return SUCCESS;
}
