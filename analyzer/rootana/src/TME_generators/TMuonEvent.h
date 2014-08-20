#ifndef TMuonEvent_hh_
#define TMuonEvent_hh_

#include "definitions.h"
#include "TDetectorPulse.h"
#include <set>
#include <vector>

/// @brief Single event in the muon centred tree
/// @author Ben Krikler
///
/// @details
/// @note The early and late event flags are initialised to true.  This should
/// be easier to detect if a TME_generator doesn't fill these fields than if
/// they're defaulted to false.
///
/// @TODO Implement muon pile-up and hit flags
/// @TODO Implement early / late TME flags
///
/// @see www.github.com/alcap-org/AlcapDAQ/issues/110
class TMuonEvent{
    public:
        /// @brief Construct a TMuonEvent with a known central muon
        TMuonEvent(const TDetectorPulse* central_mu, double window):
            fCentralMuon(central_mu),fWindowWidth(window) {};
        /// @brief Default constructor for a TMuonEvent 
        TMuonEvent():
            fCentralMuon(NULL),fWindowWidth(0){};

        /// @brief  Destructor
        /// @details empty since TMuonEvent doesn't own the pulses it contains
        ~TMuonEvent(){}

        /// @brief Reset a TME's list of pulses
        void Reset(){ 
            fPulseLists.clear();
            fExhaustedChannels.clear();
        }

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
        bool WasEarlyInEvent()const;
        /// @brief Get whether or not this TME is flagged as late in an event
        bool WasLateInEvent(double event_length, double event_uncertainty)const;

        /// @brief Set whether all pulses for a certain channel were used up
        void AllPulsesUsed(const IDs::source& so){fExhaustedChannels.insert(so);};
        /// @brief Get whether all pulses for ANY channel were used up
        bool WereNoPulsesLeft()const{return fExhaustedChannels.size()!=0;};
        /// @brief Get whether all pulses for a specific channel were used up
        bool WereNoPulsesLeft(const IDs::source& so)const{
            return fExhaustedChannels.find(so)!=fExhaustedChannels.end();};

        /// Get the time of this TME defined as the arrival time of the central muon
        double GetTime()const {return fCentralMuon->GetTime();}

    private:
            SourceDetPulseMap fPulseLists;
            const TDetectorPulse* fCentralMuon;
            double fWindowWidth;
            typedef std::set<IDs::source> SourceSet;
            SourceSet fExhaustedChannels;
};
#endif // TMuonEvent_hh_
