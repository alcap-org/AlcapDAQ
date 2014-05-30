#include "WireMap.hh"

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>

WireMap::WireMap() {
}

WireMap::WireMap(int run, std::string& odb_file) {
  Load(run, odb_file);
}

void WireMap::SetRun(unsigned int run) {
  if (run > 0 && run < 100000)
    fRun = run;
  else
    std::cout <<
      "WireMap WARNING: Run outside range! (" << run << ")" <<
      std::endl;
}

void Enable() {
  fEnabled.back() = true;
}

bool Enable(unsigned int i) {
  if (i < fEnabled.size())
    return (fEnabled[i] = true);
  return false;
}

void Disable() {
  fEnabled.back() = false;
}

bool Disable(unsigned int i) {
  if (i < fEnabled.size())
    return (!fEnabled[i] = false);
  return false;
}

void WireMap::Add(const char bankname[], const char detname[], bool en, int ped, int pol, int off) {
  std::string bn(bankname);
  std::string dn(detname);
  Add(bn, dn, en, ped, pol, off);
}
 
void WireMap::Add(std::string& bankname, std::string& detname, bool en, int ped, int pol, int off) {
  ++fNDets;
  fBankName.push_back(bankname);
  fDetName.push_back(detname);
  fEnabled.push_back(en);
  fPedestal.push_back(ped);
  fPolarity.push_back(pol);
  fOffset.push_back(off);
}

void WireMap::Add(WireMap& wm, int i) {
  ++fNDets;
  fBankName.push_back(wm.GetBanks()[i]);
  fDetName.push_back(wm.GetDets()[i]);
  fEnabled.push_back(wm.GetEnableds()[i]);
  fPedestal.push_back(wm.GetPedestals()[i]);
  fPolarity.push_back(wm.GetPolarities()[i]);
  fOffset.push_back(wm.GetOffsets()[i]);
}

void WireMap::AddBank(const std::string& bank) {
  ++fNDets;
  fBankName.push_back(bank);
}

void WireMap::AddDet(const std::string& det) {
  fDetName.push_back(det);
}

void WireMap::AddEnabled(bool en) {
  fEnabled.push_back(en);
}

void WireMap::AddPedestal(int ped) {
  fPedestal.push_back(ped);
}

void WireMap::AddPolarity(int pol) {
  fPolarity.push_back(pol);
}

void WireMap::AddOffset(int off) {
  fOffset.push_back(off);
}

void WireMap::Load(int run_number, const std::string& odb_file) {
  static const std::string header("[/Analyzer/WireMap]");
  static char tmp[256];

  if (!IsODBFile(odb_file)) {
    std::cout <<
      "WireMap ERROR: Not recognized as ODB file input! Must be ODB file." <<
      std::endl;
    return;
  }

  Clear();
  fRun = run_number;
  
  // Look through ODB file for appropriate banks
  std::ifstream f(odb_file.c_str());
  std::string str;
  key_t key;
  int x;
  char c;
  while (f.good()) {
    f >> str;
    if (str == header) {
      while (f.good()) {
	f >> str;
	key = GetKey(str);
	f.getline(tmp, 256);
	unsigned int n = 0; // Array size
	if (key == BANK) {
	  n = GetArraySize(tmp);
	  fNDets = n; // Set number of detectors equal to number of banks
	  for (unsigned int i = 0; i < n; ++i) {
	    f >> str;
	    if (f.read(tmp, 1).peek() == '\n') // Strings can be null, cause streaming problems
	      str = "";
	    else
	      f >> str;
	    fBankName.push_back(str);
	  }
	} else if (key == DETECTOR) {
	  n = GetArraySize(tmp);
	  for (unsigned int i = 0; i < n; ++i) {
	    f >> str;
	    if (f.read(tmp, 1).peek() == '\n') // Strings can be null, cause streaming problems
	      str = "";
	    else
	      f >> str;
	    fDetName.push_back(str);
	  }
	} else if (key == ENABLED) {
	  n = GetArraySize(tmp);
	  for (unsigned int i =0; i < n; ++i) {
	    f >> str >> c;
	    if (c == 'y') fEnabled.push_back(true);
	    else if (c == 'n') fEnabled.push_back(false);
	    else std::cout << "WireMap ERROR: '" << c <<
		   "' is not an ODB boolean!" << std::endl;
	  }
	} else if (key == PEDESTAL) {
	  n = GetArraySize(tmp);
	  for (unsigned int i = 0; i < n; ++i) {
	    f >> str >> x;
	    fPedestal.push_back(x);
	  }
	} else if (key == POLARITY) {
       	  n = GetArraySize(tmp);
	  for (unsigned int i = 0; i < n; ++i) {
	    f >> str >> x;
	    fPolarity.push_back(x);
	  }
	} else if (key == TIMESHIFT) {
	  n = GetArraySize(tmp);
	  for (unsigned int i = 0; i < n; ++i) {
	    f >> str >> x;
	    fOffset.push_back(x);
	  }
	}
	// If for some reason haven't got the number of detectors
	// from number of banks, then grab the first non-zero
	// array sized key from ODB
	if (fNDets == 0)
	  fNDets = n;
	// If everything is the right size
	// That means everything is finished
	if (fNDets != 0 &&
	    fBankName.size() == fNDets &&
	    fDetName.size() == fNDets &&
	    fPedestal.size() == fNDets &&
	    fPolarity.size() == fNDets &&
	    fOffset.size() == fNDets)
	  return;
      }
    }
  }
  // If we get to this point, vectors are not all equal in size
  std::cout <<
    "WireMap WARNING: Didn't find everything in the ODB! (" <<
    odb_file << ")" << std::endl;

}

void WireMap::LoadOver(WireMap& wm) {
  // Load the new WireMap variables if they exist
  // This seems to be how the ODB operates
  fRun = wm.GetRun();
  fNDets = wm.GetNDets();
  if (wm.GetBanks().size() > 0)
    fBankName = wm.GetBanks();
  if (wm.GetDets().size() > 0)
    fDetName = wm.GetDets();
  if (wm.GetPedestals().size() > 0)
    fPedestal = wm.GetPedestals();
  if (wm.GetPolarities().size() > 0)
    fPolarity = wm.GetPolarities();
  if (wm.GetOffsets().size() > 0)
    fOffset = wm.GetOffsets();
}

void WireMap::Clear() {
  fRun = 0;
  fNDets = 0;
  fBankName.clear();
  fDetName.clear();
  fPedestal.clear();
  fPolarity.clear();
  fOffset.clear();
}

void WireMap::ResizeToBanks() {
  static const std::string default_det("blank");
  static const int default_ped = 0;
  static const int default_pol = 1;
  static const int default_off = 0;

  fNDets = fBankName.size();
  // Trim down
  while (fDetName.size() > fNDets)
    fDetName.pop_back();
  while (fPedestal.size() > fNDets)
    fPedestal.pop_back();
  while (fPolarity.size() > fNDets)
    fPedestal.pop_back();;
  while (fOffset.size() > fNDets)
    fOffset.pop_back();
  // Bulk up
  while (fDetName.size() < fNDets)
    fDetName.push_back(default_det);
  while (fPedestal.size() < fNDets)
    fPedestal.push_back(default_ped);
  while (fPolarity.size() < fNDets)
    fPolarity.push_back(default_pol);
  while (fOffset.size() < fNDets)
    fOffset.push_back(default_off);
}

bool WireMap::IsODBFile(const std::string& fname) {
  return ".odb" == fname.substr(fname.size() - 4, std::string::npos);
}

int WireMap::GetArraySize(const char (&tmp)[256]) {
  // Get # of detectors from line in ODB of the form
  // VARIBALE = TYPE[#] :
  std::string str(tmp);
  std::stringstream ss;
  int n;
  int pos[2];

  pos[0] = str.find('[') + 1;
  pos[1] = str.find(']');
  str = str.substr(pos[0], pos[1] - pos[0]);
  ss << str;
  ss >> n;

  return n;
}

unsigned int WireMap::GetRun() const {
  return fRun;
}

unsigned int WireMap::GetNDets() const {
  return fNDets;
}

std::vector<std::string>& WireMap::GetBanks() {
  return fBankName;
}

std::vector<std::string>& WireMap::GetDets() {
  return fDetName;
}

std::vector<bool>& WireMap::GetEnableds() {
  return fEnabled;
}

std::vector<int>& WireMap::GetPedestals() {
  return fPedestal;
}

std::vector<int>& WireMap::GetPolarities() {
  return fPolarity;
}

std::vector<int>& WireMap::GetOffsets() {
  return fOffset;
}

WireMap::key_t WireMap::GetKey(std::string& key) {
  static const std::string bankname_key("BankName");
  static const std::string detname_key("DetectorName");
  static const std::string enabled_key("Enabled");
  static const std::string polarity_key("TriggerPolarity");
  static const std::string pedestal_key("Pedestal");
  static const std::string offset_key("TimeShift");
  if (key == bankname_key)
    return BANK;
  else if (key == detname_key)
    return DETECTOR;
  else if (key == enabled_key)
    return ENABLED;
  else if (key == polarity_key)
    return POLARITY;
  else if (key == pedestal_key)
    return PEDESTAL;
  else if (key == offset_key)
    return TIMESHIFT;
  return UNKNOWN;
}

WireMap WireMap::Default() {
  WireMap wm;
  wm.Add("Na80", "SiL2-S", true, 2745, -1, 4000);
  wm.Add("Nb80", "SiR2-S", true, 2730, -1, 4000);
  wm.Add("Nc80", "blank", true, 1000, -1, 0);
  wm.Add("Nd80", "blank", false, 1, -1, 0);
  wm.Add("Ne80", "blank", false, 1, -1, 0);
  wm.Add("Nf80", "blank", false, 1, -1, 0);
  wm.Add("Ng80", "blank", false, 1, -1, 0);
  wm.Add("Nh80", "blank", false, 1, -1, 0);

  wm.Add("Na81", "SiL2-F", true, 1415, 1, 407);
  wm.Add("Nb81", "SiR2-F", true, 1210, 1, 444);
  wm.Add("Nc81", "SiL1-1-F", true, 1292, 1, 146.7);
  wm.Add("Nd81", "SiL1-2-F", true, 1300, 1, 150);
  wm.Add("Ne81", "SiL1-3-F", true, 1280, 1, 149);
  wm.Add("Nf81", "SiL1-4-F", true, 1290, 1, 149.4);
  wm.Add("Ng81", "SiR1-sum-F", true, 1, 1, 0);
  wm.Add("Nh81", "Ge-F", true, 1390, 1, 81.1);

  wm.Add("Na82", "SiL1-1-S", true, 2760, -1, 2431);
  wm.Add("Nb82", "SiL1-2-S", true, 2820, -1, 2427);
  wm.Add("Nc82", "SiL1-3-S", true, 2740, -1, 2431);
  wm.Add("Nd82", "SiL1-4-S", true, 2740, -1, 2429);
  wm.Add("Ne82", "SiR1-1-S", true, 2750, -1, 2600);
  wm.Add("Nf82", "SiR1-2-S", true, 2750, -1, 2600);
  wm.Add("Ng82", "SiR1-3-S", true, 2740, -1, 2600);
  wm.Add("Nh82", "SiR1-4-S", true, 2740, -1, 2600);

  wm.Add("Na83", "ScL", true, 1, 1, 69.7);
  wm.Add("Nb83", "ScR", false, 1, 1, 69.2);
  wm.Add("Nc83", "ScGe", true, 1185, -1, 81.1);
  wm.Add("Nd83", "ScVe", false, 1, 1, 71.9);
  wm.Add("Ne83", "blank", false, 1, 1, 146.7);
  wm.Add("Nf83", "blank", false, 1, 1, 150);
  wm.Add("Ng83", "LiquidSc", false, 1, 1, 149);
  wm.Add("Nh83", "blank", false, 1, 1, 149);

  wm.Add("CaUH","Ge-S", true, 1140,-1, 42000);
  wm.Add("CbUH","blank", false, 1, -1, 37000);
  wm.Add("CcUH","blank", false, 1, -1, 0);
  wm.Add("CdUH","blank", false, 1, -1, 0);
  wm.Add("CeUH","blank", false, 1, -1, 0);
  wm.Add("CfUH","blank", false, 1, -1, 0);
  wm.Add("CgUH","blank", false, 1, -1, 0);
  wm.Add("ChUH","blank", false, 1, -1, 0);

  wm.Add("CaBU","muSc", false, 2071, 1, 0);
  wm.Add("CbBU","muScA", false, 2122, 1, 12.7);
  wm.Add("CcBU","muScA", false, 1, 1, 0);
  wm.Add("CdBU","blank", false, 1, 1, 0);

  wm.Add("ZZZZ","blank", false, 1, -1, 0);
  wm.Add("ZZZZ","blank", false, 1, -1, 0);
  wm.Add("ZZZZ","blank", false, 1, -1, 0);
  wm.Add("ZZZZ","blank", false, 1, -1, 0);

  wm.AddOffset(0);
  wm.AddOffset(0);
  wm.AddOffset(0);
  wm.AddOffset(0);

  return wm;
}
