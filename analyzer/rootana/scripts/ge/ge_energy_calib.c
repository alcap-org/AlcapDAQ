#include <vector>

enum IsotopeSource { CAPTURE, BACKGROUND };

struct AtomicLevel {
  unsigned int n;
  char l;
  unsigned int j[2];
};

struct Element {
  TString name;
  TString symbol;
  UInt_t z;
};

struct Isotope : Element {
  UInt_t a;
};

struct PhotonEmitter {
  std::vector<Double_t> E;
  std::vector<Double_t> I;
};

struct GammaSource : PhotonEmitter, Isotope {
  IsotopeSource type;
  Double_t lt;
};

struct XraySource : PhotonEmitter, Element {
};

struct DecayChain {
  std::vector<GammaSource> stages;
  std::vector< std::vector<double> > daughters;
};

std::vector<GammaSource> load_natural(const char* ifname) {
  

void ge_energy_calib() {
  return;
}
