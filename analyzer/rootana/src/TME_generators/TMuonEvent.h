#ifndef TMuonEvent_hh_
#define TMuonEvent_hh_

#include "definitions.h"
#include "TDetectorPulse.h"

/// @brief Single event in the muon centred tree
/// @author Ben Krikler
///
/// @details
/// @note The early and late event flags are initialised to true.  This should
/// be easier to detect if a TME_generator doesn't fill these fields than if
/// they're defaulted to false.
///
/// @see www.github.com/alcap-org/AlcapDAQ/issues/110
class TMuonEvent{
    public:
        /// @brief Construct a TMuonEvent with a known central muon
        TMuonEvent(const TDetectorPulse* central_mu):
            fCentralMuon(central_mu),fEarlyInEvent(true),fLateInEvent(true){};
        /// @brief Default constructor for a TMuonEvent 
        TMuonEvent():
            fCentralMuon(NULL),fEarlyInEvent(true),fLateInEvent(true){};

        /// @brief  Destructor
        /// @details empty since TMuonEvent doesn't own the pulses it contains
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

        /// @brief Does this TME pass the muon hit criteria
        bool HasMuonHit()const;
        /// @brief Does this TME pass the muon pile-up criteria
        bool HasMuonPileup()const;

        /// @brief Get whether or not this TME is flagged as early in an event
        bool WasEarlyInEvent()const{return fEarlyInEvent;}
        /// @brief Set whether or not this TME is flagged as early in an event
        void WasEarlyInEvent(bool v){fEarlyInEvent=v;};

        /// @brief Get whether or not this TME is flagged as late in an event
        bool WasLateInEvent()const{return fLateInEvent;};
        /// @brief Set whether or not this TME is flagged as late in an event
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
