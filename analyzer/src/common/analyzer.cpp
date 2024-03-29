/// \defgroup alcapana alcapana
/// \brief
/// Program for first stage of data analysis.

/// \defgroup dq Data Quality
/// \brief
/// Quality checks

/// \defgroup lldq Low Level Data Quality Checks
/// \ingroup dq
/// \ingroup alcapana
/// \brief
/// Low level data quality checks.

/// \defgroup process_raw Raw Data Process
/// \ingroup alcapana
/// \brief
/// Modules that are used in production of TPIs from MIDAS banks
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \defgroup alcapana_main
/// \ingroup alcapana
/// \author Stefan Ritt
///
/// \brief
/// System part of the Analyzer code.
/// @{
////////////////////////////////////////////////////////////////////////////////

/* standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>

/* midas includes */
#include "midas.h"

/* cernlib includes */
#ifdef OS_WINNT
#define VISUAL_CPLUSPLUS
#endif
#ifdef __linux__
#define f2cFortran
#endif

#ifdef HAVE_HBOOK
#include <cfortran.h>
#include <hbook.h>

PAWC_DEFINE(1000000);
#endif /* HAVE_HBOOK */

/* ROOT includes */
#include <TTree.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TVacuumData.h"
#include "TTrendTree.h"

/*-- Globals -------------------------------------------------------*/

/* The analyzer name (client name) as seen by other MIDAS clients   */
char *analyzer_name = "Analyzer";

/* analyzer_loop is called with this interval in ms (0 to disable)  */
INT  analyzer_loop_period = 0;

/* default ODB size */
INT  odb_size = DEFAULT_ODB_SIZE;

/// \brief
/// Object to hold data used and produced by modules throughout alcapana stage
/// of analysis.
TGlobalData* gData;
/// \brief
/// Hardware information about digitizers and detectors to be used during
/// alcapana stage of analysis.
TSetupData* gSetup;
/// \brief
/// Vacuum data slow control.
TVacuumData* gVacuum;
/// \brief
/// Trend tree for trend plots
TTrendTree* gTrendTree;

TSetupData* TSetupData::Instance()
{
	return gSetup;
}

TGlobalData* TGlobalData::Instance()
{
	return gData;
}

bool UpdateDetectorBankNameMap(TSetupData *gSetup);

/*-- Module declarations -------------------------------------------*/

#include "MODULES.h"

/*-- Bank definitions ----------------------------------------------*/

BANK_LIST ana_trigger_bank_list[] = {

	{ "" },
};

BANK_LIST ana_vacuum_bank_list[] = {

	{ "" },
};

/*-- Event request list --------------------------------------------*/
extern ANA_MODULE MVacuumHisto_module;
ANA_MODULE *Vacuum_module[] = {
	&MVacuumHisto_module,
	NULL };

ANALYZE_REQUEST analyze_request[] = {

	{ "Vacuum",            /* equipment name */
		{ 24,                    /* event ID */
			TRIGGER_ALL,          /* trigger mask */
			GET_SOME,             /* get some events */
			"SYSTEM",             /* event buffer */
			TRUE,                 /* enabled */
			"", "", },
		NULL,                 /* analyzer routine */
		Vacuum_module,       /* module list */
		ana_vacuum_bank_list,/* bank list */
		1000,                 /* RWNT buffer size */
		TRUE,                 /* Use tests for this event */
	},

	{ "Trigger",            /* equipment name */
		{ 1,                    /* event ID */
			TRIGGER_ALL,          /* trigger mask */
			GET_SOME,             /* get some events */
			"SYSTEM",             /* event buffer */
			TRUE,                 /* enabled */
			"", "", },
		NULL,                 /* analyzer routine */
		trigger_module,       /* module list */
		ana_trigger_bank_list,/* bank list */
		1000,                 /* RWNT buffer size */
		TRUE,                 /* Use tests for this event */
	},

	{ "" },
};

/*-- Analyzer Init -------------------------------------------------*/

static void catastrophe(int signum)
{
#if 0
	static bool previously_caught_signal = false;
#endif

	printf("Caught signal %d, exiting...", signum);

#if 0
	if(!previously_caught_signal) {
		previously_caught_signal = true;

#ifdef HAVE_ROOT
		printf("Attempting to save histograms...\n");
		extern void CloseRootOutputFile();
		CloseRootOutputFile();
#endif
	}
#endif

	exit(-signum);
}

INT analyzer_init()
{
	bool ret;

	// Initialize gData
	gData = new TGlobalData();

	// Initialize gSetup
	gSetup = new TSetupData();
	ret = UpdateDetectorBankNameMap(gSetup);
	if (!ret) {
		printf("THERE WAS AN ERROR!\n");
		return 0;
	}
	// Override ROOT's handling of signals
	signal(SIGHUP , catastrophe);
	signal(SIGINT , catastrophe);
	signal(SIGQUIT , catastrophe);
	signal(SIGILL , catastrophe);
	signal(SIGABRT , catastrophe);
	signal(SIGFPE , catastrophe);
	signal(SIGKILL , catastrophe);
	signal(SIGSEGV , catastrophe);
	signal(SIGPIPE , catastrophe);
	signal(SIGTERM , catastrophe);

	// Initialize gVacuum
	gVacuum = new TVacuumData();

	// Do not initialize trend tree.
	// MTrendTree will initialize if
	// the module is included.
	gTrendTree = new TTrendTree();
	
	return SUCCESS;
}

/*-- Analyzer Exit -------------------------------------------------*/

INT analyzer_exit()
{
	if(gData) {
		delete gData;
		gData = NULL;
	}

	if(gSetup) {
		delete gSetup;
		gSetup = NULL;
	}

	if(gVacuum) {
		delete gVacuum;
		gVacuum = NULL;
	}

	return CM_SUCCESS;
}

/*-- Begin of Run --------------------------------------------------*/

INT ana_begin_of_run(INT run_number, char *error)
{
	printf("Analyzer saw beginning of run %d\n", run_number);
	return CM_SUCCESS;
}

/*-- End of Run ----------------------------------------------------*/

INT ana_end_of_run(INT run_number, char *error)
{
	printf("Analyzer saw end of run %d\n", run_number);
	return CM_SUCCESS;
}

/*-- Pause Run -----------------------------------------------------*/

INT ana_pause_run(INT run_number, char *error)
{
	return CM_SUCCESS;
}

/*-- Resume Run ----------------------------------------------------*/

INT ana_resume_run(INT run_number, char *error)
{
	return CM_SUCCESS;
}

/*-- Analyzer Loop -------------------------------------------------*/

INT analyzer_loop()
{
	return CM_SUCCESS;
}

/*------------------------------------------------------------------*/
//}

bool UpdateDetectorBankNameMap(TSetupData *gSetup){
	// Want to go through the /Analyzer/WireMap and map bank names and detector names 
	bool ret = true;
	HNDLE hDB, hKey;
	char keyName[200];

	if(cm_get_experiment_database(&hDB, NULL) != CM_SUCCESS){
		printf("Warning: Could not connect to ODB database!\n");
		return false;
	}

	sprintf(keyName, "/Analyzer/WireMap/BankName");
	if(db_find_key(hDB,0,keyName, &hKey) != SUCCESS){
		printf("Warning: Could not find key %s\n", keyName);
		return false;
	}
	KEY bk_key;
	if(db_get_key(hDB, hKey, &bk_key) != DB_SUCCESS){
		printf("Warning: Could not find key %s\n", keyName);
		return false;
	}    
	char BankNames[bk_key.num_values][bk_key.item_size];
	int size = sizeof(BankNames);
	if(db_get_value(hDB, 0, keyName , BankNames, &size, TID_STRING, 0) != DB_SUCCESS){
		printf("Warning: Could not retrieve values for key %s\n", keyName);
		ret = false;
	}

	sprintf(keyName, "/Analyzer/WireMap/DetectorName");
	if(db_find_key(hDB,0,keyName, &hKey) != SUCCESS){
		printf("Warning: Could not find key %s\n", keyName);
		return false;
	}
	KEY det_key;
	if(db_get_key(hDB, hKey, &det_key) != DB_SUCCESS){
		printf("Warning: Could not find key %s\n", keyName);
		return false;
	}
	char DetectorNames[det_key.num_values][det_key.item_size];
	size = sizeof(DetectorNames);
	if(db_get_value(hDB, 0, keyName , DetectorNames, &size, TID_STRING, 0) != DB_SUCCESS){
		printf("Warning: Could not retrieve values for key %s\n", keyName);
		ret = false;
	}

	sprintf(keyName, "/Analyzer/WireMap/TimeShift");
	if(db_find_key(hDB,0,keyName, &hKey) != SUCCESS){
		printf("Warning: Could not find key %s\n", keyName);
		return false;
	}
	KEY timeshift_key;
	if(db_get_key(hDB, hKey, &timeshift_key) != DB_SUCCESS){
		printf("Warning: Could not find key %s\n", keyName);
		return false;
	}
	double TimeShifts[timeshift_key.num_values];
	size = sizeof(TimeShifts);
	if(db_get_value(hDB, 0, keyName, TimeShifts, &size, TID_DOUBLE, 0) != DB_SUCCESS){
		printf("Warning: Could not retrieve values for key %s\n", keyName);
		ret = false;
	}

	sprintf(keyName, "/Analyzer/WireMap/Enabled");
	if(db_find_key(hDB,0,keyName, &hKey) != SUCCESS){
		printf("Warning: Could not find key %s\n", keyName);
		return false;
	}
	KEY enablebit_key;
	if (db_get_key(hDB, hKey, &enablebit_key) != DB_SUCCESS)
	{
		printf("Warning: Could not find key %s\n", keyName);
		return false;
	}
	int EnableBitsBuffer[enablebit_key.num_values];
	size = sizeof(EnableBitsBuffer);
	//printf("sizeof EnableBitsBuffer %d\n", size);
	if(db_get_value(hDB, 0, keyName , EnableBitsBuffer, &size, TID_BOOL, 0) != DB_SUCCESS){
		printf("Warning: Could not retrieve values for key %s\n", keyName);
		ret = false;
	}

	bool EnableBits[enablebit_key.num_values];

	int jj = 0;
	for (jj = 0; jj < enablebit_key.num_values; ++jj)
	{
		EnableBits[jj] = EnableBitsBuffer[jj] == 1 ? 1:0;
		//printf("jj %d: %d %d\n", jj, EnableBitsBuffer[jj], EnableBits[jj]);
	}/* code */


	sprintf(keyName, "/Analyzer/WireMap/TriggerPolarity");
	if(db_find_key(hDB,0,keyName, &hKey) != SUCCESS){
		printf("Warning: Could not find key %s\n", keyName);
		return false;
	}  
	KEY pol_key;
	if(db_get_key(hDB, hKey, &pol_key) != DB_SUCCESS){
		printf("Warning: Could not find key %s\n", keyName);
		return false;
	}
	int TriggerPolarities[pol_key.num_values];
	size = sizeof(TriggerPolarities);
	if(db_get_value(hDB, 0, keyName , TriggerPolarities, &size, TID_INT, 0) != DB_SUCCESS){
		printf("Warning: Could not retrieve values for key %s\n", keyName);
		ret = false;
	}

	sprintf(keyName, "/Analyzer/WireMap/Pedestal");
	if(db_find_key(hDB,0,keyName, &hKey) != SUCCESS){
		printf("Warning: Could not find key %s\n", keyName);
		return false;
	}  
	KEY ped_key;
	if(db_get_key(hDB, hKey, &ped_key) != DB_SUCCESS){
		printf("Warning: Could not find key %s\n", keyName);
		return false;
	}
	int Pedestals[ped_key.num_values];
	size = sizeof(Pedestals);
	if(db_get_value(hDB, 0, keyName , Pedestals, &size, TID_INT, 0) != DB_SUCCESS){
		printf("Warning: Could not retrieve values for key %s\n", keyName);
		ret = false;
	}

	if(det_key.num_values != bk_key.num_values){
		printf("Warning: Key sizes are not equal for banks and detectors in /Analyzer/WireMap/\n");
		ret = false;
	}
	else printf("sizes are %d\n", det_key.num_values);

	std::vector<bool> duplicate_dets(det_key.num_values, false); // Keep track of duplicate detector names, and check later if they'll be a problem
	for(int i=0; i<det_key.num_values; i++){
		if(strcmp(BankNames[i], "") == 0) {
			printf("Found bank name that is empty! (Index %d)\n", i);
			ret = false;
			continue;
		}
		if(strcmp(DetectorNames[i], "") == 0) printf("Warning: No detector name associated with bank %s!\n", BankNames[i]);
		///////////////////////////////////////
		// Add the detector names to TSetupData
		// Should be a bidirectional map. Lacking that functionality
		// we simply check if the value detector exists
		std::string bank_name(BankNames[i]), detector(DetectorNames[i]);
		if(detector != "blank" && //!detector.empty() && //!bank_name.empty()
		   !gSetup->SetDetectorName(bank_name,detector)) {
			printf("WARNING: Detector %s listed multiple times in ODB! (Duplicate index %d)\n", detector.c_str(), i);
			duplicate_dets.at(i) = true;
		}
		gSetup->SetTriggerPolarity(bank_name,TriggerPolarities[i]);
		gSetup->SetPedestal(bank_name,Pedestals[i]);
		gSetup->SetTimeShift(bank_name,TimeShifts[i]);
		gSetup->SetEnableBit(bank_name,EnableBits[i]);
		//printf("%s: %d\n", bank_name.c_str(), gSetup->GetEnableBit(bank_name));

		//////////////////////////////////////////////////
		// Calculate and the add the clock ticks to TSetupData
		int DCMPhase = 1; // assume DCMPhase = 1 for most digitizers (it's only applicable to the FADCs)
		if(TSetupData::IsFADC(bank_name)){ 
			int iChn = (int)(bank_name[1] - 97);
			std::string iAddr = bank_name.substr(2, 2);

			sprintf(keyName, "/Equipment/Crate 9/Settings/NFADC %s/Channel %d/DCM phase", iAddr.c_str(), iChn);
			if(db_find_key(hDB,0,keyName, &hKey) == SUCCESS){
				db_get_key(hDB, hKey, &bk_key);

				int size = sizeof(DCMPhase);
				if(db_get_value(hDB, 0, keyName , &DCMPhase, &size, TID_INT, 0) == DB_SUCCESS){
					//	  printf("Found the DCM Phase!\n");
				}
			}
			//
			//////////////////////////////////////////////////
			// Get the sampling frequency of the different digitizers
			sprintf(keyName, "/Analyzer/WireMap/SamplingFrequency");
			if(db_find_key(hDB, 0, keyName, &hKey) == SUCCESS){
				std::string bank_name(BankNames[i]);
				double frequency = 170E6 / DCMPhase;
				int size = sizeof(double);
				db_set_data_index(hDB, hKey, &frequency, size, i, TID_DOUBLE);
			}
		}

		sprintf(keyName, "/Analyzer/WireMap/SamplingFrequency");  
		if(db_find_key(hDB,0,keyName, &hKey) != SUCCESS){
			printf("Warning: Could not find key %s\n", keyName);
			return false;
		}  
		KEY freq_key;
		if(db_get_key(hDB, hKey, &freq_key) != DB_SUCCESS){
			printf("Warning: Could not find key %s\n", keyName);
			return false;
		}
		double SamplingFrequencies[freq_key.num_values];
		size = sizeof(SamplingFrequencies);
		if(db_get_value(hDB, 0, keyName , SamplingFrequencies, &size, TID_DOUBLE, 0) != DB_SUCCESS){
			printf("Warning: Could not retrieve values for key %s\n", keyName);
			ret = false;
		}

		double clockTickInNs = (1/SamplingFrequencies[i]) * 1e9;
		//    printf("Bank %s: f = %f, clockTick = %f\n", bank_name.c_str(), true_frequency, clockTickInNs);
		gSetup->SetClockTick(bank_name,clockTickInNs);

		//////////////////////////////////////////////////
		// ADC calibration constants
		sprintf(keyName, "/Analyzer/WireMap/ADCSlopeCalib");  
		if(db_find_key(hDB,0,keyName, &hKey) != SUCCESS){
			printf("Warning: Could not find key %s\n", keyName);
			return false;
		}  
		KEY adc_slope_calib_key;
		if(db_get_key(hDB, hKey, &adc_slope_calib_key) != DB_SUCCESS){
			printf("Warning: Could not find key %s\n", keyName);
			return false;
		}
		double ADCSlopeCalibs[adc_slope_calib_key.num_values];    
		size = sizeof(ADCSlopeCalibs);
		if(db_get_value(hDB, 0, keyName , ADCSlopeCalibs, &size, TID_DOUBLE, 0) != DB_SUCCESS){
			printf("Warning: Could not retrieve values for key %s\n", keyName);
			ret = false;
		}

		sprintf(keyName, "/Analyzer/WireMap/ADCOffsetCalib");  
		if(db_find_key(hDB,0,keyName, &hKey) != SUCCESS){
			printf("Warning: Could not find key %s\n", keyName);
			return false;
		}  
		KEY adc_offset_calib_key;
		if(db_get_key(hDB, hKey, &adc_offset_calib_key) != DB_SUCCESS){
			printf("Warning: Could not find key %s\n", keyName);
			return false;
		}
		double ADCOffsetCalibs[adc_offset_calib_key.num_values];    
		size = sizeof(ADCOffsetCalibs);
		if(db_get_value(hDB, 0, keyName , ADCOffsetCalibs, &size, TID_DOUBLE, 0) != DB_SUCCESS){
			printf("Warning: Could not retrieve values for key %s\n", keyName);
			ret = false;
		}


		double adcSlopeValue = ADCSlopeCalibs[i];
		double adcOffsetValue = ADCOffsetCalibs[i];
		gSetup->SetADCSlopeCalib(bank_name,adcSlopeValue);
		gSetup->SetADCOffsetCalib(bank_name,adcOffsetValue);

		///////////////////////////////////////
		// Check to see if the bank is enabled   
		if(TSetupData::IsFADC(bank_name)) { // FADC banks
			std::string iAddr(bank_name);
			int iChn = (int)(iAddr[1] - 97);
			iAddr = iAddr.substr(2, 2);

			char wireKey[100];
			BOOL enabled = false;
			int size = sizeof(enabled);
			sprintf(wireKey, "/Analyzer/WireMap/Enabled");
			if(db_find_key(hDB,0,wireKey, &hKey) == SUCCESS){
				// Let's first reset /Analyzer/WireMap/Enabled for this channel to 'n'
				db_set_data_index(hDB, hKey, &enabled, size, i, TID_BOOL);

				sprintf(keyName, "/Equipment/Crate 9/Settings/NFADC %s/Enabled", iAddr.c_str());
				if(db_get_value(hDB, 0, keyName , &enabled, &size, TID_BOOL, 0) == DB_SUCCESS) {
					if(enabled){
						sprintf(keyName, "/Equipment/Crate 9/Settings/NFADC %s/Channel %d/Trigger mask", iAddr.c_str(), iChn);
						int trigger_mask;
						size = sizeof(trigger_mask);
						if(db_get_value(hDB, 0, keyName , &trigger_mask, &size, TID_INT, 0) == DB_SUCCESS) {
							if (trigger_mask == 1){ // if this channel is taking data
								db_set_data_index(hDB, hKey, &enabled, size, i, TID_BOOL);	
							}
						} else { // We found the channel 'Trigger mask' key in ODB
							printf("Could not find Trigger Mask key in ODB!\n");
							ret = false;
						}
					} else { // This module is enabled
						if (duplicate_dets.at(i)) {
							printf("ATTENTION: Duplicate detector at index %d resolved (not enabled).\n", i);
							duplicate_dets.at(i) = false;
						}
					}
				} // We got the value of the module 'Enable' key in ODB

			} else { // We found the 'Enabled' key in the ODB
				printf("Could not find Enabled key in ODB!\n");
				ret = false;
			}
		} else if (TSetupData::IsHoustonCAEN(bank_name)) {
			BOOL enabled = true;
			int size;
			char aCh = bank_name[1];
			int iCh = aCh - 'a'; // a->0, b->1, etc.

			size = sizeof(enabled);
			sprintf(keyName, "/Equipment/Crate 4/Settings/CAEN0/Ch%1d/Enabled", iCh);
			if (db_get_value(hDB, 0, keyName, &enabled, &size, TID_BOOL, 0) == DB_SUCCESS) {
				if (!enabled && duplicate_dets.at(i)){
					duplicate_dets[i] = false;
					printf("ATTENTION: Duplicate detector at index %d resolved (not enabled).\n", i);
				}
			} else {
				printf("Could not find UH CAEN Enabled key in ODB!\n");
				ret = false;
			}
		} else if (TSetupData::IsBostonCAEN(bank_name)) {
			BOOL enabled = true;
			int size;
			char aCh = bank_name[1];
			int iCh = aCh - 'a'; // a->0, b->1, etc.

			size = sizeof(enabled);
			sprintf(keyName, "/Equipment/Crate 5/Settings/CAEN/Ch%02d/enable", iCh);
			if (db_get_value(hDB, 0, keyName, &enabled, &size, TID_BOOL, 0) == DB_SUCCESS) {
				if (!enabled && duplicate_dets.at(i)) {
					printf("ATTENTION: Duplicate detector at index %d resolved (not enabled).\n", i);
					duplicate_dets[i] = false;
				}
			} else {
				printf("Could not find BU CAEN Enabled key in ODB!\n");
				ret = false;
			}
		}
	} // end loop over all non empty banks

	// Check if there are duplicate detectors all enabled
	// for (int i = 0; i < duplicate_dets.size(); ++i) {
	// 	if (duplicate_dets.at(i)) {
	// 		printf("ERROR: Multiple detectors enabled! (Index %d)\n", i);
	// 		ret = false;
	// 	}
	// }

	return ret;
}

/// @}
