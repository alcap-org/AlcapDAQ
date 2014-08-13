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

// Declare all our caches
std::map<IDs::channel, double> SetupNavigator::fPedestalValues;
std::map<IDs::channel, double> SetupNavigator::fNoiseValues;

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

void SetupNavigator::CacheCalibDB() {
  // Cache all the variables we have in the database
  // First the pedestals and noises
  fTableName = "pedestals_and_noises";
  if (!fServer->HasTable(fTableName.c_str())) { // check it exists
    std::cout << "SetupNavigator: ERROR: Table " << fTableName << " does not exist." << std::endl;
  } else if (!fServer->HasTable("GrossTimeOffset")) {
    std::cout << "SetupNavigator: ERROR: Table " << "GrossTimeOffset" << " does not exist." << std::endl;
  } else {
    if (fPedestalValues.empty() && fNoiseValues.empty()) {
      ReadPedestalAndNoiseValues();
      ReadGrossTimeOffsetValues();
    }
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


void SetupNavigator::ReadGrossTimeOffsetValues() {
  // The values that we will read in
  const std::vector<std::string> table = GetGrossTimeOffsetGeneratorColumns();

  std::stringstream query;
  query << "SELECT channel";
  for (unsigned int i = 0; i < table.size(); ++i)
    query << ',' << table[i];
  query << " FROM " << "GrossTimeOffset" << " WHERE run==" << GetRunNumber();
  TSQResult* res = fServer->Query(query.str().c_str());

  TSQLRow* row;
  while (row = res->Next()) {
    for (unsigned int i = 0; i < table.size(); ++i) {
      std::stringstream srcstr;
      srcstr << row->GetField(0) << IDs::field_separator << table[i] << IDs::field_separator << IDs::kAnyConfig;
      fGrossTimeOffset[IDs::source(srcstr.str())] = row->GetField(1 + i);
    }
    delete row;
  }
  delete result;
}

std::vector<std::string> SetupNavigator::GetGrossTimeOffsetColumns() {
  std::vector<std::string> tables;
  TSQLResult* res = fServer->Query("PRAGMA table_info(" << "GrossTimeOffset" << ")");
  TSQLRow* row;
  while (row = res->Next()) {
    if (row->GetField(1) != "run" && row->GetField(1) != "channel")
      tables.push_back(std::string(row->GetField(1)));
    delete row;
  }
  delete res;
  return tables;
}

void SetupNavigator::SetGrossTimeOffset(const IDs::source& src, double dt) {
  std::stringstream cmd;

  // Create table and columns if they don't exist
  server->Exec("CREATE TABLE IF NOT EXIST " << "GrossTimeOffset" << "(run INT, channel TEXT)");
  CreateColumnIfNotExist("GrossTimeOffset", src.Generator().Type(), "TEXT");

  cmd << "SELECT * FROM " << "GrossTimeOffset" << " WHERE run==" << GetRunNumber() << " AND channel=='" << src.Channel().str();
  TSQLResult* res = server->Query(cmd.str().c_str());

  cmd.str("");
  if (res->GetRowCount())
    cmd <<
      "UPDATE " << "GrossTimeOffset" << " SET " << src.Generator().Type() << "=" << dt <<
      " WHERE run==" << GetRunNumber() << " AND channel==" <<
      src.Channel().str();
  else
    cmd <<
      "INSERT INTO " << "GrossTimeOffset" << "(run,channel," << src.Generator().Type() <<
      ") VALUE (" << GetRunNumber() << "," << src.Channel().str() << "," <<
      dt << ")";
  server->Exec(cmd.str().c_str());
  server->Commit();

  delete res;
}

void SetupNavigator::CreateColumnIfNotExist(const std::string& tab, const std::string& col, const std::string& type) {
  std::stringstream cmd;
  cmd << "PRAGMA table_info(" << tab << ")";
  TSQLResult* res = fServer->Query(cmd.str().c_str());
  TSQLRow* row;
  while (row = res->Next()) {
    if (col == row->GetField(1)) {
      delete row, res;
      return;
    }
    delete row;
  }
  delete res;

  cmd.str("");
  cmd << "ALTER TABLE " << tab << " ADD COLUMN " << col << " " << type;
  fServer->Exec(cmd.str().c_str());
}
