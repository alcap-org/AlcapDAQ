#ifndef TMuonEvent_hh_
#define TMuonEvent_hh_

#include "definitions.h"
#include "TDetectorPulse.h"

// X-macro to save writing detector names over and over
#define DETECTOR_LIST \
        X( Ge     ) X( LiquidSc ) X( NDet     ) X( NDet2  ) X( ScGe   ) \
        X( ScL    ) X( ScR      ) X( ScVe     ) X( SiL1_1 ) X( SiL1_2 ) \
        X( SiL1_3 ) X( SiL1_4   ) X( SiL2     ) X( SiR1_1 ) X( SiR1_2 ) \
        X( SiR1_3 ) X( SiR1_4   ) X( SiR1_sum ) X( SiR2   ) X( MuSc   ) \
        X( MuScA  ) 

class TMuonEvent{
    public:
        /// @brief Construct a TMuonEvent
        TMuonEvent(const TDetectorPulse* central_mu):
            fCentralMuon(central_mu){};
        ~TMuonEvent(){}

        /// @brief Reset a TME's list of pulses
        void Reset(){ fPulseLists.clear();}

        /// @brief Retrieve a pulse from the specified source and position in the pulse
        /// list
        const TDetectorPulse* GetPulse(const IDs::source& detector, int index)const;
        /// @brief Add a pulse into the list of pulses
        void AddPulse(const IDs::source& source, TDetectorPulse* pulse);
        void AddPulses(const IDs::source& source,
                DetectorPulseList::const_iterator start,
                DetectorPulseList::const_iterator stop);

        /// @brief Get the n-th source that contains pulses 
        const IDs::source& GetSource(int n)const;
        /// @brief Get the number of sources with pulses in this TME
        int GetNumSources()const{return fPulseLists.size();};

        /// @brief Get the total number of pulses contained in this TME
        int TotalNumPulses()const;
        /// @brief Get the number of pulses contained in this TME for a given
        /// channel
        /// @details If multiple generators are used on the same channel then
        /// this returns the number of pulses from all sources.  See
        /// NumPulses(const IDs::source&)const for an alternative.
        int NumPulses(const IDs::channel& channel)const;
        /// @brief Get the number of pulses contained in this TME for a given
        /// source
        int NumPulses(const IDs::source& source)const;

        /// Does this TME pass the muon hit criteria
        bool HasMuonHit()const;
        /// Does this TME pass the muon pile-up criteria
        bool HasMuonPileup()const;

        bool WasEarlyInEvent()const{return fEarlyInEvent;}
        void WasEarlyInEvent(bool v){fEarlyInEvent=v;};

        bool WasLateInEvent()const{return fLateInEvent;};
        void WasLateInEvent(bool v){fLateInEvent=v;};

        /// Get the time of this TME defined as the arrival time of the central muon
        double GetTime()const {return fCentralMuon->GetTime();}

    private:
            SourceDetPulseMap fPulseLists;
            const TDetectorPulse* fCentralMuon;
            bool fEarlyInEvent;
            bool fLateInEvent;
};
#endif // TMuonEvent_hh_
