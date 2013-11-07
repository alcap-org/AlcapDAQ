#ifndef TSIPULSE_H_YAHE2XWS
#define TSIPULSE_H_YAHE2XWS

#include <vector>
#include <string>

class TOctalFADCIsland;
class TH1;

class TSimpleSiPulse : public TOctalFADCIsland 
{
	public:
		TSimpleSiPulse(TOctalFADCIsland *island, unsigned int nped = 7);
		TSimpleSiPulse(TOctalFADCIsland *island, std::string detname, 
				unsigned int nped = 7);
		~TSimpleSiPulse();
	private:
		std::string fDetName;
		double fPedestal;
		double fRMSNoise;
		double fThreshold;
		bool fIsPositive;
		unsigned int fNPedSamples;
		std::string fRawHistName;
		std::string fPulseHeightHistName;
		std::string fTimingHistName;
	public:
		double GetPedestal(){return fPedestal;}
		double GetThreshold() {return fThreshold;}
		bool IsPositive() {return fIsPositive;};
		void PrintInfo();
		TSimpleSiPulse * Offset(){ return Offset(fPedestal);}
		TSimpleSiPulse * Offset(double offset);
		TSimpleSiPulse * Invert();
		std::string GetRawHistName();
		std::string GetPulseHeightHistName();
		std::string GetTimingHistName();

		TH1 * GetWaveform(std::string histname);
}; // end of class declaration TSimpleSiPulse 

#endif /* end of include guard: TSIPULSE_H_YAHE2XWS */

