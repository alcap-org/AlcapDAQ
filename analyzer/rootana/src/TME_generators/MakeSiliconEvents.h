#ifndef MAKESILICONEVENTS_H_
#define MAKESILICONEVENTS_H_

#include "BaseModule.h"
#include "IdSource.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}

namespace{
namespace LR{
    enum Type{kLeft=0, kRight, kNum};
    const char* str(Type,bool big=false);
    const char* str(int e,bool big=false){return str((Type)e, big);}
}
namespace Ch{
    enum Type{k1_1=0, k1_2, k1_3, k1_4, k2, kNum};
    const char* str(Type);
    const char* str(int e){return str((Type)e);}
}
}

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
  const IDs::source& GetTDPSource(const std::string& ch );

 private:
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData *setup);

  IDs::source fSourceList[LR::kNum][Ch::kNum];
  double fSiWindow;
};

#endif //MAKESILICONEVENTS_H_
