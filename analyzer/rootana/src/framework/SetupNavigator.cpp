#include "SetupNavigator.h"

#include "definitions.h"
#include "debug_tools.h"
#include "IdBoard.h"
#include "IdChannel.h"
#include "IdSource.h"
#include "ModulesParser.h"
#include "TSetupData.h"
using IDs::board;
using IDs::channel;
using IDs::source;

#include <TSQLiteServer.h>
#include <TSQLiteResult.h>
#include <TSQLiteRow.h>

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <fstream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
using std::map;
using std::vector;
using std::pair;
using std::string;
using std::stringstream;
using std::cout;
using std::endl;


SetupNavigator* SetupNavigator::fThis=NULL;

static string StripTimeShiftConfigFromString(string con) {
  const char *start = "{no_time_shift=", *end = "}";
  std::size_t pos;
  while ( (pos=con.find(start)) != string::npos)
    con.replace(pos, con.find(end, pos)-pos+1, "");
  return con;
}


// Declare all our caches
map<IDs::channel, double> SetupNavigator::fPedestalValues;
map<IDs::channel, double> SetupNavigator::fNoiseValues;
map<IDs::source, double> SetupNavigator::fCoarseTimeOffset;
map<IDs::source, double> SetupNavigator::fCoarseTimeOffsetSigma;
map< IDs::channel, SetupNavigator::EnergyCalibRow_t > SetupNavigator::fEnergyCalibrationConstants;

SetupNavigator::SetupNavigator() :
  fCommandLineArgs(),
  fSQLiteFilename("sqlite://calibration.db"), fServer(new TSQLiteServer(fSQLiteFilename.c_str())),
  fPedestalNoiseTableName("PedestalAndNoise"), fCoarseTimeOffsetTableName("CoarseTimeOffset"),
  fEnergyCalibrationConstantsTableName("Energy"),
  fChannelSyncTableName("ChannelSyncs"), fBoardSyncTableName("BoardSyncTime") {
  if (fServer->IsZombie()) {
    cout << "SetupNavigator: ERROR: Couldn't connect to SQLite database." << endl;
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


string SetupNavigator::GetBank(const IDs::channel& src)const{
  map<string, string>* bankDetNameMap=&TSetupData::Instance()->fBankToDetectorMap;
  for(map<string, string>::iterator it = bankDetNameMap->begin();
      it != bankDetNameMap->end(); ++it){
    if(modules::parser::iequals(it->second , src.str())) return it->first;
  }
  cout << "Invalid detector name: " << src.str() << endl;
  throw Except::InvalidDetector();
}

void SetupNavigator::CacheCalibDB() {
  // Cache all the variables we have in the database
  // First the pedestals and noises
  if (!fServer->HasTable(fPedestalNoiseTableName.c_str())) {
    cout << "SetupNavigator: ERROR: Table " << fPedestalNoiseTableName << " does not exist." << endl;
    if (!IsCalibRun()) throw Except::MissingTable(fPedestalNoiseTableName.c_str());
  } else {
    if (!ReadPedestalAndNoiseValues()) {
      cout << "SetupNavigator: ERROR: Table " << fPedestalNoiseTableName
           << " contains no calib data for this run ("<<GetRunNumber()<<")" << endl;
    if (!IsCalibRun()) throw Except::UncalibratedRun();
    }
  }
  if (!fServer->HasTable(fCoarseTimeOffsetTableName.c_str())) {
    cout << "SetupNavigator: ERROR: Table " << fCoarseTimeOffsetTableName << " does not exist." << endl;
    if(!IsCalibRun()) throw Except::MissingTable(fPedestalNoiseTableName.c_str());
  } else {
    if (!ReadCoarseTimeOffsetValues()) {
      cout << "SetupNavigator: ERROR: Table " << fCoarseTimeOffsetTableName
           << " contains no calib data for this run ("<<GetRunNumber()<<")" << endl;
    if (!IsCalibRun()) throw Except::UncalibratedRun();
    }
  }
  if (!fServer->HasTable(fEnergyCalibrationConstantsTableName.c_str())) {
    cout << "SetupNavigator: Error: Table " << fEnergyCalibrationConstantsTableName << " does not exist." << endl;
    if (!IsCalibRun()) throw Except::MissingTable(fEnergyCalibrationConstantsTableName.c_str());
  } else {
    if (!ReadEnergyCalibrationConstants()) {
      cout << "SetupNavigator: ERROR: Table " << fEnergyCalibrationConstantsTableName
           << " contains no calib data for this run ("<<GetRunNumber()<<")" << endl;
      if (!IsCalibRun()) throw Except::UncalibratedRun();
    }
  }
}

bool SetupNavigator::ReadPedestalAndNoiseValues() {
  // The values that we will read in
  double pedestal=definitions::DefaultValue;
  double noise=definitions::DefaultValue;
  string channelname;

  stringstream query;
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
  const vector<string> table = GetCoarseTimeOffsetColumns();
for(unsigned i=0;i<table.size();++i) DEBUG_VALUE(i, table[i]);
  stringstream query;
  query << "SELECT channel";
  for (unsigned int i = 0; i < table.size(); ++i)
    query << ",\"" << table[i] << '"';
  query << " FROM " << fCoarseTimeOffsetTableName << " WHERE run==" << GetRunNumber();
  TSQLResult* res = fServer->Query(query.str().c_str());

  TSQLRow* row;
  while ((row = res->Next())) {
    for (unsigned int i = 0; i < table.size(); ++i) {
      stringstream srcstr;
      srcstr << row->GetField(0) << IDs::field_separator << table[i];
      if(row->GetField(1+i))
	     fCoarseTimeOffset[IDs::source(srcstr.str())] = atof(row->GetField(1 + i));
    }
    delete row;
  }
  delete res;
  return !fCoarseTimeOffset.empty();
}

vector<string> SetupNavigator::GetCoarseTimeOffsetColumns() {
  vector<string> cols;
  stringstream cmd;
  cmd << "PRAGMA table_info(" << fCoarseTimeOffsetTableName << ")";
  TSQLResult* res = fServer->Query(cmd.str().c_str());
  TSQLRow* row;
  while ((row = res->Next())) {
    if (row->GetField(1) != string("run") && row->GetField(1) != string("channel"))
      cols.push_back(string(row->GetField(1)));
    delete row;
  }
  delete res;
  return cols;
}


bool SetupNavigator::ReadEnergyCalibrationConstants() {
  stringstream cmd;
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

bool SetupNavigator::ReadSynchronizationInfo() {
  stringstream cmd;
  cmd << "SELECT channel,block,i1,i2 FROM " << fChannelSyncTableName
      << "WHERE run==" << GetRunNumber();
  TSQLResult* res = fServer->Query(cmd.str().c_str());
  TSQLRow* row;
  while ((row = res->Next())) {
    IDs::channel ch(row->GetField(0));
    int block = atoi(row->GetField(1));
    int i1    = atoi(row->GetField(2));
    int i2    = atoi(row->GetField(3));
    if (fChanSyncs[ch].size() < block+1)
      fChanSyncs[ch].resize(block+1);
    fChanSyncs[ch][block] = std::make_pair(i1, i2);
  }
  cmd.str("");
  cmd.clear();
  cmd << "SELECT board,block,t1,t2" << fBoardSyncTableName
      << "WHERE run==" << GetRunNumber();
  res = fServer->Query(cmd.str().c_str());
  while ((row = res->Next())) {
    IDs::board brd(row->GetField(0));
    int block = atoi(row->GetField(1));
    double t1 = atof(row->GetField(2));
    double t2 = atof(row->GetField(3));
    if (fBoardSyncTime[brd].size() < block + 1)
      fBoardSyncTime[brd].resize(block + 1);
    fBoardSyncTime[brd][block] = std::make_pair(t1, t2);
  }
}

void SetupNavigator::OutputCalibCSV() {
  char fmt[128], ofname[128];
  sprintf(fmt, "calib.%05d.%%s.csv", GetRunNumber());

  sprintf(ofname, fmt, fPedestalNoiseTableName.c_str());
  std::ofstream fPN(ofname);
  fPN << "run,channel,pedestal,noise" << endl;
  for (map<IDs::channel, double>::const_iterator i = fPedestalValues.begin(); i != fPedestalValues.end(); ++i)
    fPN << GetRunNumber() << "," << i->first.str() << ","
        << i->second << "," << fNoiseValues.at(i->first) << endl;

  sprintf(ofname, fmt, fCoarseTimeOffsetTableName.c_str());
  std::ofstream fTO(ofname);
  std::set<IDs::generator> gens;
  std::set<IDs::channel> chns;
  for (map<IDs::source, double>::const_iterator i = fCoarseTimeOffset.begin(); i != fCoarseTimeOffset.end(); ++i) {
    gens.insert(i->first.Generator());
    chns.insert(i->first.Channel());
  }
  fTO << "run,channel";
  for (std::set<IDs::generator>::const_iterator i = gens.begin(); i != gens.end(); ++i)
    fTO << "," << i->str() << "," << i->str() << "_sigma";
  fTO << endl;
  for (std::set<IDs::channel>::const_iterator i = chns.begin(); i != chns.end(); ++i) {
    fTO << GetRunNumber() << "," << i->str();
    for (std::set<IDs::generator>::const_iterator j = gens.begin(); j != gens.end(); ++j) {
      fTO << ",";
      if (fCoarseTimeOffset.count(IDs::source(*i, *j)))
        fTO << fCoarseTimeOffset.at(IDs::source(*i, *j));
      fTO << ",";
      if (fCoarseTimeOffsetSigma.count(IDs::source(*i, *j)))
       fTO << fCoarseTimeOffsetSigma.at(IDs::source(*i, *j));
    }
    fTO << endl;
  }

  sprintf(ofname, fmt, fChannelSyncTableName.c_str());
  std::ofstream fCS(ofname);
  fCS << "run,channel,block,i1,i2" << endl;
  for (map< IDs::channel, vector< pair<int, int> > >::const_iterator i = fChanSyncs.begin();
       i != fChanSyncs.end(); ++i)
    for (int j = 0; j < i->second.size(); ++j)
      fCS << GetRunNumber() << ", " << i->first << ", " << j << ", "
          << i->second[j].first << ", " << i->second[j].second << endl;
  sprintf(ofname, fmt, fBoardSyncTableName.c_str());
  std::ofstream fBS(ofname);
  fBS.precision(std::numeric_limits<double>::digits10+2);
  fBS << "run,board,block,t1,t2" << endl;
  for (map< IDs::board, vector< pair<double, double> > >::const_iterator i = fBoardSyncTime.begin();
       i != fBoardSyncTime.end(); ++i)
    for (int j = 0; j < i->second.size(); ++j)
      fBS << GetRunNumber() << ", " << i->first << ", " << j << ", "
          << i->second[j].first << ", " << i->second[j].second << endl;
}

void SetupNavigator::SetPedestalAndNoise(const IDs::channel& chn, double ped, double nse) {
  if(!IsCalibRun()) {
    cout << "SetupNavigator: Warning: Request to edit pedestals and "
            "noises when not flagged as calibration. Not setting." << endl;
    return;
  }
  fPedestalValues[chn] = ped;
  fNoiseValues[chn] = nse;
}

void SetupNavigator::SetCoarseTimeOffset(const IDs::source& src, double mean, double sigma) {
  if(!IsCalibRun()) {
    cout << "SetupNavigator: Warning: Request to edit coarse time offsets "
            "when not flagged as calibration. Not setting." << endl;
    return;
  }
  // The excess code here is to strip {no_time_shift=*} from the generator config string
  fCoarseTimeOffset[IDs::source(src.Channel(), IDs::generator(StripTimeShiftConfigFromString(src.Generator().str())))] = mean;
  fCoarseTimeOffsetSigma[IDs::source(src.Channel(), IDs::generator(StripTimeShiftConfigFromString(src.Generator().str())))] = sigma;
}

void SetupNavigator::SetChanSyncs(const IDs::channel& ch, int b,
                                  pair<int, int> is) {
  if(!IsCalibRun()) {
    cout << "SetupNavigator: Warning: Request to edit chan syncs "
            "when not flagged as calibration." << endl;
    assert(false);
  }
  if (fChanSyncs[ch].size() < b+1)
    fChanSyncs[ch].resize(b+1);
  fChanSyncs[ch][b] = is;
}

void SetupNavigator::SetBoardSync(const IDs::board& brd, int b, std::pair<double, double> ts) {
  if(!IsCalibRun()) {
    cout << "SetupNavigator: Warning: Request to edit board syncs "
            "when not flagged as calibration." << endl;
    assert(false);
  }
  if (fBoardSyncTime[brd].size() < b+1)
    fBoardSyncTime[brd].resize(b+1);
  fBoardSyncTime[brd][b] = ts;
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
 string conf=source.Generator().Config();
 unsigned curly_br=conf.find('}');
if(curly_br!=std::string::npos){ source.Generator().Config(conf.substr(0,curly_br+1));}

 return source.matches(IDs::channel("SiT-1-S")) ? 0. : alcap::at<Except::InvalidDetector>(fCoarseTimeOffset,source,source.str().c_str());
}

double SetupNavigator::GetSyncOffset(const channel& ch, int blk) const {
  static board brd0 = board(channel("SiT-1-S"));
  board brd(ch);
  pair<double, double> t0s = fBoardSyncTime.at(brd0).at(blk);
  pair<double, double> ts  = fBoardSyncTime.at(brd) .at(blk);
  return ts.first - t0s.first;
}

pair<int, int> SetupNavigator::GetChanSyncs(const IDs::channel& ch, int blk) const {
  if      (!fChanSyncs.count(ch)) return std::make_pair(-1, -1);
  else if (blk <= 0)              return fChanSyncs.find(ch)->second.back();
  else                            return fChanSyncs.find(ch)->second.at(blk);
}
