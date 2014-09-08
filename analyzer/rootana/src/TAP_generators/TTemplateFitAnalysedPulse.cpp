#include "TTemplateFitAnalysedPulse.h"
#include "TF1.h"
#include "TH1F.h"
#include "Functions.h"

void TTemplateFitAnalysedPulse::Draw(const TH1F* tpi_pulse)const{
	if(tpi_pulse) {
	  std::string name=tpi_pulse->GetName();
	  TF1* tap_pulse=new TF1((name+"_AP").c_str(),functions::gauss_lin,0,1000,5);
    // parameters:
    // 0: constant (pedestal)
    // 1: gradient
    // 2: Gauss amplitude
    // 3: Gauss mean
    // 4: Gauss width
      tap_pulse->SetParameters(GetPedestal(),fGradient,GetAmplitude(),GetTime(),fWidth);
      tap_pulse->Draw();
      tap_pulse->Write();
	}
}
