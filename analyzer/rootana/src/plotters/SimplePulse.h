#ifndef SimplePulse_h_
#define SimplePulse_h_

enum BitMask { kOK=0, kDoubleCounted=1, kPileUp=2, kSomething=4 };

struct SimplePulse {
public:
  
SimplePulse() : tpi_id(0), E(0), Amp(0), tblock(0), tTME(0), bit_mask(0) { }

  int tpi_id; // ID of the TPulseIsland this pulse came from
  double E; // energy of this pulse
  double Amp; // amplitude of this pulse
  double tblock; // MIDAS block time of this pulse (between 0 and 100 ms)
  double tTME; // time to the central muon in this TMuonEvent
  char bit_mask; // used for various statuses (e.g. double counted, unresolved pile-up etc.)
};


#endif
