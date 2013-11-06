#ifndef TSIPULSE_H_YAHE2XWS
#define TSIPULSE_H_YAHE2XWS

#include <vector>
class TOctalFADCIsland;

class TSimpleSiPulse : public TOctalFADCIsland 
{
	public:
		TSimpleSiPulse(TOctalFADCIsland *island, unsigned int nped = 7);
		~TSimpleSiPulse();
	private:
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
		//TSimpleSiPulse Offset(int offset);
		TSimpleSiPulse * Invert();
}; // end of class declaration TSimpleSiPulse 

#endif /* end of include guard: TSIPULSE_H_YAHE2XWS */

