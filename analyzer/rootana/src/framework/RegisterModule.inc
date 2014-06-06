#ifndef REGISTER_MODULE_INC
#define REGISTER_MODULE_INC
#include "ModulesFactory.h"

#define ALCAP_REGISTER_MODULE( CLASS , ... ) \
RegistryProxy<CLASS,modules::BaseModule,modules::options,modules::factory> p_##CLASS(#CLASS, #__VA_ARGS__);

#endif // REGISTER_MODULE_INC
