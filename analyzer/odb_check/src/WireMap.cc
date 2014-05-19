#include "WireMap.hh"

#include <string>

void WireMap::LoadFromODB(std::string odb_file, bool is_odb_file) {
  static std::string header("");
  static std::string bankname_key("BankName");
  static std::string detname_key("DetectorName");
  static std::string enabled_key("Enabled");
  static std::string polarity_key("TriggerPolarity");
  static std::string pedestal_key("Pedestal");
  static std::string offset_key("TimeShift");
  static char tmp[256];
  std::string ifname;
  // The headers in the ODB file have a leading slash,
  // in the MIDAS file this is not the case.
  if (is_odb_file) {
    ifname = fDataDirs.GetODBFileName(fRun);
    header = "[/Analyzer/WireMap]";
  } else {
    ifname = fDataDirs.GetRawFileName(fRun);
    header = "[Analyzer/WireMap]";
  }

  // Clear WireMap
  n = 0;
  bankname.clear();
  detname.clear();
  pedestal.clear();
  polarity.clear();
  offset.clear();
  threshold.clear()
  
  // Look through MIDAS or ODB file for appropriate banks
  std::ifstream f(odb_file.c_str());
  std::string str;
  std::stringstream ss;
  int x;
  while (f.good()) {
    f >> str;
    if (str == header) {
      while (f.good()) {
	f >> str;
	if (str == bankname_key) {
	  f.getline(tmp, 256);
	  str = tmp;
	  int pos[2];
	  pos[0] = str.find('[') + 1;
	  pos[1] = str.find(']');
	  str = str.substr(pos[0], pos[1] - pos[0]);
	  ss << str;
	  ss >> n;
	  for (unsigned int i = 0; i < n; ++i) {
	    f >> str;
	    f >> str;
	    bankname.push_back(str);
	  }
	} else if (str == detname_key) {
	  f.getline(tmp, 256);
	  for (unsigned int i = 0; i < n; ++i) {
	    f >> str;
	    f >> str;
	    detname.push_back(str);
	  }
	} else if (str == pedestal_key) {
	  f.getline(tmp, 256);
	  for (unsigned int i = 0; i < n; ++i) {
	    f >> str;
	    f >> x;
	    pedestal.push_back(x);
	  }
	} else if (str == polarity_key) {
	  f.getline(tmp, 256);
	  for (unsigned int i = 0; i < n; ++i) {
	    f >> str;
	    f >> x;
	    polarity.push_back(x);
	  }
	} else if (str == offset_key) {
	  f.getline(tmp, 256);
	  for (unsigned int i = 0; i < n; ++i) {
	    f >> str;
	    f >> x;
	    offset.push_back(x);
	  }
	}
	if (n != 0 &&
	    bankname.size() == n &&
	    detname.size() == n &&
	    pedestal.size() == n &&
	    polarity.size() == n &&
	    offset.size() == n)
	  return;2
      }
    }
  }
  std::cout << "Warning: Didn't find everything in the ODB!" << std::endl;

}
