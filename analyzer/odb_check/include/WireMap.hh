/** \addtogroup odbcheck
 * @{
 */
#ifndef WIREMAP_H__
#define WIREMAP_H__

#include <vector>
#include <string>

////////////////////////////////////////////////////////////
/// \brief C++ Class containing certain ODB keys in a usable form.
///
/// The WireMap contains some information from the ODB wiremap.
/// Specifically the run, banks names, detector names,
/// enabled/disabled status, pedestals, polarities, and
/// offsets. Can be given the path to an ODB file to construct
/// from or built up with the Add method.
////////////////////////////////////////////////////////////

class WireMap {
private:
  /// WireMap corresponding to default ODB.
  ///
  /// Constructed if needed.
  static WireMap* DefaultODB;

  /// Run Number
  unsigned int fRun; ///< Run number
  /// Number of detectors
  unsigned int fNDets;
  /// Name of each channel.
  std::vector<std::string> fBankName;
  /// Detector attached to each channel.
  std::vector<std::string> fDetName;
  /// Enabled status for each channel.
  std::vector<bool> fEnabled;
  /// Pedestals for each channel.
  std::vector<int> fPedestal;
  /// Polarity of each channel.
  std::vector<int> fPolarity;
  /// Timing offset (TimeShift) for each channel.
  std::vector<int> fOffset;
  /// Sampling frequency of each channel.
  std::vector<double> fFrequency;

public:
  /**
   * Each member corresponds to an ODB key used in WireMap.
   * The method GetKey takes a string and if it matches
   * an ODB key used by WireMap, returns the corresponding
   * one (or UNKNOWN if not found.
   */
  enum key_t { BANK, DETECTOR, ENABLED,
	       PEDESTAL, POLARITY, TIMESHIFT,
	       FREQUENCY, UNKNOWN };

public:
  /// This constructs an empty %WireMap.
  WireMap();
  /// Construct a %WireMap from an ODB file.
  ///
  /// @param[in] run The run number needs to be specified since,
  /// as of yet, it isn't got from the ODB file.
  /// @param[in] odb_file Path to the ODB files to use to construct a %WireMap.
  WireMap(int run, std::string& odb_file);

public:
  /// \name Setters
  //@{
  void SetRun(unsigned int); ///< Set the run number
  void Enable(); ///< Enable the last channel added.
  bool Enable(unsigned int); ///< Enable a channel. @return True if succesful.
  void Disable(); ///< Disable last channel added.
  bool Disable(unsigned int); ///< Disable a channel. @return True if succesful.
  //@}
  /// \name Getters
  //@{
  /// @return Run number.
  unsigned int GetRun() const;
  /// @return Number of detectors.
  unsigned int GetNDets() const;
  /// @return Reference to vector of bank names.
  std::vector<std::string>& GetBanks();
  /// @return Reference to vector of detector names.
  std::vector<std::string>& GetDets();
  /// @return Reference to vector of channel's enabled status.
  std::vector<bool>& GetEnableds();
  /// @return Reference to vector of pedestals.
  std::vector<int>& GetPedestals();
  /// @return Reference to vector of polarities (+/- 1).
  std::vector<int>& GetPolarities();
  /// @return Reference to vector of timing offsets (TimeShift).
  std::vector<int>& GetOffsets();
  /// @return Reference to vector of sampling frequencies.
  std::vector<double>& GetFrequencies();
  //@}

  /// \name Adders
  //@{
  /// \brief Add new value to %WireMap.
  ///
  /// @param[in] bankname Channel name
  /// @param[in] detname Detector name
  /// @param[in] en Enabled status
  /// @param[in] ped Pedestal
  /// @param[in] pol Polarity
  /// @param[in] off Timing offset (TimeShift)
  /// @param[in] freq Sampling frequency
  void Add(const char bankname[], const char detname[], bool en, int ped, int pol, int off, double freq);
  /// Add new value to %WireMap. See Add(const char[], const char[], bool, int, int, int, double).
  void Add(std::string& bankname, std::string& detname, bool en, int ped, int pol, int off, double freq);
  /// Add new value to %WireMap from another %WireMap
  ///
  /// @param[in] wm %WireMap that has element we wish to copy to this %WireMap.
  /// @param[in] index Which element of this %WireMap to copy.
  void Add(WireMap& wm, int index);
  //@}

  /// Unique fixes for certain runs
  ///
  /// 1. For runs 2091-2103, correct the sampling frequency
  void UniqueFixes();

public:
  /// \name Loaders
  //@{
  /// Load ODB values into %WireMap from an ODB file.
  ///
  /// Parses an ODB file, finds keys indicating what the %WireMap
  /// can hold, and loads those.
  ///
  /// @param[in] run Run number.
  /// Needs to be included here because it's not parsed from the ODB file yet.
  /// @param[in] odb_file Path to ODB file to load.
  void Load(int run, const std::string& odb_file);
  /// Load another %WireMap over this one in the same manner ODB Edit loads one ODb over another.
  void LoadOver(WireMap&);
  //@}
  /// Reset this %WireMap to a state similar to what it would be if called with the default constructor.
  void Clear();

  /// Resize all vectors to size of banks.
  ///
  /// It's possible that upond loading an ODB file not all
  /// of the data vectors (bank name, detector names, enabled status, ...)
  /// are equal in size. This poses a problem because the index
  /// of these vectors are what correspond the data to one another.
  ///
  /// This resizes all vectors to the same size as the bank name vector
  /// by trimming them down or bloating them up. The trimming is done by
  /// removing the last element of the data vectors until they're
  /// short enough, or by appending default values until long enough.
  /// Ideally, this would never be necessary; however it was
  /// needed during development of odb_check.cc and is being kept in case
  /// it is needed again.
  void ResizeToBanks();
  /// Check for duplicate detectors that are both enabled.
  ///
  /// @return True if there exist two detectors with the
  /// same name that are both enabled.
  bool AreThereDuplicates();
  /// Removes duplicate detectors as long as only one is enabled.
  ///
  /// If there are multiple detectors with the same name,
  /// this sets the name of all of them to "blank" as long as
  /// only a single one of the duplicates is enabled.
  void ClearDisabledDuplicateDetectors();

  /// Return default WireMap
  ///
  /// The batch scripts cause a number of ODB files to be loaded
  /// in analyzer/odb/ that can confuse issues. The WireMap
  /// that would result from loading all of these defaults has
  /// been hardcoded here.
  ///
  /// @return The default WireMap.
  static WireMap Default();

private:
  /// Checks extension for 'odb'
  ///
  /// This is called internally before trying to load an ODB file.
  /// @param[in] fname Path to ODB file.
  /// @return True if the extension indicates the path points to an ODB file..
  static bool IsODBFile(const std::string& fname);
  /// Determines the number of elements in an ODB key's array
  ///
  /// The ODB records how many elements are in an key that is an array.
  /// The key declaration in the ODB file is of the form
  ///
  /// VARIABLE = TYPE [##] :
  ///
  /// @param tmp The part of the key declaration of the form " = TYPE [##] :"
  /// @return The number of elemented in the array.
  static int GetArraySize(const char (&tmp)[256]);
  /// Get the ODB key from its declaration in the ODB file.
  ///
  /// Keys in an ODB file are declared akin to
  ///
  /// KEYNAME = KEYTYPE
  ///
  /// @param[in] key The string in the ODB file that declares the key. Just the stirng KEYNAME
  /// @return The key type.
  static key_t GetKey(std::string& key);

public:
  /// Print for debugging.
  void Print();
};

#endif
/** @}*/
