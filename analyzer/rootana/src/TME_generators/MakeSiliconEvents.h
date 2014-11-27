#ifndef MAKESILICONEVENTS_H_
#define MAKESILICONEVENTS_H_

#include "BaseModule.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author AuthorName
///
/// \brief
/// A one line description of what your module does.
///
/// \details
/// A longer, more descriptive block of text.
/// Specifics like members and methods will be described later.
/// You can add this to other groups instead of rootana_modules or in addition
/// to rootana_modules by adding more of the ingroup tags.
////////////////////////////////////////////////////////////////////////////////
class MakeSiliconEvents : public BaseModule {

 public:
  /// \brief
  /// Constructor description. If necessary, add a details tag like above.
  ///
  /// \param[in] opts Describe the options this module takes.
  MakeSiliconEvents(modules::options* opts);
  /// \brief
  /// Is anything done in the destructor?
  ~MakeSiliconEvents();

 private:
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData *setup);

  int fXMax;
};

#endif //MAKESILICONEVENTS_H_
