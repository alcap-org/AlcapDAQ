#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;

#pragma link C++ class IDs::channel+;
#pragma link C++ class IDs::generator+;
#pragma link C++ class IDs::source+;
#pragma link C++ enum IDs::Detector_t+;
#pragma link C++ class std::map<std::string,IDs::Detector_t>+;
#pragma link C++ class std::map<IDs::Detector_t,std::string>+;

#pragma link C++ class TAnalysedPulse+;
#pragma link C++ class TGaussFitAnalysedPulse+;
#pragma link C++ class TDetectorPulse+;
#pragma link C++ class vector<TAnalysedPulse*>+;
#pragma link C++ class vector<TAnalysedPulse*>::iterator;
#pragma link C++ function operator!= (vector<TAnalysedPulse*>::iterator, vector<TAnalysedPulse*>::iterator);
//#pragma link C++ class map<IDs::source, vector<TAnalysedPulse*> >+;
//#pragma link C++ class map<IDs::source, vector<TAnalysedPulse*> >::iterator+;
//#pragma link C++ class map<IDs::source, vector<TAnalysedPulse*> >::const_iterator+;
//#pragma link C++ class map<IDs::source, vector<TDetectorPulse*> >+;
//#pragma link C++ class map<IDs::source, vector<TDetectorPulse*> >::iterator+;
//#pragma link C++ class map<IDs::source, vector<TDetectorPulse*> >::const_iterator+;
//#pragma link C++ class map<IDs::source, vector<TMuonEvent*> >+;
//#pragma link C++ class map<IDs::source, vector<TMuonEvent*> >::iterator+;
//#pragma link C++ class map<IDs::source, vector<TMuonEvent*> >::const_iterator+;
#pragma link C++ class map<string, vector<TAnalysedPulse*> >+;
#pragma link C++ class map<string, vector<TAnalysedPulse*> >::iterator;
#pragma link C++ class pair<string,vector<TAnalysedPulse*> >+;
//#pragma link C++ class pair<IDs::source,vector<TAnalysedPulse*> >+;
//#pragma link C++ class pair<IDs::source,vector<TDetectorPulse*> >+;
//#pragma link C++ class pair<IDs::source,vector<TMuonEvent*> >+;
#pragma link C++ class std::vector<std::string>+;

#pragma link C++ class std::map<IDs::source, int>+;
#pragma link C++ class std::pair<IDs::source, int> +;
#pragma link C++ class std::vector<IDs::source>+;
#pragma link C++ class FlyWeightLists<IDs::source, TAnalysedPulse::Tag>+;
#pragma link C++ class FlyWeightLists<IDs::source, TDetectorPulse::Tag>+;
#pragma link C++ class FlyWeight<IDs::source, TAnalysedPulse::Tag>+;
#pragma link C++ class FlyWeight<IDs::source, TDetectorPulse::Tag>+;

//#pragma link C++ class vector<TPulseIsland*>+;
#pragma link C++ class BankBranch<vector<TPulseIsland*> >+;
#pragma link C++ class BankBranch<vector<TAnalysedPulse*> >+;
#pragma link C++ class BankBranch<vector<TDetectorPulse*> >+;
#pragma link C++ class BankBranch<vector<TMuonEvent*> >+;

#pragma link C++ class TAnalysedPulseMapWrapper+;
#endif    /* __CINT__ */
