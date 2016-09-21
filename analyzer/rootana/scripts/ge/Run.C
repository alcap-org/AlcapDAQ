#include "Run.h"
#include "Source.h"

#include "TTimeStamp.h"

Run::Run(int n, int nblock, TTimeStamp t0, Source src,
	 bool eff, Double_t hiscaleest)
: n(n), nblock(nblock), t0(t0), src(src),
  eff(eff), hiscaleest(hiscaleest) {}

int Run::N() {
  return n;
}
Double_t Run::LT() {
  return nblock*tblock;
}
TTimeStamp Run::T0() {
  return t0;
}
Double_t Run::Decays() {
  return src.Decays(t0, nblock*tblock);
}
int Run::Spectrum(const Double_t*& e, const Double_t*& i,
		  const Double_t*& adc) { 
  return src.Spectrum(e, i, adc);
}
bool Run::ValidForEff() {
  return eff;
}
Double_t Run::HiScaleEstimate() {
  return hiscaleest;
}
