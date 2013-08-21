#ifdef __CINT__

#include <vector>
#include <string>
#include <map>

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class TGlobalData+;
#pragma link C++ class TOctalFADCIsland+;
#pragma link C++ class TPulseIsland+;
#pragma link C++ class map<string, vector<TPulseIsland*> >+;
#pragma link C++ map<string, vector<TPulseIsland*> >::iterator;
#pragma link C++ class pair<string,vector<TPulseIsland*> >+;
#endif
