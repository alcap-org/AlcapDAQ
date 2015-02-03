#ifndef TME_REMOVESCATTEREDMUONS_H_
#define TME_REMOVESCATTEREDMUONS_H_

#include "BaseModule.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}

#include "IdChannel.h"

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author Andrew Edmonds
///
/// \brief
/// Removes any TMEs that have scattered muons in them
///
/// \details
/// Loops through the TME container and removes any hits in the Si detectors 
/// that are too close to the central muon hit)
////////////////////////////////////////////////////////////////////////////////
class TME_RemoveScatteredMuons : public BaseModule {

 public:
  /// \brief
  /// Constructor.
  ///
  /// \param[in] time_cut The time cut used to remove the scattered muons
  TME_RemoveScatteredMuons(modules::options* opts);
  /// \brief
  /// Destructor.
  ~TME_RemoveScatteredMuons();

 private:
  /// \brief
  /// In every entry, we check for any Si hits that are too close to the central muon hit
  /// 
  /// \return Non-zero to indicate a problem.
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);
  /// \brief
  /// Before each run, create a list of silicon detectors
  ///
  /// \return Non-zero to indicate a problem.
  virtual int BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup);
  /// \brief
  /// Nothing is done after each run
  ///
  /// \return Non-zero to indicate a problem.
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData *setup);

  /// \brief The time cut being used to remove scattered muons
  ///
  /// \details If the time difference between the central muon and any Si detector 
  /// hit is shorter than this, then the TME is deleted.
  double fTimeCut;

  /// \brief All the silicon detectors
  typedef std::vector<IDs::channel> DetectorList;
  DetectorList fSiDetectors;

};

#endif //TME_REMOVESCATTEREDMUONS_H_
