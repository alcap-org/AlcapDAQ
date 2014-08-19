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
MAKE_EXCEPTION(InvalidDetector, Base);

SetupNavigator* SetupNavigator::fThis=NULL;

// Declare all our caches
std::map<IDs::channel, double> SetupNavigator::fPedestalValues;
std::map<IDs::channel, double> SetupNavigator::fNoiseValues;
std::map<IDs::source, double> SetupNavigator::fCoarseTimeOffset;

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
    std::cout << "Invalid detector name: " << src.str() << std::endl;
    throw Except::InvalidDetector();
}

void SetupNavigator::CacheCalibDB() {
  // Cache all the variables we have in the database
  // First the pedestals and noises
  fTableName = "pedestals_and_noises";
  if (!fServer->HasTable(fTableName.c_str())) {// check it exists
    std::cout << "SetupNavigator: ERROR: Table " << fTableName << " does not exist." << std::endl;
    if (!fCommandLineArgs.calib)
      throw Except::NoCalibDB();
  } else {
    ReadPedestalAndNoiseValues();
  }
  if (!fServer->HasTable("CoarseTimeOffset")) {
    std::cout << "SetupNavigator: ERROR: Table " << "CoarseTimeOffset" << " does not exist." << std::endl;
    if (!fCommandLineArgs.calib)
      throw Except::NoCalibDB();
  } else {
    ReadCoarseTimeOffsetValues();
  }
}

void SetupNavigator::ReadPedestalAndNoiseValues() {
  // The values that we will read in
  double pedestal=definitions::DefaultValue;
  double noise=definitions::DefaultValue;
  std::string channelname;
 
  std::stringstream query;
  int run_number = GetRunNumber(); // get this run number (Note that if we don't have a catalogue of pedestals and noises for each run then we will want to change this)

  query << "SELECT * FROM " << fTableName << " WHERE run=" << run_number << ";"; // get all the pedestals and noises
  TSQLiteResult* result = (TSQLiteResult*) fServer->Query(query.str().c_str());  // get the result of this query
  query.str(""); // clear the stringstream after use
  
  TSQLiteRow* row = (TSQLiteRow*) result->Next(); // get the first row
  while (row != NULL) {
    channelname = row->GetField(1);
    pedestal = atof(row->GetField(3));
    noise = atof(row->GetField(4));
    
    fPedestalValues[channelname] = pedestal;
    fNoiseValues[channelname] = noise;
    delete row;
    row = (TSQLiteRow*) result->Next(); // get the next row
  }
  delete result; // user has to delete the result

}


void SetupNavigator::ReadCoarseTimeOffsetValues() {
  // The values that we will read in
  const std::vector<std::string> table = GetCoarseTimeOffsetColumns();

  std::stringstream query;
  query << "SELECT channel";
  for (unsigned int i = 0; i < table.size(); ++i)
    query << ",\"" << table[i] << '"';
  query << " FROM " << "CoarseTimeOffset" << " WHERE run==" << GetRunNumber();
  TSQLResult* res = fServer->Query(query.str().c_str());

  TSQLRow* row;
  while ((row = res->Next())) {
    for (unsigned int i = 0; i < table.size(); ++i) {
      std::stringstream srcstr;
      srcstr << row->GetField(0) << IDs::field_separator << table[i];
      fCoarseTimeOffset[IDs::source(srcstr.str())] = atof(row->GetField(1 + i));
    }
    delete row;
  }
  delete res;
}

std::vector<std::string> SetupNavigator::GetCoarseTimeOffsetColumns() {
  std::vector<std::string> cols;
  std::stringstream cmd;
  cmd << "PRAGMA table_info(" << "CoarseTimeOffset" << ")";
  TSQLResult* res = fServer->Query(cmd.str().c_str());
  TSQLRow* row;
  while ((row = res->Next())) {
    if (row->GetField(1) != std::string("run") && row->GetField(1) != std::string("channel"))
      cols.push_back(std::string(row->GetField(1)));
    delete row;
  }
  delete res;
  return cols;
}

void SetupNavigator::SetCoarseTimeOffset(const IDs::source& src, const double dt) {
  std::stringstream cmd;
  fServer->StartTransaction();
  // Create table and columns if they don't exist
  cmd << "CREATE TABLE IF NOT EXISTS " << "CoarseTimeOffset"
      << "(run INT, channel TEXT)";
  fServer->Exec(cmd.str().c_str());
  CreateColumnIfNotExist("CoarseTimeOffset", src.Generator().str(), "REAL");

  cmd.str("");
  cmd << "SELECT * FROM " << "CoarseTimeOffset" << " WHERE run=="
      << GetRunNumber() << " AND channel=='" << src.Channel().str() << "'";
  TSQLResult* res = fServer->Query(cmd.str().c_str());

  cmd.str("");
  if (res->Next())
    cmd << "UPDATE " << "CoarseTimeOffset" << " SET \"" << src.Generator().str()
	<< "\"=" << dt << " WHERE run==" << GetRunNumber() << " AND channel=='"
	<< src.Channel().str() << "'";
  else
    cmd << "INSERT INTO " << "CoarseTimeOffset" << "(run,channel,\""
	<< src.Generator().str() << "\") VALUES (" << GetRunNumber() << ",'"
	<< src.Channel().str() << "'," << dt << ")";
  std::cout << "Executing SQL command: " << cmd.str() << std::endl;
  fServer->Exec(cmd.str().c_str());
  fServer->Commit();

  delete res;
}

void SetupNavigator::CreateColumnIfNotExist(const std::string& tab, const std::string& col, const std::string& type) {
  std::stringstream cmd;
  cmd << "PRAGMA table_info(" << tab << ")";
  TSQLResult* res = fServer->Query(cmd.str().c_str());
  TSQLRow* row;
  while ((row = res->Next())) {
    if (col == row->GetField(1)) {
      delete row;
      delete res;
      return;
    }
    delete row;
  }
  delete res;

  cmd.str("");
  // Since the table name is the source string, which inclueds the character #,
  // the table name in the SQLite database must be surrounded by double quotes.
  cmd << "ALTER TABLE " << tab << " ADD COLUMN \"" << col << "\" " << type;
  fServer->Exec(cmd.str().c_str());
}
