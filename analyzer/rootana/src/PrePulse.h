#include <vector>

class TPulseIsland;

class PrePulse {

 private:
  int fStart;
  int fStop;
  int fLocation;
  int fHeight;
  TPulseIsland* fPulse;

 public:
  PrePulse(int start, int stop, int location, int height, TPulseIsland* pulse);
  ~PrePulse();

 public:
  int GetStart() const;
  int GetStop() const;
  int GetLocation() const;
  int GetHeight() const;
  TPulseIsland* GetPulse() const;

 public:
  static std::vector<PrePulse> FindPrePulses(TPulseIsland*, int rise, int fall);

};
