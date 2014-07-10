#ifndef TANALYSEDPULSEMAPWRAPPER_H_CKBU0TFP
#define TANALYSEDPULSEMAPWRAPPER_H_CKBU0TFP

#include <vector>
#include <map>
#include <string>

#include "definitions.h"
#include "TObject.h"
#include "TAnalysedPulse.h"

using std::map;
using std::string;
using std::vector;

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana
/// \deprecated
/// This was used to save TAPs to a file, which will be taken care of by the
/// EventNavigator in the future.
////////////////////////////////////////////////////////////////////////////////
class TAnalysedPulseMapWrapper : public TObject 
{
	public:
		TAnalysedPulseMapWrapper();
		TAnalysedPulseMapWrapper (const SourceAnalPulseMap& aMap);
		~TAnalysedPulseMapWrapper();

	private:
		SourceAnalPulseMap fChn2PulseMap;

	public:
		void ShowInfo();
		void Reset();
		SourceAnalPulseMap GetMap()
		{
			return fChn2PulseMap;
		}

		void SetMap(const SourceAnalPulseMap& aMap)
		{
			fChn2PulseMap = aMap;
		}

		ClassDef(TAnalysedPulseMapWrapper, 2);

}; // end of class declaration TAnalysedPulseMapWrapper 

#endif /* end of include guard: TANALYSEDPULSEMAPWRAPPER_H_CKBU0TFP */

