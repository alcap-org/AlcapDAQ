#include "PrePulse.h"
#include "TPulseIsland.h"

#include <cmath>
#include <vector>
#include <iostream>

PrePulse::PrePulse(int start, int stop, int loc, int height, TPulseIsland* pulse) :
  fStart(start), fStop(stop), fLocation(loc), fHeight(height), fPulse(pulse) {}

PrePulse::~PrePulse() {}

int PrePulse::GetStart() const {
  return fStart;
}

int PrePulse::GetStop() const {
  return fStop;
}

int PrePulse::GetLocation() const {
  return fLocation;
}

int PrePulse::GetHeight() const {
  return fHeight;
}

TPulseIsland* PrePulse::GetPulse() const {
  return fPulse;
}

void PrePulse::Print() {
  std::cout << "PrePulse Information:"
	    << "\t" << "Strt" << "\t" << "Stop"
	    << "\t" << "Loc" << "\t" << "Hght"
	    << std::endl;
  std::cout << "                     "
	    << "\t" << fStart << "\t" << fStop
	    << "\t" << fLocation << "\t" << fHeight
	    << std::endl;
}

std::vector<PrePulse> PrePulse::FindPrePulses(TPulseIsland* pulse, int rise, int fall) {
  // Looks through pulse to look for samples that go rise as fast as rise between two samples
  // and then fall 
  std::vector<int> samples = pulse->GetSamples();
  unsigned int nsamps = samples.size();
  int pPed = pulse->GetPedestal(0);
  int pPol = pulse->GetTriggerPolarity();
  std::vector<PrePulse> prepulses;

  int s1, s2, ds;
  int start, stop, loc, height;
  bool found = false;
  for (unsigned int i = 1; i < nsamps; ++i) {
    s1 = pPol * (samples[i-1] - pPed);
    s2 = pPol * (samples[i] - pPed);
    ds = s2 - s1;
    if (found) {
      if (-ds > fall) {
	stop = (int)i;
	prepulses.push_back(PrePulse(start, stop, loc, height, pulse));
	start = stop = loc = height = 0;
	found = false;
      }
      if (s2 > height) {
	loc = (int)i;
	height = s2;
      }
    } else {
      if (ds > rise) {
	found = true;
	start = (int)(i - 1);
	loc = start + 1;
	height = s2;
      }
    }
  }
  
  return prepulses;
}
