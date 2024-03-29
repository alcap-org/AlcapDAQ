#ifdef __CINT__

#include <vector>
#include <string>
#include <map>

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class TGlobalData+;

#pragma link C++ class TPulseIsland+;
#pragma link C++ class vector<TPulseIsland*>+;
#pragma link C++ class vector<TPulseIsland*>::iterator;
#pragma link C++ function operator!= (vector<TPulseIsland*>::iterator, vector<TPulseIsland*>::iterator);
#pragma link C++ class map<string, vector<TPulseIsland*> >+;
#pragma link C++ class map<string, vector<TPulseIsland*> >::iterator;
#pragma link C++ class pair<string,vector<TPulseIsland*> >+;

#pragma link C++ class TSetupData+;

#pragma link C++ class map<string, string>+;
#pragma link C++ class map<string, string>::iterator;
#pragma link C++ class pair<string, string>+;

#pragma link C++ class TVacuumData+;

#pragma link C++ class TMuPCCluster+;
#endif
