#include "TGlobalData.h"

ClassImp(TGlobalData)

#ifdef ROOT_VERSION
TGlobalData* TGlobalData::Instance() {
  return NULL;
}
#endif
