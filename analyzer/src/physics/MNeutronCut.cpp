#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <sstream>
#include <algorithm>
#include <cmath>

#include "midas.h"

#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using std::string;
using std::map;
using std::vector;

//INT MNeutronCut_init(void);
INT MNeutronCut(EVENT_HEADER*, void*);
//Int MNeutronCut_eor(INT);

extern TGlobalData* gData;
extern TSetupData* gSetup;

ANA_MODULE MNeutronCut_module = 
  {
    "MNeutronCut",              /*module name         */
    "Damien Alexander",         /*author name         */
    MNeutronCut,                /*event routine       */
    NULL,                       /*BOR routine         */
    NULL,                       /*EOR routine         */
    NULL,                       /*init routine        */
    NULL,                       /*exit routine        */
    NULL,                       /*parameter structure */
    0,                          /*structure size      */
    NULL,                       /*initial parameters  */
  };

INT MNeutronCut(EVENT_HEADER *pheader, void *pevent)
{
  
  std::map<std::string, std::vector<TPulseIsland*> >& TPI_map = 
    gData->fPulseIslandToChannelMap;

  for(std::map<std::string, std::vector<TPulseIsland*> >::iterator mIter = 
	TPI_map.begin();  mIter != TPI_map.end(); ++mIter)
    {
      std::string bankname = mIter->first;
      std::string detname = gSetup->GetDetectorName(bankname);
      std::vector<TPulseIsland*>& thePulses = mIter->second;

      if(!gSetup->IsNeutron(detname))
	continue;

      int trigger_polarity = gSetup->GetTriggerPolarity(bankname);

      //std::cout << thePulses.size() << " total hits in the neutron detector" << std::endl;
      
      for(std::vector<TPulseIsland*>::iterator pIter = thePulses.begin(); pIter != thePulses.end(); ++pIter)
	{
	  //get pulse samples
	  const std::vector<int>& samples = (*pIter)->GetSamples();
	  
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
	  //double energyMevnr=-1.;
	  if(detname == "NdetD")  energyMevee = (pulse_height * 0.0003999) + 0.008234;
	  if(detname == "NdetU")  energyMevee = (pulse_height * 0.0004015) + 0.009037;

	  /*
	  energyMevee=0.83*energyMevnr - (2.82*(1.0 - exp(-0.25 * pow(energyMevnr,0.93)))); //from bicron data sheet (via Cecil 1978 paper)
          need to invert this or parametrize this to get energy in Mevnr
	  */

	  bool neutron = FALSE;
	  if(energyMevee > 2.0){
	    if(detname == "NdetU")
	      if(ratio >  -0.005*energyMevee +0.17)
		neutron = TRUE;
	    if(detname == "NdetD")
	      if(ratio > -0.00581*energyMevee +0.14663)
		neutron = TRUE;
	  }
	  else if(energyMevee <= 2.0) {
	    if(detname == "NdetU")
	      if(ratio > -0.025*energyMevee +0.21)
		neutron = TRUE;
	    if(detname == "NdetD")
	      if(ratio >  -0.01667*energyMevee +0.16833)
		neutron = TRUE;
	  }


	  // Damien's initial PSD based on AmBe data
	  if(neutron) {//do nothing
	  }
	  else{  //gamma, cut this
	    if(std::distance(thePulses.begin(), pIter) > 1){
	      thePulses.erase(pIter);
	      pIter--;
	    }
	  }
	} // pIter

      //std::cout << thePulses.size() << " neutron pulses" << std::endl;
      
	
    } //mIter
  
  return SUCCESS;
}
