#ifndef SETUPRECORD_H
#define SETUPRECORD_H

#include <IdChannel.h>

class TSetupData;
//EventNavigator;

/// This provides access to information stored in TSetupData.  Any
/// request for data on a non-existant channel results in an exception
/// (unknown_channel_error) being thrown.
class SetupRecord {
public:
  friend class EventNavigator;
  typedef IDs::channel ChannelID;
  
  ///These are the channel characteristics recored in TSetupData. 
  struct BankInfo_t {
    /// The length, in nanoseconds, of each sample clock tick. [0~]
    Double_t TickLength;      
    /// @brief Depricated. [8~]
    /// @detail The input width of the LSB i.e. gradient of linear calibration
    Double_t LinearWidthLSB;
    /// @brief Deprecated. [16~]
    /// @detail The input value at 0 counts i.e. offset of linear calibration
    Double_t LinearZeroPoint;
    /// The timing offset from the muSc. [24~]
    Double_t TimeFromMuSc;
    /// The ID of the channel [32~](?)
    ChannelID ID;
    /// The bit depth of the ADC [40~]
    Short_t ADCDepth;
    /// The pedestal of the channel [42~]
    Short_t Pedestal;
    /// The polarity of the channel [44~]
    Short_t Polarity;
    /// The name of the MIDAS Bank. (4 ASCII char + '\0') [46~]
    char Name[5]; 
    /// Whether the channel was enabled [51~52]
    Char_t Enabled;

    ///Prints a summary of everything stored, primarily for debugging.
    void Print() const;
  };
  
  /// Destructor. Default is correct.
  virtual ~SetupRecord();

  /// The length, in nanoseconds, of each sample clock tick.
  inline Double_t GetTickLength(const ChannelID& cid) const
  { return BankInfo(cid).TickLength; }

  /// The timing offset from the muSc.
  inline Double_t GetTimeFromMuSc(const ChannelID& cid) const
  { return BankInfo(cid).TimeFromMuSc; }

  /// The bit depth of the ADC
  inline Short_t GetADCDepth(const ChannelID& cid) const
  { return BankInfo(cid).ADCDepth; }

  /// The overflow value of the ADC, i.e 2^{bit depth} the first impossible value
  inline Int_t GetOverflowADC(const ChannelID& cid) const
  { return (1 << GetADCDepth(cid)); }

  /// The maximum value recordable by the ADC i.e. 1 less than the overflow 
  inline Int_t GetMaxADC(const ChannelID& cid) const
  { return GetOverflowADC(cid) -1; }

  /// The pedestal of the channel
  inline Short_t GetPedestal(const ChannelID& cid) const
  { return BankInfo(cid).Pedestal; }

  /// @brief The headroom, defined as MaxADC - Pedestal
  /// @detail Note that negative pulses should typically have |headroom| < |pedestal|,
  /// in the same way that positive pulses typically have |pedestal| < |headroom|
  inline Short_t GetHeadroom(const ChannelID& cid) const 
  { return GetMaxADC(cid) - GetPedestal(cid); }

  /// The polarity of the channel
  inline Short_t GetPolarity(const ChannelID& cid) const
  { return BankInfo(cid).Polarity; }

  /// Is the enabled bit set for this channel?
  inline Bool_t IsEnabled(const ChannelID& cid) const
  { return static_cast<Bool_t>(BankInfo(cid).Enabled); }

  /// Get the MIDAS bank name for this channelID 
  inline std::string GetBankName(const ChannelID& cid) const
  { return BankFromChannel(cid); }

  /// Get the MIDAS bank name for this channelID 
  std::string BankFromChannel(const ChannelID& cid) const
  { return std::string(BankInfo(cid).Name); }

  /// Convert a MIDAS bankname to a Channel ID.
  ChannelID ChannelFromBank(const std::string& bankname) const
  { return BankInfo(bankname).ID; }

protected:
  /// Get the BankInfo for this bank name
  const BankInfo_t& BankInfo(const std::string& bankname) const;

  /// Get the BankInfo for this Channel ID
  const BankInfo_t& BankInfo(const ChannelID& cid) const;

  /// Get the Writable BankInfo for this bank name
  BankInfo_t& BankInfo(const std::string& bankname);

  /// Get the Writable BankInfo for this Channel ID
  BankInfo_t& BankInfo(const ChannelID& cid);

  /// Normal constructor, used by friends
  SetupRecord(const TSetupData* setup_data);

  /// Default constructor, shouldn't need this
  SetupRecord();

  /// Copy constructor, shouldn't need this
  SetupRecord(const SetupRecord& src);

  /// Copy assignment, shouldn't need this
  SetupRecord& operator=(const SetupRecord& rhs);

  /// Construction helper
  int InitChannels(const TSetupData* setup_data);

  /// Construction helper
  int InitMetaData(const TSetupData* setup_data);

private:
  /// A lookup for channel Info, with arbitrary ordering;
  std::vector<BankInfo_t> fInfoLookup;

  /// A lookup for the record if we know the Channel ID.
  std::map<ChannelID, UInt_t> fIDOrderLookup; 

  /// A lookup for the record if we know the MIDAS Bankname.
  std::map<std::string, UInt_t> fNameOrderLookup; 
};

#endif //SETUPRECORD_H
