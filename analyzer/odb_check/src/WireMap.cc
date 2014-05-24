#include "WireMap.hh"

#include <string>

WireMap::WireMap() {
}

WireMap::WireMap(int run, std::string& odb_file) {
  Load(run, odb_file);
}

void WireMap::Add(std::string& bankname, std::string& detname, int ped, int pol, int thresh, int off) {
  ++fNDets;
  fBankName.push_back(bankname);
  fDetName.push_back(detname);
  fPedestal.push_back(ped);
  fPolarity.push_back(pol);
  /*** THRESHOLD NOT IMPLEMENTED ***/
  fOffset.push_back(off);
}

void WireMap::Add(const WireMap& wm, int i) {
  ++fNDets;
  fBankName.push_back(wm.GetBanks()[i]);
  fDetName.push_back(wm.GetDets()[i]);
  fPedestal.push_back(wm.GetPedestals()[i]);
  fPolarity.push_back(wm.GetPolarities()[i]);
  /*** THRESHOLD NOT IMPLEMENTED ***/
  fOffset.push_back(wm.push_back()[i]);
}

void WireMap::AddBank(std::string& bank) {
  ++fNDets;
  fBankName.push_back(bank);
}

void WireMap::AddDet(std::string& det) {
  fDetName.push_back(det);
}

void WireMap::AddPedestal(int ped) {
  fPedestal.push_back(ped);
}

void WireMap::AddPolarity(int pol) {
  fPolarity.push_back(pol);
}

/*** THRESHOLD NOT IMPLEMENTED ***/

void WireMap::AddOffset(int off) {
  fOffset.push_back(off);
}

void WireMap::Load(int run_number, std::string& odb_file) {
  static const std::string header("[/Analyzer/WireMap");
  static const std::string bankname_key("BankName");
  static const std::string detname_key("DetectorName");
  static const std::string enabled_key("Enabled");
  static const std::string polarity_key("TriggerPolarity");
  static const std::string pedestal_key("Pedestal");
  static const std::string offset_key("TimeShift");
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
  std::stringstream ss;
  int x;
  while (f.good()) {
    f >> str;
    if (str == header) {
      while (f.good()) {
	f >> str;
	f.getline(tmp, 256);
	if (str == bankname_key) {
	  if (fNDets == 0)
	    fNDets = GetNDetsFromLine(tmp);
	  for (unsigned int i = 0; i < fNDets; ++i) {
	    f >> str;
	    if (f.read(tmp, 1).peek() == '\n') // Strings can be null, cause streaming problems
	      str = "";
	    else
	      f >> str;
	    fBankName.push_back(str);
	  }
	} else if (str == detname_key) {
	  if (fNDets == 0)
	    fNDets = GetNDetsFromLine(tmp);
	  for (unsigned int i = 0; i < n; ++i) {
	    f >> str;
	    if (f.read(tmp, 1).peek() == '\n') // Strings can be null, cause streaming problems
	      str = "";
	    else
	      f >> str;
	    fDetName.push_back(str);
	  }
	} else if (str == pedestal_key) {
	  if (fNDets == 0)
	    fNDets = GetNDetsFromLine(tmp);
	  for (unsigned int i = 0; i < n; ++i) {
	    f >> str;
	    f >> x;
	    fPedestal.push_back(x);
	  }
	} else if (str == polarity_key) {
	  if (fNDets == 0)
	    fNDets = GetNDetsFromLine(tmp);
	  for (unsigned int i = 0; i < n; ++i) {
	    f >> str;
	    f >> x;
	    fPolarity.push_back(x);
	  }
	} else if (str == offset_key) {
	  if (fNDets == 0)
	    fNDets = GetNDetsFromLine(tmp);
	  for (unsigned int i = 0; i < n; ++i) {
	    f >> str;
	    f >> x;
	    fOffset.push_back(x);
	  }
	}
	// If everything is the right size
	// That means everything is finished
	/*** THRESHOLD NOT IMPLEMENTED YET ***/
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
  // If we get to this point, things are not all equal in size
  std::cout <<
    "WireMap WARNING: Didn't find everything in the ODB! (" <<
    odb_file << ")" << std::endl;

}

void WireMap::LoadOver(const WireMap& wm) {
  // Load the new WireMap variables if they exist
  // This seems to be how the ODB operates
  fNDets = wm.GetNDets();
  if (wm.GetBanks().size() > 0)
    fBankName = wm.GetBanks();
  if (wm.GetDets().size() > 0)
    fDetName = wm.GetDets();
  if (wm.GetPedestals().size() > 0)
    fPedestal = wm.GetPedestals();
  if (wm.Polarities().size() > 0)
    fPolarity = wm.GetPolarities();
  /*** THRESHOLDS NOT IMPLEMENTED ***/
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
  fThreshold.clear();
  fOffset.clear();
}

void WireMap::ResizeToBanks() {
  static const std::string default_det("blank");
  static const int default_ped = 0;
  static const int default_pol = 1;
  static const int default_thresh = 0;
  static const int default_off = 0;

  fNDets = fBankName.size();
  // Trim down
  while (fDetName.size() > fNDets)
    fDetName.pop_back();
  while (fPedestal.size() > fNDets)
    fPedestal.pop_back();
  while (fPolarity.size() > fNDets)
    fPedestal.pop_back();;
  /*** THRESHOLD NOT IMPLEMENTED YET ***/
  while (fOffset.size() > fNDets)
    fOffset.pop_back();
  // Bulk up
  while (fDetName.size() < fNDets)
    fDetName.push_back(default_det);
  while (fPedestal.size() < fNDets)
    fPedestal.push_back(default_ped);
  while (fPolarity.size() < fNDets)
    fPolarity.push_back(default_polarity);
  /*** THRESHOLD NOT IMPLEMENTED YET ***/
  while (fOffset.size() < fNDets)
    fOffset.push_back(default_off);
}

int WireMap::GetNDetsFromLine(char (&tmp)[256]) {
  // Get # of detectors from line in ODB of the form
  // VARIBALE = TYPE[#] :
  std::string str;
  int pos[2];
  int n;

  str = tmp;
  pos[0] = str.find('[') + 1;
  pos[1] = str.find(']');
  str = str.substr(pos[0], pos[1] - pos[0]);
  ss << str;
  ss >> n;

  return n;
}

int WireMap::GetRun() const {
  return fRun;
}

int WireMap::GetNDets() const {
  return fNDets;
}

std::vector<std::string>& WireMap::GetBanks() const {
  return fBankName
}

std::vector<std::string>& WireMap::GetDets() const {
  return fDetName;
}

std::vector<int>& WireMap::GetPedestals() const {
  return fPedestal;
}

std::vector<int>& WireMap::GetPolarities() const {
  return fPolarity;
}

std::vector<int>& WireMap::GetThresholds() const {
  return fThreshold;
}

std::vector<int>& WireMap::GetOffsets() const {
  return fOffset;
}

WireMap WireMap::Default() {
  WireMap wm;
  wm.Add("Na80", "SiL2-S", 2745, -1, 0, 4000);
  wm.Add("Nb80", "SiR2-S", 2730, -1, 0, 4000);
  wm.Add("Nc80", "blank", 1000, -1, 0,0);
  wm.Add("Nd80", "blank", 1, -1, 0, 0);
  wm.Add("Ne80", "blank", 1, -1, 0, 0);
  wm.Add("Nf80", "blank", 1, -1, 0, 0);
  wm.Add("Ng80", "blank", 1, -1, 0, 0);
  wm.Add("Nh80", "blank", 1, -1, 0, 0);

  wm.Add("Na81", "SiL2-F", 1415, 1, 0, 407);
  wm.Add("Nb81", "SiR2-F", 1210, 1, 0, 444);
  wm.Add("Nc81", "SiL1-1-F", 1292, 1, 0, 146.7);
  wm.Add("Nd81", "SiL1-2-F", 1300, 1, 0, 150);
  wm.Add("Ne81", "SiL1-3-F", 1280, 1, 0, 149);
  wm.Add("Nf81", "SiL1-4-F", 1290, 1, 0, 149.4);
  wm.Add("Ng81", "SiR1-sum-F", 1, 1, 0, 0);
  wm.Add("Nh81", "Ge-F", 1390, 1, 0, 81.1);

  wm.Add("Na82", "SiL1-1-S", 2760, -1, 0, 2431);
  wm.Add("Nb82", "SiL1-2-S", 2820, -1, 0, 2427);
  wm.Add("Nc82", "SiL1-3-S", 2740, -1, 0, 2431);
  wm.Add("Nd82", "SiL1-4-S", 2740, -1, 0, 2429);
  wm.Add("Ne82", "SiR1-1-S", 2750, -1, 0, 2600);
  wm.Add("Nf82", "SiR1-2-S", 2750, -1, 0, 2600);
  wm.Add("Ng82", "SiR1-3-S", 2740, -1, 0, 2600);
  wm.Add("Nh82", "SiR1-4-S", 2740, -1, 0, 2600);

  wm.Add("Na83", "ScL", 1, 1, 0, 69.7);
  wm.Add("Nb83", "ScR", 1, 1, 0, 69.2);
  wm.Add("Nc83", "ScGe", 1185, -1,0, 81.1);
  wm.Add("Nd83", "ScVe", 1, 1, 0, 71.9);
  wm.Add("Ne83", "blank", 1, 1, 0, 146.7);
  wm.Add("Nf83", "blank", 1, 1, 0, 150);
  wm.Add("Ng83", "LiquidSc", 1, 1, 0, 149);
  wm.Add("Nh83", "blank", 1, 1, 0, 149);

  wm.Add("CaUH","Ge-S",1140,-1,0,42000);
  wm.Add("CbUH","blank",1,-1,0,37000);
  wm.Add("CcUH","blank",1,-1,0,0);
  wm.Add("CdUH","blank",1,-1,0,0);
  wm.Add("CeUH","blank",1,-1,0,0);
  wm.Add("CfUH","blank",1,-1,0,0);
  wm.Add("CgUH","blank",1,-1,0,0);
  wm.Add("ChUH","blank",1,-1,0,0);

  wm.Add("CaBU","muSc",2071,1,0,0);
  wm.Add("CbBU","muScA",2122,1,0,12.7);
  wm.Add("CcBU","muScA",1,1,0,);
  wm.Add("CdBU","blank",1,1,0,);

  wm.Add("ZZZZ","blank",1,-1,0,);
  wm.Add("ZZZZ","blank",1,-1,0,);
  wm.Add("ZZZZ","blank",1,-1,0,);
  wm.Add("ZZZZ","blank",1,-1,0,);

  wm.AddOffset(0);
  wm.AddOffset(0);
  wm.AddOffset(0);
  wm.AddOffset(0);

  return wm;
}
