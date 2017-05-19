#pragma once

#include "BaseModule.h"
class TGlobalData;
class TSetupData;

#include <map>
#include <string>

namespace modules {class options;}

#include "TH1.h"

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author John R Quirk
///
/// \brief
/// Plot time bits in time stamp.
///
/// \details
/// Look at the distribution of of bits in the trigger timestamp to look for
/// stuck bits.
////////////////////////////////////////////////////////////////////////////////
class PlotTPI_TimeBits : public BaseModule {

 public:
  /// \brief
  /// The constructor. Takes standard arguments but does nothing.
  ///
  /// \param[in] opts Not used.
  PlotTPI_TimeBits(modules::options* opts);
  /// \brief
  /// Empty destructor
  ~PlotTPI_TimeBits();

 private:
  /// \param[in] gData See BaseModule::ProcessEntry
  /// \param[in] gSetup See BaseModule::ProcessEntry
  /// \return Non-zero to indicate a problem.
  virtual int ProcessEntry(TGlobalData* gData, const TSetupData* gSetup);

  /// \brief
  /// Nothing is done before each run
  ///
  /// \param[in] gData See BaseModule::BeforeFirstEntry
  /// \param[in] setup See BaseModule::BeforeFirstEntry
  /// \return Non-zero to indicate a problem.
  virtual int BeforeFirstEntry(TGlobalData* gData, const TSetupData* setup);

  /// \brief
  /// Book a histogram.
  /// \param[in] bank Bank name.
  void BookHistogram(const std::string& bank);

  /// \brief
  /// Empty.
  ///
  /// \param[in] gData See BaseModule::AfterLastEntry
  /// \param[in] setup See BaseModule::AfterLastEntry
  /// \return Non-zero to indicate a problem.
  virtual int AfterLastEntry(TGlobalData* gData, const TSetupData* setup);

  /// \brief
  /// The map that we store the time bits. First element is all, second is
  /// with sync cut, second is those excluded by sync cut.
  std::map<std::string, TH1*> fHists[3];
};
