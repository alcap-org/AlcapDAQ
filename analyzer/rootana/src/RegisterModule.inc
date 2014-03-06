#include "ModulesManager.h"

#define ALCAP_REGISTER_STRING(CLASS, NAME) \
modules::ModuleBase* Make##CLASS(modules::options* opt) { return new CLASS(opt) ; } \
 \
class proxy##CLASS \
{ \
  public: \
    proxy##CLASS() \
    { \
      modules::manager *f = modules::manager::Instance(); \
      f->registerModule( NAME , &Make##CLASS); \
    } \
} p_##CLASS;

#define ALCAP_REGISTER_MODULE(CLASS) ALCAP_REGISTER_STRING(CLASS, #CLASS)
