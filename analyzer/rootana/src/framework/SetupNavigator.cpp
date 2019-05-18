#include "SetupNavigator.h"
#include "ModulesParser.h"
#include "TSetupData.h"
#include "definitions.h"
#include "debug_tools.h"

#include <TSQLiteServer.h>
#include <TSQLiteResult.h>
#include <TSQLiteRow.h>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <TSQLiteResult.h>
#include <TSQLiteRow.h>

SetupNavigator* SetupNavigator::fThis=NULL;

static std::string StripTimeShiftConfigFromString(std::string con) {
  const char *start = "{no_time_shift=", *end = "}";
  std::size_t pos;
  while ( (pos=con.find(start)) != std::string::npos)
    con.replace(pos, con.find(end, pos)-pos+1, "");
  return con;
}


// Declare all our caches
std::map<IDs::channel, double> SetupNavigator::fPedestalValues;
std::map<IDs::channel, double> SetupNavigator::fNoiseValues;
std::map<IDs::source, double> SetupNavigator::fCoarseTimeOffset;
std::map< IDs::channel, SetupNavigator::EnergyCalibRow_t > SetupNavigator::fEnergyCalibrationConstants;

SetupNavigator::SetupNavigator() :
  fCommandLineArgs(),
  fSQLiteFilename("sqlite://calibration.db"), fServer(new TSQLiteServer(fSQLiteFilename.c_str())),
  fPedestalNoiseTableName("PedestalAndNoise"), fCoarseTimeOffsetTableName("CoarseTimeOffset"),
  fEnergyCalibrationConstantsTableName("Energy") {
  if (fServer->IsZombie()) {
    std::cout << "SetupNavigator: ERROR: Couldn't connect to SQLite database." << std::endl;
    throw Except::NoCalibDB();
  }
}


SetupNavigator::~SetupNavigator() {
}


void SetupNavigator::Close() {
  if(IsCalibRun()) 
    OutputCalibCSV();
  fServer->Close();
  if (fThis)
    delete fThis;
  fThis = NULL;
}


std::string SetupNavigator::GetBank(const IDs::channel& src)const{
  std::map<std::string, std::string>* bankDetNameMap=&TSetupData::Instance()->fBankToDetectorMap;
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
  if (!fServer->HasTable(fPedestalNoiseTableName.c_str())) {
    std::cout << "SetupNavigator: ERROR: Table " << fPedestalNoiseTableName << " does not exist." << std::endl;
    if (!IsCalibRun()) throw Except::MissingTable(fPedestalNoiseTableName.c_str());
  } else {
    if (!ReadPedestalAndNoiseValues()) {
      std::cout << "SetupNavigator: ERROR: Table " << fPedestalNoiseTableName 
                << " contains no calib data for this run ("<<GetRunNumber()<<")" << std::endl;
    if (!IsCalibRun()) throw Except::UncalibratedRun();
    }
  }
  if (!fServer->HasTable(fCoarseTimeOffsetTableName.c_str())) {
    std::cout << "SetupNavigator: ERROR: Table " << fCoarseTimeOffsetTableName << " does not exist." << std::endl;
    if(!IsCalibRun()) throw Except::MissingTable(fPedestalNoiseTableName.c_str());
  } else {
    if (!ReadCoarseTimeOffsetValues()) {
      std::cout << "SetupNavigator: ERROR: Table " << fCoarseTimeOffsetTableName 
                << " contains no calib data for this run ("<<GetRunNumber()<<")" << std::endl;
    if (!IsCalibRun()) throw Except::UncalibratedRun();
    }
  }
  if (!fServer->HasTable(fEnergyCalibrationConstantsTableName.c_str())) {
    std::cout << "SetupNavigator: Error: Table " << fEnergyCalibrationConstantsTableName << " does not exist." << std::endl;
    if (!IsCalibRun()) throw Except::MissingTable(fEnergyCalibrationConstantsTableName.c_str());
  } else {
    if (!ReadEnergyCalibrationConstants()) {
      std::cout << "SetupNavigator: ERROR: Table " << fEnergyCalibrationConstantsTableName
                << " contains no calib data for this run ("<<GetRunNumber()<<")" << std::endl;
      if (!IsCalibRun()) throw Except::UncalibratedRun();
    }
  }
}

bool SetupNavigator::ReadPedestalAndNoiseValues() {
  // The values that we will read in
  double pedestal=definitions::DefaultValue;
  double noise=definitions::DefaultValue;
  std::string channelname;

  std::stringstream query;
  int run_number = GetRunNumber(); // get this run number (Note that if we don't have a catalogue of pedestals and noises for each run then we will want to change this)

  query << "SELECT * FROM " << fPedestalNoiseTableName << " WHERE run=" << run_number << ";"; // get all the pedestals and noises
  TSQLiteResult* result = (TSQLiteResult*) fServer->Query(query.str().c_str());  // get the result of this query
  query.str(""); // clear the stringstream after use
  
  TSQLiteRow* row = (TSQLiteRow*) result->Next(); // get the first row
  while (row != NULL) {
    channelname = row->GetField(1);
    pedestal = atof(row->GetField(2));
    noise = atof(row->GetField(3));
    
    fPedestalValues[channelname] = pedestal;
    fNoiseValues[channelname] = noise;
    delete row;
    row = (TSQLiteRow*) result->Next(); // get the next row
  }
  delete result; // user has to delete the result

  return !fPedestalValues.empty();
}


bool SetupNavigator::ReadCoarseTimeOffsetValues() {
  // The values that we will read in
  const std::vector<std::string> table = GetCoarseTimeOffsetColumns();
for(unsigned i=0;i<table.size();++i) DEBUG_VALUE(i, table[i]);

  std::stringstream query;
  query << "SELECT channel";
  for (unsigned int i = 0; i < table.size(); ++i)
    query << ",\"" << table[i] << '"';
  query << " FROM " << fCoarseTimeOffsetTableName << " WHERE run==" << GetRunNumber();
  TSQLResult* res = fServer->Query(query.str().c_str());

  TSQLRow* row;
  while ((row = res->Next())) {
    for (unsigned int i = 0; i < table.size(); ++i) {
      std::stringstream srcstr;
      srcstr << row->GetField(0) << IDs::field_separator << table[i];
      if(row->GetField(1+i))
	     fCoarseTimeOffset[IDs::source(srcstr.str())] = atof(row->GetField(1 + i));
    }
    delete row;
  }
  delete res;
  return !fCoarseTimeOffset.empty();
}

std::vector<std::string> SetupNavigator::GetCoarseTimeOffsetColumns() {
  std::vector<std::string> cols;
  std::stringstream cmd;
  cmd << "PRAGMA table_info(" << fCoarseTimeOffsetTableName << ")";
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


bool SetupNavigator::ReadEnergyCalibrationConstants() {
  std::stringstream cmd;
  // // Ben:  In the future we will want to include generator parameters in the
  // // table to be able to calibrate multiple generators simultaneously:
  // cmd << "SELECT channel,gen_params,gain,offset FROM " << fEnergyCalibrationConstantsTableName 
  cmd << "SELECT channel,gain,offset FROM " << fEnergyCalibrationConstantsTableName
      << " WHERE run==" << GetRunNumber();
  TSQLResult* res = fServer->Query(cmd.str().c_str());
  TSQLRow* row;
  while ((row = res->Next())) {
    fEnergyCalibrationConstants[IDs::channel(row->GetField(0))] =
      EnergyCalibRow_t(atof(row->GetField(1)), atof(row->GetField(2)));
    delete row;
  }
  delete res;
  //  return !fEnergyCalibrationConstants.empty();
  return true; // Right now there's no reason this should be filled for anything
}


void SetupNavigator::OutputCalibCSV() {
  char r[6];
  sprintf(r, "%05d", GetRunNumber());

  std::ofstream fPN((std::string("calib.db/calib.run") + r + "." + fPedestalNoiseTableName + ".csv").c_str());
  fPN << "run,channel,pedestal,noise" << std::endl;
  for (std::map<IDs::channel, double>::const_iterator i = fPedestalValues.begin(); i != fPedestalValues.end(); ++i)
    fPN << GetRunNumber() << "," << i->first.str() << ","
	<< i->second << "," << fNoiseValues.at(i->first) << std::endl;

  std::ofstream fTO((std::string("calib.db/calib.run") + r + "." + fCoarseTimeOffsetTableName + ".csv").c_str());
  std::set<IDs::generator> gens;
  std::set<IDs::channel> chns;
  for (std::map<IDs::source, double>::const_iterator i = fCoarseTimeOffset.begin(); i != fCoarseTimeOffset.end(); ++i) {
    gens.insert(i->first.Generator());
    chns.insert(i->first.Channel());
  }
  fTO << "run,channel";
  for (std::set<IDs::generator>::const_iterator i = gens.begin(); i != gens.end(); ++i)
    fTO << "," << i->str();
  fTO << std::endl;
  for (std::set<IDs::channel>::const_iterator i = chns.begin(); i != chns.end(); ++i) {
    fTO << GetRunNumber() << "," << i->str();
    for (std::set<IDs::generator>::const_iterator j = gens.begin(); j != gens.end(); ++j) {
      fTO << ",";
      if (fCoarseTimeOffset.count(IDs::source(*i, *j)))
	fTO << fCoarseTimeOffset.at(IDs::source(*i, *j));
    }
    fTO << std::endl;
  }
}

void SetupNavigator::SetPedestalAndNoise(const IDs::channel& chn, double ped, double nse) {
  if(!IsCalibRun()) {
    std::cout << "SetupNavigator: Warning: Request to edit pedestals and "
                 "noises when not flagged as calibration. Not setting." << std::endl;
    return;
  }
  fPedestalValues[chn] = ped;
  fNoiseValues[chn] = nse;
}

void SetupNavigator::SetCoarseTimeOffset(const IDs::source& src, double dt) {
  if(!IsCalibRun()) {
    std::cout << "SetupNavigator: Warning: Request to edit coarse time offsets "
                 "when not flagged as calibration. Not setting." << std::endl;
    return;
  }
  // The excess code here is to strip {no_time_shift=*} from the generator config string
  fCoarseTimeOffset[IDs::source(src.Channel(), IDs::generator(StripTimeShiftConfigFromString(src.Generator().str())))] = dt;
}

double SetupNavigator::GetNoise(const IDs::channel& channel) const {
  return alcap::at<Except::InvalidDetector>(fNoiseValues,channel,channel.str().c_str());
}
double SetupNavigator::GetPedestal(const IDs::channel& channel) const { 
  return alcap::at<Except::InvalidDetector>(fPedestalValues,channel,channel.str().c_str());
}

SetupNavigator::EnergyCalibRow_t SetupNavigator::GetEnergyCalibrationConstants(const IDs::channel& ch) const {
  return alcap::at<Except::InvalidDetector>(fEnergyCalibrationConstants, ch, ch.str().c_str());
}
double SetupNavigator::GetCoarseTimeOffset( IDs::source source) const {
// Massive hack to remove config strings that specify other options.  Assumes
// the timing option is placed first for the generator
 std::string conf=source.Generator().Config();
 unsigned curly_br=conf.find('}');
if(curly_br!=std::string::npos){ source.Generator().Config(conf.substr(0,curly_br+1));}
 
 return source.matches(IDs::channel("muSc")) ? 0. : alcap::at<Except::InvalidDetector>(fCoarseTimeOffset,source,source.str().c_str());
 }
