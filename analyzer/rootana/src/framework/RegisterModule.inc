#ifndef REGISTER_MODULE_INC
#define REGISTER_MODULE_INC
#include "ModulesFactory.h"

/// @brief Macro to register a module with the modules::factory.  
/// It needs the name of the class to be added.  Every subsequent argument is
/// treated as the name of the corresponding argument that could be given in
/// the modules list using the constructor approach (see the github wiki for more)
///
/// @param CLASS Name of the module to be registered (don't quote!)
/// @param ... List of all argument names 
#define ALCAP_REGISTER_MODULE( CLASS , ... ) \
RegistryProxy<CLASS,modules::BaseModule,modules::options,modules::factory> p_##CLASS(#CLASS, #__VA_ARGS__);

#endif // REGISTER_MODULE_INC
// vim: set syn=cpp:
