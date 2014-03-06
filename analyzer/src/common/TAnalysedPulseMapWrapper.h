#ifndef TANALYSEDPULSEMAPWRAPPER_H_CKBU0TFP
#define TANALYSEDPULSEMAPWRAPPER_H_CKBU0TFP

#include <vector>
#include <map>
#include <string>

#include "TObject.h"
#include "TAnalysedPulse.h"

using std::map;
using std::string;
using std::vector;

class TAnalysedPulseMapWrapper : public TObject 
{
	public:
		TAnalysedPulseMapWrapper();
		TAnalysedPulseMapWrapper (map<string, vector<TAnalysedPulse *> > aMap);
		~TAnalysedPulseMapWrapper();

	private:
		map<string, vector<TAnalysedPulse *> > fChn2PulseMap;

	public:
		void ShowInfo();
		void Reset();
		map<string, vector<TAnalysedPulse *> > GetMap()
		{
			return fChn2PulseMap;
		}

		void SetMap(map<string, vector<TAnalysedPulse *> > aMap)
		{
			fChn2PulseMap = aMap;
		}

		ClassDef(TAnalysedPulseMapWrapper, 1);

}; // end of class declaration TAnalysedPulseMapWrapper 

#endif /* end of include guard: TANALYSEDPULSEMAPWRAPPER_H_CKBU0TFP */

