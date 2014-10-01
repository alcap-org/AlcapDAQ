#include <vector>

enum PhotonType { XRAY, GAMMA };
enum EmitterType { NAT, NUCLCAP, ATOMCAPT };

struct Element {
  TString name;
  TString symbol;
  UInt_t z;
};

struct Isotope : Element {
  Element el;
  UInt_t a;
};

struct PhotonEmitter {
  std::vector<Double_t> e;
  std::vector<Double_t> i;
  PhotonType type;
  EmitterType src;
  Double_t lt;
};

struct Chain {
  std::vector<PhotonEmitter> em;
  std::vector<
