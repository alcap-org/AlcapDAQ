/**************************************
Class: WireMap
Author: John R. Quirk

The WireMap contains some rough information from the ODB wiremap.
Specifically the run, banks/detectors/status, pedestals,
polarities, and offsets.
**************************************/

#ifndef WIREMAP_H__
#define WIREMAP_H__

#include <vector>
#include <string>

class WireMap {
private:
  unsigned int fRun;
  unsigned int fNDets;
  std::vector<std::string> fBankName;
  std::vector<std::string> fDetName;
  std::vector<bool> fEnabled;
  std::vector<int> fPedestal;
  std::vector<int> fPolarity;
  std::vector<int> fOffset;

public:
  enum key_t { BANK, DETECTOR, ENABLED,
	       PEDESTAL, POLARITY, TIMESHIFT,
	       UNKNOWN };

public:
  WireMap();
  WireMap(int run, std::string& odb_file);

public:
  // Setters
  void SetRun(unsigned int);
  void Enable(); // Enable last channel added
  bool Enable(unsigned int); // Return true if succesful
  void Disable(); // Disable last channel added
  bool Disable(unsigned int); // Return true if succesful
  // Getters
  unsigned int GetRun() const;
  unsigned int GetNDets() const;
  std::vector<std::string>& GetBanks();
  std::vector<std::string>& GetDets();
  std::vector<bool>& GetEnableds();
  std::vector<int>& GetPedestals();
  std::vector<int>& GetPolarities();
  std::vector<int>& GetOffsets();

  // Add new value
  void Add(const char bankname[], const char detname[], bool en, int ped, int pol, int off);
  void Add(std::string& bankname, std::string& detname, bool en, int ped, int pol, int off);
  void Add(WireMap&, int index);

private:
  // Add individual elements
  void AddBank(const std::string&); // Increments fNDets; others do not
  void AddDet(const std::string&);
  void AddEnabled(bool);
  void AddPedestal(int);
  void AddPolarity(int);
  void AddOffset(int);

public:
  // Load the ODB values, or load over with
  // another WireMap in the same way that
  // ODBEdit does
  void Load(int run, const std::string& odb_file);
  void LoadOver(WireMap&);
  void Clear();

  // Resize all vectors to size of banks by trimming
  // and/or bloating. This should never be necessary.
  void ResizeToBanks();
  // Check for duplicates that are both enabled
  // and remove the duplicates that are disabled
  bool AreThereDuplicates();
  void ClearDisabledDuplicateDetectors();

  // Return default WireMap
  // This is what the first run of alcapana saw as a default
  // Got from loading the ODB files in analyzer/odb/ in
  // alphabeticla order
  static WireMap Default();

private:
  // Checks extension for 'odb'
  // Any other format returns false.
  static bool IsODBFile(const std::string& fname);
  // Determines the number of elements in an ODB key's
  // array; assumes the line is of the form
  // VARIABLE = TYPE [##] :
  // This method return ##
  // It must be passed only the part of the line
  // (including single leading space)
  //  = TYPE [##] :
  static int GetArraySize(const char (&tmp)[256]);
  // Get the key
  static key_t GetKey(std::string&);

public:
  void Print();
};

#endif
