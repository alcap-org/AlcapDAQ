#ifndef DEADTIMEGE_H_DUEVBG4Q
#define DEADTIMEGE_H_DUEVBG4Q

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class DeadTimeGe : public FillHistBase 
{
	public:
		DeadTimeGe(char * HistogramDirectoryName);
		~DeadTimeGe();
	private:
		/* data */
	public:
		virtual int ProcessEntry(TGlobalData * gData, TSetupData * gSetup);
}; // end of class declaration DeadTimeGe 


#endif /* end of include guard: DEADTIMEGE_H_DUEVBG4Q */

