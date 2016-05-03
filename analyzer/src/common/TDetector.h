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
    ~TDetector();
    std::vector<std::string> GetName() { return sName; }
    std::vector<std::string> GetChannel() { return sChannel; }
    std::vector<Float_t> GetMean() { return fMean; }
    std::vector<Float_t> GetStddev() { return fStddev; }
    std::vector<Float_t> GetRms() { return fRms; }
    std::vector<Float_t> GetPedestal() { return fPedestal; }
    std::vector<Float_t> GetTime() { return fTime; }
    std::vector<Float_t> GetEnergy() { return fEnergy; }
    std::vector<Float_t> GetAmplitude() { return fAmplitude; }

    void SetName(std::vector<std::string> Name) { Name = sName; };
    void SetChannel(std::vector<std::string> Channel) { Channel = sChannel; };
    void SetMean(std::vector<Float_t> Mean) { Mean = fMean; };
    void SetStddev(std::vector<Float_t> Stddev) { Stddev = fStddev; };
    void SetRms(std::vector<Float_t> Rms) { Rms = fRms; };
    void SetPedestal(std::vector<Float_t> Pedestal) { Pedestal = fPedestal; };
    void SetTime(std::vector<Float_t> Time) { Time = fTime; };
    void SetEnergy(std::vector<Float_t> Energy) { Energy = fEnergy; };
    void SetAmplitude(std::vector<Float_t> Amplitude) { Amplitude = fAmplitude; };
  private:
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
