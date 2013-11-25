#include "TSetupData.h"
#include "midas.h"

extern HNDLE hDB;

TSetupData::TSetupData() {
  // Go through and fill out the bank name to detector map
  HNDLE hKey;
  KEY key;

  // Loop through all the elements and get the bank and detector name
  for (int iElement = 0; iElement < 48; iElement++) {
    // Find the WireMap key
    if(db_find_key(hDB,0,"Analyzer/WireMap/BankName", &hKey) == SUCCESS) {
    
      // Found the key so retrieve it
      db_get_key(hDB, hKey, &key);

      char bankname[5];
      int size = sizeof(bankname);

      db_get_data_index(hDB, hKey, &bankname, &size, iElement, TID_STRING);
      printf("Bank #%d: %s\n", iElement, bankname);
      
      // Find the detector name key
      if(db_find_key(hDB,0,"Analyzer/WireMap/DetectorName", &hKey) == SUCCESS) { // find the bank
	  

	char detectorname[80];
	int size = sizeof(detectorname);
	db_get_data_index(hDB, hKey, &detectorname, &size, iElement, TID_STRING);
	printf("Detector Name: %s\n", detectorname);

	std::pair<std::string, std::string> thePair(bankname, detectorname);
	fBankToDetectorMap[bankname] = detectorname;
      }
    }
  }
}

ClassImp(TSetupData)
