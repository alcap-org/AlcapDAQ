#pragma once

#include "./scripts/TMETree/TMETreeBranches.h"
#include "./src/plotters/SimplePulse.h"

#include <vector>

namespace TMECuts {
  bool OnlyOneHit(const std::vector<SimplePulse>* sps) {
    return sps->size() == 1;
  }
  bool OnlyOneHit(const std::vector<std::vector<SimplePulse>*>& sps) {
    int n = 0;
    for (int i = 0; i < sps.size(); ++i)
      n += sps[i]->size();
    return n == 1;
  }
  bool OnlyOneMuon() {
    // Make sure to call CollectChannels first
    return OnlyOneHit(all_SiT_channels);
  }
  bool AtMostOneHit(const std::vector<SimplePulse>* sps) {
    return sps->size() <= 1;
  }
  bool Valid(const std::vector<SimplePulse>* sps) {
    return OnlyOneHit(sps);
  }
  bool Valid(const std::vector<std::vector<SimplePulse>*>& sps) {
    return OnlyOneHit(sps);
  }
  bool TCorr(double t1, double t2, double dt) {
    return std::abs(t1-t2) < dt;
  }
  bool TCorr(const SimplePulse& p1, const SimplePulse& p2, double dt) {
    return TCorr(p1.tTME, p2.tTME, dt);
  }
  bool ValidCentralMuE(double elo, double ehi) {
    return elo < centralMuonEnergy && centralMuonEnergy < ehi;
  }
  bool PileupProtected(double pp=20e3) {
    // return SiT_1->size() + SiT_2->size() + SiT_3->size() + SiT_4->size() == 1;
    return timeToPrevTME > pp && timeToNextTME > pp;
  }
}

namespace TMECal {
  struct ECal {
    double e0, g;
    ECal(double e0=0, double g=0) : e0(e0), g(g) {}
    double Eval      (double adc) const { return e0+g*adc; }
    double operator()(double adc) const { return Eval(adc); }
  };
  ECal SiL1A2E[16];
  ECal SiL3A2E(4.47046,11.0748);
  ECal SiR1A2E[4];
  ECal SiR2A2E(4.35313,85.7855);
  ECal SiR3A2E(5.90075,35.8103);
  ECal SiTA2E[4];
  ECal GeLoA2E(0.396515,0.261953);
  ECal GeHiA2E(0.184892,-0.896645);
  void Init() {
    SiL1A2E[0]  = ECal(1.370678, 2.056969);
    SiL1A2E[1]  = ECal(1.370678, 2.056969);
    SiL1A2E[2]  = ECal(1.370678, 2.056969);
    SiL1A2E[3]  = ECal(1.370678, 2.056969);
    SiL1A2E[4]  = ECal(1.370678, 2.056969);
    SiL1A2E[5]  = ECal(1.370678, 2.056969);
    SiL1A2E[6]  = ECal(1.370678, 2.056969);
    SiL1A2E[7]  = ECal(1.370678, 2.056969);
    SiL1A2E[8]  = ECal(1.370678, 2.056969);
    SiL1A2E[9]  = ECal(1.370678, 2.056969);
    SiL1A2E[10] = ECal(1.370678, 2.056969);
    SiL1A2E[11] = ECal(1.370678, 2.056969);
    SiL1A2E[12] = ECal(1.370678, 2.056969);
    SiL1A2E[13] = ECal(1.370678, 2.056969);
    SiL1A2E[14] = ECal(1.370678, 2.056969);
    SiL1A2E[15] = ECal(1.370678, 2.056969);
    SiR1A2E[0]  = ECal(1.81475, -48.8053);
    SiR1A2E[1]  = ECal(1.80298, -34.7606);
    SiR1A2E[2]  = ECal(1.83618, -62.3042);
    SiR1A2E[3]  = ECal(1.80375, -76.4902);
    SiTA2E[0]   = ECal(2.07393, -33.4731);
    SiTA2E[1]   = ECal(1.96649, -12.673);
    SiTA2E[2]   = ECal(1.99088, -37.2894);
    SiTA2E[3]   = ECal(2.00442, -24.4704);
  }
}
