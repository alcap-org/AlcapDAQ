
/***********************************************/
//This program is NOT complete, do not use
/***********************************************/

//While this is good, I should also consider using or making my own program using the other
//method as well.

//This is a test of the method of neutron discrimination

/********************************************************************\

Name:         MScintInt
Created by:   Damien Alexander

Contents:     A module to integrate the pulse for the scintilators.

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <sstream>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>

/* AlCap includes */
#include "TOctalFADCIsland.h"
#include "TOctalFADCBankReader.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using std::string;
using std::map;
using std::vector;
using std::pair;


/*-- Module declaration --------------------------------------------*/
INT  MScintInt_init(void);
INT  MScintInt(EVENT_HEADER*, void*);

vector<string> GetAllFADCBankNames();        
double GetClockTickForChannel(string bank_name);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static vector<TOctalFADCBankReader*> fadc_bank_readers;
map <std::string, TH1I*> scintillator_integral_map;

std::string Scint[4] ={"ScL", "ScR", "ScVe", "ScGe"};
std::string ScintBank[4] = {"Na83", "Nb83", "Nc83", "Nd83"};

ANA_MODULE MScintInt_module =
{
	"MScintEnergy",                    /* module name           */
	"Damien Alexander",              /* author                */
	MScintInt,                      /* event routine         */
	NULL,                            /* BOR routine           */
	NULL,                            /* EOR routine           */
	MScintInt_init,                 /* init routine          */
	NULL,                            /* exit routine          */
	NULL,                            /* parameter structure   */
	0,                               /* structure size        */
	NULL,                            /* initial parameters    */
};

INT MScintInt_init()
{
    // Loop over the map and select the scintillators
    std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;

    for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin(); mapIter != bank_to_detector_map.end(); mapIter++)
    {
        std::string bankname = mapIter->first;
        std::string detname = gSetup->GetDetectorName(bankname);
   
        if((bankname != "Na83" && bankname != "Nb83") && (bankname != "Nc83" && bankname != "Nd83"))
             continue;
        // Generate a histogram for each.  For some reason it is generating a histogram for all det.

        //std::string detname = gSetup->GetDetectorName(mapIter->first);  //this may have changed
        std::string histname = "h" + detname + "_Integral";
        std::string histtitle = "Plot of the integral of the " + detname + " channel";
        TH1I* hScintInt = new TH1I(histname.c_str(),histtitle.c_str(),4095,0,30000);
        hScintInt->GetXaxis()->SetTitle("Integral of the Pulses(ADC counts)");
        hScintInt->GetXaxis()->CenterTitle(1);
        hScintInt->GetYaxis()->SetTitle("Number of pulses");
        hScintInt->GetYaxis()->CenterTitle(1);
        hScintInt->SetBit(TH1::kCanRebin);

        scintillator_integral_map[bankname] = hScintInt;
        
    }
    return SUCCESS;
}

INT MScintInt(EVENT_HEADER *pheader, void *pevent)
{

    // some typedefs
    typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
    typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
    typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;


    //Fetch a refernce to the gData structure that stores a map
    //of (bank_name, vector<TPulseIsland*>) pairs
    TStringPulseIslandMap& pulse_islands_map = 
          gData->fPulseIslandToChannelMap;

    // Loop over the map and get each bank name
    for (map_iterator theMapIter = pulse_islands_map.begin(); theMapIter !=
                pulse_islands_map.end(); theMapIter++) 
    {
         std::string bankname = theMapIter->first;
	 std::vector<TPulseIsland*> thePulses = theMapIter->second;
 
         //  If the bank is a scintillator, we continue
         if((bankname != "Na83" && bankname != "Nb83") && (bankname != "Nc83" && bankname != "Nd83"))
             continue;


          int V=20
         
          //Loop over the Pulses
          for (std::vector<TPulseIsland*>::iterator thePulseIter = thePulses.begin(); 
                    thePulseIter != thePulses.end(); thePulseIter++)
          {
               // make sure our signal is long enough
               if(thePulses.size() < 37)
                   continue; 


               int integral = 0;
               std::vector<int> samples = (*thePulseIter)->GetSamples();


               //get the pedestal
               int pedestal = 0, sum = 0;
               for (int k = 0; k < 10; k++)
                   sum += samples[k];
               pedestal = sum / 10;
                  
         
               //now we determine the integral 
               //(pulses are positive, so no need to correct)
               for (int i=10; i < 60; i++)
               {
                     if(samples[i] > pedestal +20)
                     {
                            
                        for (int j = i; j < i + 6 ; j++)
                        {
                            int sample_value=samples[j];
                            integral = integral + sample_value;
                        }
                        break;
                            
                     }
               }

               int Energy = integral - (pedestal * 5);
                                   
                
               scintillator_integral_map[bankname]->Fill(V);
               
                  
          }      //close the vector loop
       
    }     //close the map loop

    return SUCCESS;
 
}


