#include "SetupNavigator.h"
#include "ModulesParser.h"
#include "TSetupData.h"
#include "definitions.h"

#include <TSQLiteServer.h>
#include <TSQLiteResult.h>
#include <TSQLiteRow.h>

#include <cstdlib>
#include <iostream>
#include <sstream>

#include <TSQLiteResult.h>
#include <TSQLiteRow.h>

#include <cstdlib>
#include <iostream>
#include <sstream>

#include "AlcapExcept.h"

MAKE_EXCEPTION(NoCalibDB, Base);

SetupNavigator* SetupNavigator::fThis=NULL;

SetupNavigator::SetupNavigator() {
  fSQLiteFilename="sqlite://pedestals-and-noises.sqlite";
  fServer = new TSQLiteServer(fSQLiteFilename.c_str());

  if (fServer->IsZombie()) {
    std::cout << "SetupNavigator: ERROR: Couldn't connect to SQLite database." << std::endl;
    throw Except::NoCalibDB();
  }
}

SetupNavigator::~SetupNavigator() {

  fServer->Close();

  if (fThis) 
    delete fThis;
}

std::string SetupNavigator::GetBank(const IDs::channel& src)const{
    TSetupData* setup=TSetupData::Instance();
    std::map<std::string, std::string>* bankDetNameMap=&setup->fBankToDetectorMap;
    for(std::map<std::string, std::string>::iterator it = bankDetNameMap->begin();
            it != bankDetNameMap->end(); ++it){
        if(modules::parser::iequals(it->second , src.str())) return it->first;
    }
    return "invalid-det-bank";
}

double SetupNavigator::GetPedestalError(std::string bankname) {

  // The values that we will read in
  double noise=definitions::DefaultValue;

  std::stringstream query; 
  std::string tablename = "pedestals_and_noises";
  int run_number = GetRunNumber(); // get this run number (Note that if we don't have a catalogue of pedestals and noises for each run then we will want to change this)

  query << "SELECT * FROM " << tablename << " WHERE bank=\'" << bankname << "\' AND run=" << run_number << ";"; // get all the pedestals and noises
  TSQLiteResult* result = (TSQLiteResult*) fServer->Query(query.str().c_str());  // get the result of this query
  query.str(""); // clear the stringstream after use
  
  TSQLiteRow* row = (TSQLiteRow*) result->Next(); // get the first row
  while (row != NULL) {
    noise = atof(row->GetField(4));
    
    delete row;
    row = (TSQLiteRow*) result->Next(); // get the next row
  }
  delete result; // user has to delete the result

  return noise;
}

double SetupNavigator::GetPedestal(std::string bankname) {

  // The values that we will read in
  double pedestal=definitions::DefaultValue;

  std::stringstream query; 
  std::string tablename = "pedestals_and_noises";
  int run_number = GetRunNumber(); // get this run number (Note that if we don't have a catalogue of pedestals and noises for each run then we will want to change this)

  query << "SELECT * FROM " << tablename << " WHERE bank=\'" << bankname << "\' AND run=" << run_number << ";"; // get all the pedestals and noises
  TSQLiteResult* result = (TSQLiteResult*) fServer->Query(query.str().c_str());  // get the result of this query
  query.str(""); // clear the stringstream after use
  
  TSQLiteRow* row = (TSQLiteRow*) result->Next(); // get the first row
  while (row != NULL) {
    pedestal = atof(row->GetField(3));
    
    delete row;
    row = (TSQLiteRow*) result->Next(); // get the next row
  }
  delete result; // user has to delete the result

  return pedestal;
}
