#include "WireMap.hh"

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
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

void WireMap::Enable() {
  fEnabled.back() = true;
}

bool WireMap::Enable(unsigned int i) {
  if (i < fEnabled.size())
    return (fEnabled[i] = true);
  return false;
}

void WireMap::Disable() {
  fEnabled.back() = false;
}

bool WireMap::Disable(unsigned int i) {
  if (i < fEnabled.size())
    return !(fEnabled[i] = false);
  return false;
}

void WireMap::Add(const char bankname[], const char detname[], bool en, int ped, int pol, int off, double freq) {
  std::string bn(bankname);
  std::string dn(detname);
  Add(bn, dn, en, ped, pol, off, freq);
}
 
void WireMap::Add(std::string& bankname, std::string& detname, bool en, int ped, int pol, int off, double freq) {
  ++fNDets;
  fBankName.push_back(bankname);
  fDetName.push_back(detname);
  fEnabled.push_back(en);
  fPedestal.push_back(ped);
  fPolarity.push_back(pol);
  fOffset.push_back(off);
  fFrequency.push_back(freq);
}

void WireMap::Add(WireMap& wm, int i) {
  ++fNDets;
  fBankName.push_back(wm.GetBanks()[i]);
  fDetName.push_back(wm.GetDets()[i]);
  fEnabled.push_back(wm.GetEnableds()[i]);
  fPedestal.push_back(wm.GetPedestals()[i]);
  fPolarity.push_back(wm.GetPolarities()[i]);
  fOffset.push_back(wm.GetOffsets()[i]);
  fFrequency.push_back(wm.GetFrequencies()[i]);
}

void WireMap::UniqueFixes() {
  // The UH CAEN had the incorrect sampling
  // for runs 2091-2103
  double default_freq = 0.;
  double freq_fix = 1.6129e7;
  if (fRun >= 2091 && fRun <= 2103) {
    for (unsigned int i = 0; i < fBankName.size(); ++i) {
      if (fBankName[i].substr(0,1) == "C" && fBankName[i].substr(fBankName[i].size() - 2) == "UH") {
	while (fFrequency.size() <= i) {
	  fFrequency.push_back(default_freq);
	}
	fFrequency[i] = freq_fix;
      }
    }
  }
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

void WireMap::AddFrequency(double freq) {
  fFrequency.push_back(freq);
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
  double d;
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
	  fBankName.reserve(n);
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
	  fDetName.reserve(n);
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
	  fEnabled.reserve(n);
	  for (unsigned int i =0; i < n; ++i) {
	    f >> str >> c;
	    if (c == 'y') fEnabled.push_back(true);
	    else if (c == 'n') fEnabled.push_back(false);
	    else std::cout << "WireMap ERROR: '" << c <<
		   "' is not an ODB boolean!" << std::endl;
	  }
	} else if (key == PEDESTAL) {
	  n = GetArraySize(tmp);
	  fPedestal.reserve(n);
	  for (unsigned int i = 0; i < n; ++i) {
	    f >> str >> x;
	    fPedestal.push_back(x);
	  }
	} else if (key == POLARITY) {
       	  n = GetArraySize(tmp);
	  fPolarity.reserve(n);
	  for (unsigned int i = 0; i < n; ++i) {
	    f >> str >> x;
	    fPolarity.push_back(x);
	  }
	} else if (key == TIMESHIFT) {
	  n = GetArraySize(tmp);
	  fOffset.reserve(n);
	  for (unsigned int i = 0; i < n; ++i) {
	    f >> str >> x;
	    fOffset.push_back(x);
	  }
	} else if (key == FREQUENCY) {
	  n = GetArraySize(tmp);
	  fFrequency.reserve(n);
	  for (unsigned int i = 0; i < n; ++i) {
	    f >> str >> d;
	    fFrequency.push_back(d);
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
	    fOffset.size() == fNDets &&
	    fFrequency.size() == fNDets)
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
  if (wm.GetEnableds().size() > 0)
    fEnabled = wm.GetEnableds();
  if (wm.GetPedestals().size() > 0)
    fPedestal = wm.GetPedestals();
  if (wm.GetPolarities().size() > 0)
    fPolarity = wm.GetPolarities();
  if (wm.GetOffsets().size() > 0)
    fOffset = wm.GetOffsets();
  if (wm.GetFrequencies().size() > 0)
    fFrequency = wm.GetFrequencies();
}

void WireMap::Clear() {
  fRun = 0;
  fNDets = 0;
  fBankName.clear();
  fDetName.clear();
  fEnabled.clear();
  fPedestal.clear();
  fPolarity.clear();
  fOffset.clear();
  fFrequency.clear();
}

void WireMap::ResizeToBanks() {
  static const std::string default_det("blank");
  static const bool default_en = false;
  static const int default_ped = 0;
  static const int default_pol = 1;
  static const int default_off = 0;
  static const double default_freq = 0.;

  fNDets = fBankName.size();
  // Trim down
  while (fDetName.size() > fNDets)
    fDetName.pop_back();
  while (fEnabled.size() > fNDets)
    fEnabled.pop_back();
  while (fPedestal.size() > fNDets)
    fPedestal.pop_back();
  while (fPolarity.size() > fNDets)
    fPedestal.pop_back();;
  while (fOffset.size() > fNDets)
    fOffset.pop_back();
  while (fFrequency.size() > fNDets)
    fFrequency.pop_back();
  // Bulk up
  while (fDetName.size() < fNDets)
    fDetName.push_back(default_det);
  while (fEnabled.size() < fNDets)
    fEnabled.push_back(default_en);
  while (fPedestal.size() < fNDets)
    fPedestal.push_back(default_ped);
  while (fPolarity.size() < fNDets)
    fPolarity.push_back(default_pol);
  while (fOffset.size() < fNDets)
    fOffset.push_back(default_off);
  while (fFrequency.size() < fNDets)
    fFrequency.push_back(default_freq);
}

bool WireMap::AreThereDuplicates() {
  unsigned int n = 0;
  if (fEnabled.size() < fDetName.size()) {
    std::cout << "WireMap WARNING: Array of enabled information is smaller than " <<
      "                 array of detector name information." << std::endl;
    n = fEnabled.size();
  } else if (fEnabled.size() > fDetName.size()) {
    std::cout << "WireMap WARNING: Array of enabled information is larger than " <<
      "array of detector name information." << std::endl;
    n = fDetName.size();
  }

  // If two channels are both the same detector and
  // both enabled, return true because there is a problem.
  for (unsigned int i = 0; i < n; ++i)
    for (unsigned int j = i + 1; j < n; ++j)
      if (fDetName[i] == fDetName[j] &&
	  fEnabled[i] == fEnabled[j])
	return true;

  return false;
}

void WireMap::ClearDisabledDuplicateDetectors() {
  static const std::string default_det("blank");
  for (unsigned int i = 0; i < fDetName.size(); ++i) {
    for (unsigned int j = i + 1; j < fDetName.size(); ++j) {
      if (fDetName[i] == fDetName[j]) {
	if (j < fEnabled.size() && !fEnabled[j]) {
	  fDetName[j] = default_det;
	} else if (i < fEnabled.size() && !fEnabled[i]) {
	  fDetName[i] = default_det;
	} else {
	  std::cout << "WireMap ERROR: Couldn't determine which detector " << fDetName[i] <<
	    " to remove in run " << fRun << "!" << std::endl;
	}
      }
    }
  }
}

bool WireMap::IsODBFile(const std::string& fname) {
  return ".odb" == fname.substr(fname.size() - 4);
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

std::vector<double>& WireMap::GetFrequencies() {
  return fFrequency;
}

WireMap::key_t WireMap::GetKey(std::string& key) {
  static const std::string bankname_key("BankName");
  static const std::string detname_key("DetectorName");
  static const std::string enabled_key("Enabled");
  static const std::string polarity_key("TriggerPolarity");
  static const std::string pedestal_key("Pedestal");
  static const std::string offset_key("TimeShift");
  static const std::string frequency_key("SamplingFrequency");

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
  else if (key == frequency_key)
    return FREQUENCY;
  return UNKNOWN;
}

WireMap WireMap::Default() {
  WireMap wm;
  wm.Add("Na80", "SiL2-S", true, 2745, -1, 4000, 1.7e+07);
  wm.Add("Nb80", "SiR2-S", true, 2730, -1, 4000, 1.7e+07);
  wm.Add("Nc80", "blank", true, 1000, -1, 0, 8.5e+06);
  wm.Add("Nd80", "blank", false, 1, -1, 0, 1.7e+07);
  wm.Add("Ne80", "blank", false, 1, -1, 0, 1.7e+08);
  wm.Add("Nf80", "blank", false, 1, -1, 0, 1.7e+08);
  wm.Add("Ng80", "blank", false, 1, -1, 0, 1.7e+08);
  wm.Add("Nh80", "blank", false, 1, -1, 0, 1.7e+08);

  wm.Add("Na81", "SiL2-F", true, 1415, 1, 407, 1.7e+08);
  wm.Add("Nb81", "SiR2-F", true, 1210, 1, 444, 1.7e+08);
  wm.Add("Nc81", "SiL1-1-F", true, 1292, 1, 146.7, 1.7e+08);
  wm.Add("Nd81", "SiL1-2-F", true, 1300, 1, 150, 1.7e+08);
  wm.Add("Ne81", "SiL1-3-F", true, 1280, 1, 149, 1.7e+08);
  wm.Add("Nf81", "SiL1-4-F", true, 1290, 1, 149.4, 1.7e+08);
  wm.Add("Ng81", "SiR1-sum-F", true, 1, 1, 0, 1.7e+08);
  wm.Add("Nh81", "Ge-F", true, 1390, 1, 81.1, 1.7e+08);

  wm.Add("Na82", "SiL1-1-S", true, 2760, -1, 2431, 1.7e+07);
  wm.Add("Nb82", "SiL1-2-S", true, 2820, -1, 2427, 1.7e+07);
  wm.Add("Nc82", "SiL1-3-S", true, 2740, -1, 2431, 1.7e+07);
  wm.Add("Nd82", "SiL1-4-S", true, 2740, -1, 2429, 1.7e+07);
  wm.Add("Ne82", "SiR1-1-S", true, 2750, -1, 2600, 1.7e+07);
  wm.Add("Nf82", "SiR1-2-S", true, 2750, -1, 2600, 1.7e+07);
  wm.Add("Ng82", "SiR1-3-S", true, 2740, -1, 2600, 1.7e+07);
  wm.Add("Nh82", "SiR1-4-S", true, 2740, -1, 2600, 1.7e+07);

  wm.Add("Na83", "ScL", true, 1, 1, 69.7, 1.7e+07);
  wm.Add("Nb83", "ScR", false, 1, 1, 69.2, 3.4e+07);
  wm.Add("Nc83", "ScGe", true, 1185, -1, 81.1, 1.7e+08);
  wm.Add("Nd83", "ScVe", false, 1, 1, 71.9, 3.4e+07);
  wm.Add("Ne83", "blank", false, 1, 1, 146.7, 3.4e+07);
  wm.Add("Nf83", "blank", false, 1, 1, 150, 3.4e+07);
  wm.Add("Ng83", "LiquidSc", false, 1, 1, 149, 3.4e+07);
  wm.Add("Nh83", "blank", false, 1, 1, 149, 3.4e+07);

  wm.Add("CaUH","Ge-S", true, 1140,-1, 42000, 1e+08);
  wm.Add("CbUH","blank", false, 1, -1, 37000, 1e+08);
  wm.Add("CcUH","blank", false, 1, -1, 0, 1e+08);
  wm.Add("CdUH","blank", false, 1, -1, 0, 1e+08);
  wm.Add("CeUH","blank", false, 1, -1, 0, 1e+08);
  wm.Add("CfUH","blank", false, 1, -1, 0, 1e+08);
  wm.Add("CgUH","blank", false, 1, -1, 0, 1e+08);
  wm.Add("ChUH","blank", false, 1, -1, 0, 1e+08);

  wm.Add("CaBU","muSc", false, 2071, 1, 0, 2.5e+08);
  wm.Add("CbBU","muScA", false, 2122, 1, 12.7, 2.5e+08);
  wm.Add("CcBU","muScA", false, 1, 1, 0, 2.5e+08);
  wm.Add("CdBU","blank", false, 1, 1, 0, 2.5e+08);

  wm.Add("ZZZZ","blank", false, 1, -1, 0, 0.);
  wm.Add("ZZZZ","blank", false, 1, -1, 0, 0.);
  wm.Add("ZZZZ","blank", false, 1, -1, 0, 0.);
  wm.Add("ZZZZ","blank", false, 1, -1, 0, 0.);

  wm.AddOffset(0);
  wm.AddOffset(0);
  wm.AddOffset(0);
  wm.AddOffset(0);

  return wm;
}

void WireMap::Print() {
  using namespace std;
  stringstream ss;
  std::vector<unsigned int> sizes;
  sizes.push_back(fBankName.size());
  sizes.push_back(fDetName.size());
  sizes.push_back(fEnabled.size());
  sizes.push_back(fPedestal.size());
  sizes.push_back(fPolarity.size());
  sizes.push_back(fOffset.size());
  sizes.push_back(fFrequency.size());

  unsigned int n = sizes[0];
  for (unsigned int i = 1; i < sizes.size(); ++i)
    if (sizes[i] > n)
      n = sizes[i];

  cout << left;
  cout << setw(24) << "Run Number:" << fRun << endl;
  cout << setw(24) << "Number of detectors:" << fNDets << endl;
  
  ss << "Bank (" << fBankName.size() << ")";
  cout << setw(11) << ss.str(); ss.str(string());
  ss << "Detector (" << fDetName.size() << ")";
  cout << setw(15) << ss.str(); ss.str(string());
  ss << "Enabled (" << fEnabled.size() << ")";
  cout << setw(14) << ss.str(); ss.str(string());
  ss << "Pedestal (" << fPedestal.size() << ")";
  cout << setw(15) << ss.str(); ss.str(string());
  ss << "Polarity (" << fPolarity.size() << ")";
  cout << setw(15) << ss.str(); ss.str(string());
  ss << "TimeShift (" << fOffset.size() << ")";
  cout << setw(16) << ss.str(); ss.str(string());
  ss << "Frequency (" << fFrequency.size() << ")";
  cout << setw(16) << ss.str(); ss.str(string());

  cout << endl;
  cout << setfill('-') << setw(98) << '-' << setfill(' ') << endl;
  cout << setfill(' ') << right;
  for (unsigned int i = 0; i < n; ++i) {
    cout << setw(11);
    if (i < fBankName.size()) cout << fBankName[i];
    else cout << ' ';
    cout << setw(15);
    if (i < fDetName.size()) cout << fDetName[i];
    else cout << ' ';
    cout << setw(14);
    if (i < fEnabled.size()) cout << (fEnabled[i] ? 'y' : 'n');
    else cout << ' ';
    cout << setw(15);
    if (i < fPedestal.size()) cout << fPedestal[i];
    else cout << ' ';
    cout << setw(15);
    if (i < fPolarity.size()) cout << (fPolarity[i] == -1 ? '-' : '+');
    else cout << ' ';
    cout << setw(16);
    if (i < fOffset.size()) cout << -fOffset[i];
    else cout << ' ';
    cout << setw(16);
    if (i < fFrequency.size()) cout << fFrequency[i];
    else cout << ' ';
    cout << endl;
  }
}
