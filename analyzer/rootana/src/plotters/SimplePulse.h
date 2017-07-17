#ifndef SimplePulse_h_
#define SimplePulse_h_

#include "TBits.h"

struct SimplePulse {
public:
  
SimplePulse() : tpi_id(0), E(0), Amp(0), tblock(0), tTME(0), bit_mask(8) { }

  int tpi_id; // ID of the TPulseIsland this pulse came from
  double E; // energy of this pulse
  double Amp; // amplitude of this pulse
  double tblock; // MIDAS block time of this pulse (between 0 and 100 ms)
  double tTME; // time to the central muon in this TMuonEvent

  enum BitMask { kDoubleCounted };

  TBits bit_mask; // used for various statuses (e.g. double counted, unresolved pile-up etc.)
};


#endif
