#ifndef TME_SIARRIVALTIMES_H_
#define TME_SIARRIVALTIMES_H_

#include "BaseModule.h"
#include "IdChannel.h"

class TGlobalData;
class TSetupData;
namespace modules {class options;}

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author Andrew Edmonds
///
/// \brief
/// Plots the arrival time of hits in the silicon detectors
///
/// \details
/// This can be used to see if there's a peak from lead emissions or scattered muons
////////////////////////////////////////////////////////////////////////////////
class TME_SiArrivalTimes : public BaseModule {

 public:
  /// \brief
  /// Constructor
  ///
  /// \param[in] opts Takes not options
  TME_SiArrivalTimes(modules::options* opts);
  /// \brief
  /// Destructor
  ~TME_SiArrivalTimes();

 private:
  /// \brief
  /// Histograms are filled in every entry
  /// 
  /// \return Non-zero to indicate a problem.
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);

  /// \brief
  /// Histograms are created at the start of each run
  ///
  /// \return Non-zero to indicate a problem.
  virtual int BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup);
  /// \brief
  /// Nothing is done at the end of each run
  ///
  /// \return Non-zero to indicate a problem.
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData *setup);

  /// \brief
  /// The list of silicon detectors
  typedef std::vector<IDs::channel> DetectorList;
  DetectorList fSiDetectors;

};

#endif //TME_SIARRIVALTIMES_H_
