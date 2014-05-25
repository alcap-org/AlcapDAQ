/**************************************
Class: WireMap
Author: John R. Quirk

The WireMap contains some rough information from the ODB wiremap.
Specifically the run, banks/detectors, pedestals, polarities,
thresholds, and offsets.

CURRENTLY THRESHOLDS ARE NOT IMPLEMENTS
The reason for this is that the thresholds aren't stored in
the ODB wiremap.
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
  std::vector<int> fPedestal;
  std::vector<int> fPolarity;
  std::vector<int> fThreshold; /*** NOT IMPLEMENTED YET ***/
  std::vector<int> fOffset;

public:
  WireMap();
  WireMap(int run, std::string& odb_file);

public:
  // Getters
  unsigned int GetRun() const;
  unsigned int GetNDets() const;
  std::vector<std::string>& GetBanks();
  std::vector<std::string>& GetDets();
  std::vector<int>& GetPedestals();
  std::vector<int>& GetPolarities();
  std::vector<int>& GetThresholds();
  std::vector<int>& GetOffsets();

  // Add new value
  void Add(const char bankname[], const char detname[], int ped, int pol, int thresh, int off);
  void Add(std::string& bankname, std::string& detname, int ped, int pol, int thresh, int off);
  // Copy value from another WireMap 
  void Add(WireMap& wm, int index);
  // Add individual elements
  void AddBank(const std::string&); // Increments fNDets; other do not
  void AddDet(const std::string&);
  void AddPedestal(int);
  void AddPolarity(int);
  void AddThreshold(int); /*** THRESHOLDS NOT IMPLEMENTED ***/
  void AddOffset(int);

  // Load the ODB values
  void Load(int run, const std::string& odb_file);
  // Overwrite any values already here with those
  // from another WireMap
  void LoadOver(WireMap&);

  // Clear
  void Clear();

  // Resize all vectors to size of banks
  // fNDets is set to this size
  void ResizeToBanks();

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
};

#endif
