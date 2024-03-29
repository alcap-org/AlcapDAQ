////////////////////////////////////////////////////////////////////////////////
/// \defgroup MDQ_Thresholds
/// \ingroup lldq
/// \author Andrew Edmonds
///
/// \brief
/// Record thresholds from the ODB.
///
/// \details
/// Record thresholds from ODB as entries in a histogram per bank.
/// @{

/// \var DQ_Thresholds_histograms_map
/// \brief
/// Map of bank name to histogram. FADC histograms have 2 entries,
/// one each for an upper and lower trigger threshold. CAENs have single entry.
////////////////////////////////////////////////////////////////////////////////
/********************************************************************\

Name:         MDQ_Thresholds
Created by:   Andrew Edmonds

Contents:     hDQ_Thresholds
              - plots the thresholds in each channel

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <sstream>
#include <cmath>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>
#include <TDirectory.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MDQ_Thresholds_init(void);
INT  MDQ_Thresholds(EVENT_HEADER*, void*);
INT  MDQ_Thresholds_eor(INT);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

map <std::string, TH1F*> DQ_Thresholds_histograms_map;

ANA_MODULE MDQ_Thresholds_module =
{
	"MDQ_Thresholds",              /* module name           */
	"Andrew Edmonds",              /* author                */
	MDQ_Thresholds,                /* event routine         */
	NULL,                          /* BOR routine           */
	MDQ_Thresholds_eor,            /* EOR routine           */
	MDQ_Thresholds_init,           /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};



/** This method initializes histograms.
*/
INT MDQ_Thresholds_init()
{
    // See if the DataQuality_LowLevel/ directory already exists
  if (!gDirectory->Cd("DQ_Thresholds")) {
    
    std::string dir_name("DQ_Thresholds/");
    gDirectory->mkdir(dir_name.c_str());
    gDirectory->Cd(dir_name.c_str());
  }

  // Create a histogram for each channel
  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin(); 
      mapIter != bank_to_detector_map.end(); mapIter++) { 

    std::string bankname = mapIter->first;
    if(TSetupData::IsTDC(bankname)) continue;
    std::string detname = gSetup->GetDetectorName(bankname);

    // hDQ_Thresholds_[DetName]_[BankName]
    std::string histname = "hDQ_Thresholds_" + detname + "_" + bankname;
    std::string histtitle = "ODB Thresholds in " + bankname;
    TH1F* hDQ_Histogram = new TH1F(histname.c_str(), histtitle.c_str(), 
				   2,0,2); // will want an upper and lower threshold for FADC channels
    hDQ_Histogram->GetXaxis()->SetTitle("");
    hDQ_Histogram->GetYaxis()->SetTitle("Threshold");
    DQ_Thresholds_histograms_map[bankname] = hDQ_Histogram;
  }

  gDirectory->Cd("/MidasHists/");
  return SUCCESS;
}

/** This method does any last minute things to the histograms at the end of the run
 */
INT MDQ_Thresholds_eor(INT run_number) {

  // Get the run duration to scale the histogram
  HNDLE hDB, hKey;
  char keyName[200];

  if(cm_get_experiment_database(&hDB, NULL) != CM_SUCCESS){
    printf("Warning: Could not connect to ODB database!\n");
    return false;
  }

  // Loop through the channels and get the thresholds
  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin(); 
      mapIter != bank_to_detector_map.end(); mapIter++) { 

    std::string bankname = mapIter->first;
    if(TSetupData::IsTDC(bankname)) continue;

    //I can keep old methods for R13.
    //For R15a, override values and use IsWFD(bankname)
    if (TSetupData::Instance()->IsFADC(bankname)) {
      // get the FADC thresholds (both upper and lower)

      // first get the channel and address from the bankname
      int iChn = (int)(bankname[1] - 97);
      std::string iAddr = bankname.substr(2, 2);

      // Get the lower threshold
      sprintf(keyName, "/Equipment/Crate 9/Settings/NFADC %s/Channel %d/Lower threshold", iAddr.c_str(), iChn);
      if(db_find_key(hDB,0,keyName, &hKey) != SUCCESS){
	printf("Warning: Could not find key %s\n", keyName);
	return false;
      }
      KEY lower_threshold_key;
      if(db_get_key(hDB, hKey, &lower_threshold_key) != DB_SUCCESS){
	printf("Warning: Could not find key %s\n", keyName);
	return false;
      }
      INT LowerThresholds[lower_threshold_key.num_values];
      int size = sizeof(LowerThresholds);
      if(db_get_value(hDB, 0, keyName, LowerThresholds, &size, TID_INT, 0) != DB_SUCCESS){
	printf("Warning: Could not retrieve values for key %s\n", keyName);
	return false;
      }


      // Upper threshold
      sprintf(keyName, "/Equipment/Crate 9/Settings/NFADC %s/Channel %d/Upper threshold", iAddr.c_str(), iChn);
      if(db_find_key(hDB,0,keyName, &hKey) != SUCCESS){
	printf("Warning: Could not find key %s\n", keyName);
	return false;
      }
      KEY upper_threshold_key;
      if(db_get_key(hDB, hKey, &upper_threshold_key) != DB_SUCCESS){
	printf("Warning: Could not find key %s\n", keyName);
	return false;
      }
      INT UpperThresholds[upper_threshold_key.num_values];
      size = sizeof(UpperThresholds);
      if(db_get_value(hDB, 0, keyName, UpperThresholds, &size, TID_INT, 0) != DB_SUCCESS){
	printf("Warning: Could not retrieve values for key %s\n", keyName);
	return false;
      }

      // Print the results
      DQ_Thresholds_histograms_map[bankname]->Fill("lower threshold", LowerThresholds[0]);
      DQ_Thresholds_histograms_map[bankname]->Fill("upper threshold", UpperThresholds[0]);
    }
    else if (TSetupData::Instance()->IsHoustonCAEN(bankname)) {
      // get the UH CAEN thresholds

      // first get the channel and address from the bankname
      int iChn = (int)(bankname[1] - 97);

      // Get the threshold
      sprintf(keyName, "/Equipment/Crate 4/Settings/CAEN0/Ch%d/trigger threshhold", iChn);
      if(db_find_key(hDB,0,keyName, &hKey) != SUCCESS){
	printf("Warning: Could not find key %s\n", keyName);
	return false;
      }
      KEY threshold_key;
      if(db_get_key(hDB, hKey, &threshold_key) != DB_SUCCESS){
	printf("Warning: Could not find key %s\n", keyName);
	return false;
      }
      DWORD Thresholds[threshold_key.num_values];
      int size = sizeof(Thresholds);
      if(db_get_value(hDB, 0, keyName, Thresholds, &size, TID_DWORD, 0) != DB_SUCCESS){
	printf("Warning: Could not retrieve values for key %s\n", keyName);
	return false;
      }
      DQ_Thresholds_histograms_map[bankname]->Fill("trigger threshold", Thresholds[0]);
    }
    else if (TSetupData::Instance()->IsBostonCAEN(bankname)) {
      // get the BU CAEN thresholds

      // first get the channel and address from the bankname
      int iChn = (int)(bankname[1] - 97);

      // Get the threshold
      sprintf(keyName, "/Equipment/Crate 5/Settings/CAEN/Ch0%d/self_trigger_threshhold", iChn);
      if(db_find_key(hDB,0,keyName, &hKey) != SUCCESS){
	printf("Warning: Could not find key %s\n", keyName);
	return false;
      }
      KEY threshold_key;
      if(db_get_key(hDB, hKey, &threshold_key) != DB_SUCCESS){
	printf("Warning: Could not find key %s\n", keyName);
	return false;
      }
      float Thresholds[threshold_key.num_values];
      int size = sizeof(Thresholds);
      if(db_get_value(hDB, 0, keyName, Thresholds, &size, TID_FLOAT, 0) != DB_SUCCESS){
	printf("Warning: Could not retrieve values for key %s\n", keyName);
	return false;
      }
      DQ_Thresholds_histograms_map[bankname]->Fill("self_trigger_threshold", Thresholds[0]);
    }

    //add Thresholds for R15a

    //----------------------v1724--------------------------//
    if((TSetupData::IsWFD(bankname)) && (bankname[1] == '4')){
      // first get the channel and address from the bankname
      int iChn = (int)(bankname[3] - 48);

      // Get the threshold
      sprintf(keyName, "/Equipment/Crate 4/Settings/CAEN0/Ch%d/trigger threshhold", iChn);
      if(db_find_key(hDB,0,keyName, &hKey) != SUCCESS){
	printf("Warning: Could not find key %s\n", keyName);
	return false;
      }
      KEY threshold_key;
      if(db_get_key(hDB, hKey, &threshold_key) != DB_SUCCESS){
	printf("Warning: Could not find key %s\n", keyName);
	return false;
      }
      DWORD Thresholds[threshold_key.num_values];
      int size = sizeof(Thresholds);
      if(db_get_value(hDB, 0, keyName, Thresholds, &size, TID_DWORD, 0) != DB_SUCCESS){
	printf("Warning: Could not retrieve values for key %s\n", keyName);
	return false;
      }
      //std::cout << bankname << "   "  << Thresholds[0] << std::endl;

      DQ_Thresholds_histograms_map[bankname]->Fill("trigger threshold", Thresholds[0]);
    }

    //---------------------dt5720---------------------------//
    if(TSetupData::IsWFD(bankname) && bankname[1] == '5'){
      // first get the channel and address from the bankname
      int iChn = (int)(bankname[3] - 48);

      // Get the threshold
      sprintf(keyName, "/Equipment/Crate 5/Settings/CAEN/Ch0%d/Self trigger threshold DPP", iChn);
      if(db_find_key(hDB,0,keyName, &hKey) != SUCCESS){
	printf("Warning: Could not find key %s\n", keyName);
	return false;
      }
      KEY threshold_key;
      if(db_get_key(hDB, hKey, &threshold_key) != DB_SUCCESS){
	printf("Warning: Could not find key %s\n", keyName);
	return false;
      }
      WORD Thresholds[threshold_key.num_values];
      int size = sizeof(Thresholds);
      if(db_get_value(hDB, 0, keyName, Thresholds, &size, TID_WORD, 0) != DB_SUCCESS){
	printf("Warning: Could not retrieve values for key %s\n", keyName);
	return false;
      }
      //std::cout << bankname << "   "  << Thresholds[0] << std::endl;

      DQ_Thresholds_histograms_map[bankname]->Fill("self_trigger_threshold DPP", Thresholds[0]);
    }

    //---------------------dt5730----------------------------//
    if(TSetupData::IsWFD(bankname) && bankname.at(1) == '7'){
      // first get the channel and address from the bankname
      int iChn = (int)(bankname[3] - 48);

      // Get the threshold
      sprintf(keyName, "/Equipment/Crate 7/Settings/CAEN/Ch0%d/Self trigger threshold DPP", iChn);
      if(db_find_key(hDB,0,keyName, &hKey) != SUCCESS){
	printf("Warning: Could not find key %s\n", keyName);
	return false;
      }
      KEY threshold_key;
      if(db_get_key(hDB, hKey, &threshold_key) != DB_SUCCESS){
	printf("Warning: Could not find key %s\n", keyName);
	return false;
      }
      WORD Thresholds[threshold_key.num_values];
      int size = sizeof(Thresholds);
      if(db_get_value(hDB, 0, keyName, Thresholds, &size, TID_WORD, 0) != DB_SUCCESS){
	printf("Warning: Could not retrieve values for key %s\n", keyName);
	return false;
      }
      //std::cout << bankname << "   "  << Thresholds[0] << std::endl;

      DQ_Thresholds_histograms_map[bankname]->Fill("Self_trigger_threshold DPP", Thresholds[0]);
    }

    //-----------------------v1720--------------------------//
    if(TSetupData::IsWFD(bankname) && bankname.at(1) == '8'){
      // first get the channel and address from the bankname
      int iChn = (int)(bankname[3] - 48);

      // Get the threshold
      sprintf(keyName, "/Equipment/Crate 8/Settings/CAEN0/Ch%d/Trigger threshhold", iChn);
      if(db_find_key(hDB,0,keyName, &hKey) != SUCCESS){
	printf("Warning: Could not find key %s\n", keyName);
	return false;
      }
      KEY threshold_key;
      if(db_get_key(hDB, hKey, &threshold_key) != DB_SUCCESS){
	printf("Warning: Could not find key %s\n", keyName);
	return false;
      }
      DWORD Thresholds[threshold_key.num_values];
      int size = sizeof(Thresholds);
      if(db_get_value(hDB, 0, keyName, Thresholds, &size, TID_DWORD, 0) != DB_SUCCESS){
	printf("Warning: Could not retrieve values for key %s\n", keyName);
	return false;
      }
      //std::cout << bankname << "   "  << Thresholds[0] << std::endl;

      DQ_Thresholds_histograms_map[bankname]->Fill("Trigger threshhold", Thresholds[0]);
    }

  }

  return SUCCESS;
}


INT MDQ_Thresholds(EVENT_HEADER *pheader, void *pevent)
{

	// Don't need anything here

	return SUCCESS;
}

/// @}
