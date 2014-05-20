#ifndef REGISTER_MODULE_INC
#define REGISTER_MODULE_INC
#include "ModulesFactory.h"

#define ALCAP_REGISTER_STRING(CLASS, NAME, ...) \
modules::ModuleBase* Make##CLASS(modules::options* opt) { return new CLASS(opt) ; } \
 \
class proxy##CLASS \
{ \
  public: \
    proxy##CLASS() \
    { \
      modules::factory *f = modules::factory::Instance(); \
      f->registerModule( NAME , &Make##CLASS); \
      /* Add all the arguments to the manager */\
      f->addArguments( #__VA_ARGS__);\
    } \
} p_##CLASS;

#define ALCAP_REGISTER_MODULE( CLASS , ... ) ALCAP_REGISTER_STRING(CLASS, #CLASS, ##__VA_ARGS__ )

#endif // REGISTER_MODULE_INC
