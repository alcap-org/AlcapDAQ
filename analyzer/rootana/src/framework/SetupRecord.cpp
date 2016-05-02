#include "SetupRecord.h"

//C++/STL
#include <stdexcept>
#include <iostream>

//ROOT

//Local
#include "TSetupData.h"


///This shoud get a proper home and some more flexability
class unknown_channel_error : public std::out_of_range {
public:
  unknown_channel_error()
    : std::out_of_range("Unknown Channel")
  {}
};


//======================================================================
SetupRecord::SetupRecord(const TSetupData* setup_data)
  : fInfoLookup(0)
{
  InitChannels(setup_data);
  InitMetaData(setup_data);

  //for (int i =0; i < fInfoLookup.size(); ++i) {
  //  std::cout << "\n\n";
  //  fInfoLookup[i].Print();
  //  }
}


//----------------------------------------------------------------------
SetupRecord::~SetupRecord()
{}


//----------------------------------------------------------------------
int SetupRecord::InitChannels(const TSetupData* setup_data)
{
  typedef const std::map<std::string, std::string> StrStrMap;
  typedef StrStrMap::const_iterator StrStrIt;


  UInt_t lut_size = fInfoLookup.size();
  StrStrMap& b2d = setup_data->fBankToDetectorMap;
  fInfoLookup.resize(b2d.size());
  for (StrStrIt it = b2d.begin() ; it != b2d.end(); ++it){
    const std::string& name = it->first;
    //    if (name.size() > 4) throw std::out_of_range("Bank name too long!");
    ChannelID cid(it->second);
    fNameOrderLookup.insert(std::make_pair(name,lut_size));
    fIDOrderLookup.insert(std::make_pair(cid,lut_size));

    BankInfo_t& info = fInfoLookup.at(lut_size);
    name.copy(info.Name, 4);     info.Name[4] = '\0';
    info.ID = cid;
    ++lut_size;
  }
  return lut_size;
}

//----------------------------------------------------------------------
int SetupRecord::InitMetaData(const TSetupData* setup_data)
{
  typedef const std::map<std::string, int> StrIntMap;
  typedef StrIntMap::const_iterator StrIntIt;

  UInt_t lut_size = fInfoLookup.size();
  const StrIntMap& bit_map = setup_data->fBankToBitMap;
  if (bit_map.size() > lut_size){
    BankInfo_t blank;
    blank.ID = ChannelID(IDs::kErrorDetector, 
                         IDs::kErrorSlowFast);
    fInfoLookup.resize(bit_map.size(), blank);
  }
  
  //Assume all of these maps have the same length.  If not they wil
  //record default values, and it probably does not matter
  for (StrIntIt it = bit_map.begin(); it != bit_map.end(); ++it){
    std::string name = it->first;
    if (fNameOrderLookup.count(name) == 0){
      fNameOrderLookup.insert(make_pair(name, lut_size));
      ++lut_size;
    }  
    
    BankInfo_t& info = BankInfo(name);
    name.copy(info.Name, 4);    info.Name[4] = '\0';
    info.TickLength = setup_data->GetClockTick(name);
    info.LinearWidthLSB = setup_data->GetADCSlopeCalib(name);
    info.LinearZeroPoint = setup_data->GetADCOffsetCalib(name);
    info.TimeFromMuSc = setup_data->GetTimeShift(name);
    info.ADCDepth = setup_data->GetNBits(name);
    info.Pedestal = setup_data->GetPedestal(name);
    info.Polarity = setup_data->GetTriggerPolarity(name);
    info.Enabled = setup_data->GetEnableBit(name);
  }
  return lut_size;
}


//----------------------------------------------------------------------
const SetupRecord::BankInfo_t& 
SetupRecord::BankInfo(const std::string& bankname) const
{
  std::map<std::string, UInt_t>::const_iterator it;
  it = fNameOrderLookup.find(bankname);
  if (it == fNameOrderLookup.end() ) throw unknown_channel_error();
  return fInfoLookup.at(it->second);
}


//----------------------------------------------------------------------
const SetupRecord::BankInfo_t& 
SetupRecord::BankInfo(const ChannelID& cid) const
{
  std::map<ChannelID, unsigned int>::const_iterator it;
  it = fIDOrderLookup.find(cid);
  if (it == fIDOrderLookup.end()) throw unknown_channel_error();
  return fInfoLookup.at(it->second);
}


//----------------------------------------------------------------------
SetupRecord::BankInfo_t& 
SetupRecord::BankInfo(const std::string& bankname) 
{
  const SetupRecord* c_this = this;
  return const_cast<BankInfo_t&>(c_this->BankInfo(bankname));
}


//----------------------------------------------------------------------
SetupRecord::BankInfo_t& 
SetupRecord::BankInfo(const ChannelID& cid)
{
  const SetupRecord* c_this = this;
  return const_cast<BankInfo_t&>(c_this->BankInfo(cid));
}


//======================================================================
void SetupRecord::BankInfo_t::Print() const
{
  std::cout << ID << " at bank " << Name 
            << (Enabled>0 ? "   (enabled)" : "  (disabled)")
            << "\n" << "- - - - - - - - - - - - - - - - - - - - -"
            << "\n" << "  Tick:         " << TickLength
            << "\n" << "  Time offset:  " << TimeFromMuSc
            << "\n" << "  ADC depth:    " << ADCDepth 
            << " [Max: " << (1<<ADCDepth) << "]"
            << "\n" << "  Pedestal:     " << Pedestal
            << "\n" << "  Polarity:     " << (Polarity>0 ? "+" : "-")
    //        << "\n" << "  (Calibration: " << LinearWidthLSB 
    //        << " * C + " << LinearZeroPoint << ")"
            << std::endl;
}
