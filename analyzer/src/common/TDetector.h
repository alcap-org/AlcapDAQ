#ifndef TDetector_h
#define TDetector_h
#include <vector>
#include <string>

#include <TROOT.h>
#include <TObject.h>

class TDetector : public TObject {
  public:
    TDetector();
    TDetector(std::string name);
    std::vector<std::string> sName;
    std::vector<std::string> sChannel;
    std::vector<Float_t> fMean;
    std::vector<Float_t> fStddev;
    std::vector<Float_t> fRms;
    std::vector<Float_t> fPedestal;
    std::vector<Float_t> fTime;
    std::vector<Float_t> fEnergy;
    std::vector<Float_t> fAmplitude;
    ClassDef(TDetector, 2);
};
#endif
