#ifndef TMuonEvent_hh_
#define TMuonEvent_hh_

// Being lazy to write getters and setters for each detector
#define PREP_DETECTOR(detector)\
	public:\
TDetectorPulse* Get##detector()const{return f##detector;};\
void Set##detector(TDetectorPulse* p){f##detector=p;};\
	private:\
		TDetectorPulse* f##detector;


#include "TDetectorPulse.h"

class TMuonEvent{
        public:
                TMuonEvent();
                ~TMuonEvent();

                TDetectorPulse* GetPulse(const std::string& detector)const;
                void SetPulse(const std::string& detector, TDetectorPulse* pulse);
		
		int GetNumPulses()const;

	private:
		void ResetDetectors();

		// Add getters and setters and pulses for all detectors
		PREP_DETECTOR( Ge )
		PREP_DETECTOR( LiquidSc )
		PREP_DETECTOR( NDet )
		PREP_DETECTOR( NDet2 )
		PREP_DETECTOR( ScGe )
		PREP_DETECTOR( ScL )
		PREP_DETECTOR( ScR )
		PREP_DETECTOR( ScVe )
		PREP_DETECTOR( SiL1_1 )
		PREP_DETECTOR( SiL1_2 )
		PREP_DETECTOR( SiL1_3 )
		PREP_DETECTOR( SiL1_4 )
		PREP_DETECTOR( SiL2 )
		PREP_DETECTOR( SiR1_1 )
		PREP_DETECTOR( SiR1_2 )
		PREP_DETECTOR( SiR1_3 )
		PREP_DETECTOR( SiR1_4 )
		PREP_DETECTOR( SiR1_sum )
		PREP_DETECTOR( SiR2 )
		PREP_DETECTOR( MuSc )
		PREP_DETECTOR( MuScA )

		static std::vector<std::string> fDetectorNames;
};
#undef PREP_DETECTOR
#endif // TMuonEvent_hh_