#ifndef TVANALYSEDPULSEGENERATOR_H__
#define TVANALYSEDPULSEGENERATOR_H__

#include <vector>
#include <string>
#include <stdexcept>
#include "definitions.h"
#include "TAPGeneratorOptions.h"
#include "TAnalysedPulse.h"
#include "SetupNavigator.h"

#include <TClass.h>

class TPulseIsland;

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana
/// \author Ben Krikler
///
/// \brief
/// The intended ancestor of all TAnalysedPulse generators, used to create
/// TAPs from TPIs.
///
/// \details
/// All TAP generators should be derived from this, and have their
/// ProcessPulses and MayDivideTPIs methods defined.
////////////////////////////////////////////////////////////////////////////////
class TVAnalysedPulseGenerator {

    public:
        TVAnalysedPulseGenerator(const char* name, TAPGeneratorOptions* opts);
        virtual ~TVAnalysedPulseGenerator(){};

    public:
        /// \brief
        /// This method is called on each vector of pulses from a MIDAS event.
        ///
        /// \param[in] pil The vector of TPIs to produce TAPs from.
        /// \param[out] apl The vector of TAPs to append the new TAPs to.
        virtual int ProcessPulses(const PulseIslandList& pil, AnalysedPulseList& apl)=0;
        /// \todo Document this
        virtual bool MayDivideTPIs()=0;

        void SetPulseList(PulseIslandList* list){fPulseList=list;};
        /// \brief
        /// The suggested method for constructing a new TAP.
        ///
        /// \details
        /// In the process of constructing TAP, information that isn't immediately
        /// obvious how to get needs to be passed to TAP. This method returns
        /// a pointer to a TAP construced for you, and all you need to pass it is
        /// the TPulseIslandID.  
        ///
        /// In the case where a generator wants to add extra information to the
        /// standard TAP information, we expect people to derive from TAnalysedPulse.
        /// If your generator is one of these, then use the template argument to
        /// produce the type of TAP you desire.  Note that this forces the constructor
        /// of the specialised TAP to be the same as for TAnalysedPulse itself.
        ///
        /// For example, for to make a specialied analysed pulse called
        /// TSpecialAnalysedPulse (original, huh?) you would do:
        /// \code
        /// TSpecialAnalysedPulse* tsap=MakeNewTAP<TSpecialAnalysedPulse>(parent_index, tap_index);
        /// \endcode
        ///
        /// \param[in] parent_index The TPulseIslandID of the TPI being used
        /// to make the new TAP.
        /// \tparam[in] TypeOfTAP The type of specialisation of TAnalysedPulse that
        /// you want to create.
        template <typename TypeOfTAP>
	  TypeOfTAP* MakeNewTAP(int parent_index, int tap_index)const;

        TAnalysedPulse* MakeNewTAP(int parent_index, int tap_index)const{
	  return MakeNewTAP<TAnalysedPulse>(parent_index, tap_index);
        }

        bool Debug()const{return fDebug;};

        /// A convenience method for analysis to get the channel ID of the channel
        /// being analysed
        IDs::channel GetChannel()const {return fSource.Channel();};

        /// A convenience method for analysis to get the channel ID of the channel
        /// being analysed
        std::string GetBank()const {return fBankName;};

        /// \brief Get the source id for this generator being used on the current
        /// channel
        ///
        /// \details Useful for a generator to know the full source much like in the
        /// above GetChannel method, but this could also be useful for checking that
        /// this generator is the one that made a given TAP in later analysis by
        /// comparing this value to that stored in the TAP itself.
        const IDs::source& GetSource()const {return fSource;};

        static const char* TapType(){return TAnalysedPulse::Class()->GetName();}

        virtual void CalibratePulses( AnalysedPulseList& theAnalysedPulses)const;

    protected:
        friend class MakeAnalysedPulses;

        /// \brief Set the channel for this generator. Should NOT be called by user
        /// code
        /// 
        /// \details Called by MakeAnalysedPulses to tell this generator what channel
        /// it is looking at.  
        virtual void SetChannel(const std::string& det){
           fSource.Channel()=det;
           fBankName= SetupNavigator::Instance()->GetBank(det);
         };


    private:
        /// \brief
        /// The source identifying the generator and detector being
        /// processed.
        IDs::source fSource;
        /// \brief
        /// The bankname in the MIDAS file used by this channel
        std::string fBankName;
        /// \brief
        /// Debug flagged set from modules file. Used, for example, in
        /// deciding whether or not to print helpful messages.
        bool fDebug;
        /// \brief
        /// The vector of TPIs being processed.
        PulseIslandList* fPulseList;

        /// Flag if we have calibration constants for this generator
        bool fCanCalibrate;
        /// Cache the calibration constants so we don't need to look up in the setup navigator each time
        double fAdcToEnergyGain, fAdcToEnergyOffset;
};

template <typename TypeOfTAP>
inline TypeOfTAP* TVAnalysedPulseGenerator::MakeNewTAP(int parent_index, int tap_index)const{
    TypeOfTAP* pulse=NULL;
    TPulseIsland* parent = fPulseList->at(parent_index);
    pulse=new TypeOfTAP(GetSource(),parent_index,tap_index,parent);
    return pulse;
}

#endif // TVANALYSEDPULSEGENERATOR_H__
