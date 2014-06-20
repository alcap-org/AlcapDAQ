#ifndef TVANALYSEDPULSEGENERATOR_H__
#define TVANALYSEDPULSEGENERATOR_H__

#include <vector>
#include <string>
#include <stdexcept>
#include "definitions.h"
#include "TAPGeneratorOptions.h"
#include "TAnalysedPulse.h"

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
  TVAnalysedPulseGenerator(const char* name, TAPGeneratorOptions* opts){
    if(opts){
      fDebug=opts->HasOption("debug");
    }
    fSource.Generator()=IDs::generator(name);//,opts->ToOneString());
  };
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
  /// obvious how to get needs to be passed to TAP. Luckily, this method returns
  /// a poointer to a TAP construced for you, and all you need to pass it is
  /// the TPulseIslandID.
  ///
  /// \param[in] parent_index The TPulseIslandID of the TPI being used
  /// to make the new TAP.
  TAnalysedPulse* MakeNewTAP(int parent_index)const;

  bool Debug()const{return fDebug;};

  /// \brief
  /// The generator of fSource does not change during processing,
  /// however the channel does. This is how we update the generator,
  /// letting it know what's being processed.
  ///
  /// \todo Verify this
  void SetChannel(const std::string& det){fSource.Channel()=det;};
  /// \todo What is this for?
  IDs::channel GetChannel()const {return fSource.Channel();};
  /// \todo What is this for since we have access to fSource?
  IDs::source GetSource()const {return fSource;};

 private:
  /// \brief
  /// The source identifying the generator and detector being
  /// processed.
  IDs::source fSource;
  /// \brief
  /// Debug flagged set from modules file. Used, for example, in
  /// deciding whether or not to print helpful messages.
  bool fDebug;
  /// \brief
  /// The vector of TPIs being processed.
  PulseIslandList* fPulseList;
};

inline TAnalysedPulse* TVAnalysedPulseGenerator::MakeNewTAP(int parent_index)const{
  TAnalysedPulse* pulse=NULL;
  try{
      TPulseIsland* parent = fPulseList->at(parent_index);
      pulse=new TAnalysedPulse(GetSource(),parent_index,parent);
  } catch (std::out_of_range){}
  return pulse;
}

#endif // TVANALYSEDPULSEGENERATOR_H__
