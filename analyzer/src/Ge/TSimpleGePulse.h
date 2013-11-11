#ifndef TGEPULSE_H_YAHE2XWS
#define TGEPULSE_H_YAHE2XWS

#include <vector>
#include <string>

class TOctalFADCIsland;
class TH1I;

class TSimpleGePulse : public TOctalFADCIsland 
{
	public:
		TSimpleGePulse(TOctalFADCIsland *island, unsigned int nped = 7);
		TSimpleGePulse(TOctalFADCIsland *island, double pedestal);
		~TSimpleGePulse();
		TSimpleGePulse(TOctalFADCIsland *island, std::string detname, 
				unsigned int nped = 7);
	private:
		std::string fDetName;
		double fPedestal;
		double fRMSNoise;
		double fThreshold;
		bool fIsPositive;
		unsigned int fNPedSamples;
	public:
		double GetPedestal(){return fPedestal;}
		double GetThreshold() {return fThreshold;}
		bool IsPositive() {return fIsPositive;};
		void PrintInfo();

		TSimpleGePulse * Invert();

		TH1I * GetWaveform(std::string histname);
		double GetPulseHeight();
		double GetPulseTime();
}; // end of class declaration TSimpleGePulse 

#endif /* end of include guard: TGEPULSE_H_YAHE2XWS */

