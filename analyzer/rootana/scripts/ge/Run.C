#include "Run.h"
#include "Source.h"

#include "TTimeStamp.h"

#include <iostream>

Run::Run(int n, int nblock, TTimeStamp t0, Source src,
	 bool eff, Double_t hiscaleest, Double_t dead_time)
: n(n), nblock(nblock), t0(t0), src(src),
  eff(eff), hiscaleest(hiscaleest), deadtime(dead_time) {}

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
  /*  std::cout << "LT = " << nblock << " * " << tblock << " = " << nblock*tblock << std::endl;
  std::cout << "End Time - Start Time = " << end_time << " - " << start_time << " = " << end_time - start_time << std::endl;
  std::cout << "Dead Time = " << (end_time - start_time) - nblock*tblock << std::endl;
  double dead_time_per_block = ((end_time - start_time) - nblock*tblock)/nblock;
  std::cout << "Dead Time/block = " << dead_time_per_block << std::endl;
  std::cout << "N Deacys(LT) = " << src.Decays(t0, nblock*tblock) << std::endl;
  double time_per_block = (end_time - start_time)/nblock;
  std::cout << "Time per block = " << time_per_block << std::endl;
  double total_decays = 0;
  for (int i_block = 0; i_block < nblock; ++i_block) {
    total_decays += src.Decays(t0+i_block*time_per_block, tblock);
  }
  std::cout << "total decays = " << total_decays << std::endl;
  std::cout << "N Deacys(end - start) = " << src.Decays(t0, end_time - start_time) << std::endl;
  */
  return src.Decays(t0, LT());
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
