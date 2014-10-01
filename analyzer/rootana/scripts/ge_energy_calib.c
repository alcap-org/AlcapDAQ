#include <vector>

enum IsotopeSource { CAPTURE, BACKGROUND };

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
  std::vector<GammaSource> products;
  std::vector< std::vector<unsigned int> > parents;
  std::vector< std::vector<unsigned int> > daughters;
};

void ge_energy_calib() {
  return;
}
