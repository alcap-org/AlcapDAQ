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
    ClassDef(TDetector, 2);
};
#endif
