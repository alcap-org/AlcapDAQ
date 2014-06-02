#pragma link C++ class IDs::channel+;
#pragma link C++ class IDs::generator+;
#pragma link C++ enum IDs::Detector_t+;
#pragma link C++ class std::map<std::string,IDs::Detector_t>+;
#pragma link C++ class std::map<IDs::Detector_t,std::string>+;

#pragma link C++ class TAnalysedPulse+;
#pragma link C++ class TDetectorPulse+;
#pragma link C++ class vector<TAnalysedPulse*>+;
#pragma link C++ class vector<TAnalysedPulse*>::iterator;
#pragma link C++ function operator!= (vector<TAnalysedPulse*>::iterator, vector<TAnalysedPulse*>::iterator);
#pragma link C++ class map<string, vector<TAnalysedPulse*> >+;
#pragma link C++ class map<string, vector<TAnalysedPulse*> >::iterator;
#pragma link C++ class pair<string,vector<TAnalysedPulse*> >+;
#pragma link C++ class std::vector<std::string>+;

#pragma link C++ class TAnalysedPulseMapWrapper+;
