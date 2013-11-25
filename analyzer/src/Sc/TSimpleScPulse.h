#ifndef TSCPULSE_H_YAHE2XWS
#define TSCPULSE_H_YAHE2XWS

#include <vector>
#include <string>

class TOctalFADCIsland;
class TH1I;

class TSimpleScPulse : public TOctalFADCIsland 
{
	public:
		TSimpleScPulse(TOctalFADCIsland *island, unsigned int nped = 7);
		TSimpleScPulse(TOctalFADCIsland *island, double pedestal);
		~TSimpleScPulse();
		TSimpleScPulse(TOctalFADCIsland *island, std::string detname, 
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
		double GetRMSNoise() {return fRMSNoise;}
		bool IsPositive() {return fIsPositive;};
		void PrintInfo();

		TSimpleScPulse * Invert();

		TH1I * GetWaveform(std::string histname);
		double GetPulseHeight();
		double GetPulseTime();
}; // end of class declaration TSimpleScPulse 

#endif /* end of include guard: TSCPULSE_H_YAHE2XWS */

