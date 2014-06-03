#ifndef IDCHANNEL_H_
#define IDCHANNEL_H_

#include "TObject.h"
#include <map>
#include <string>

namespace IDs{
	class channel;
	enum Detector_t { 
		kAnyDetector=0,
		kGe, kLiquidSc, kNDet, kNDet2, kScGe, kScL, kScR, 
		kScVe, kSiL1_1, kSiL1_2, kSiL1_3, kSiL1_4, kSiL2, 
		kSiR1_1, kSiR1_2, kSiR1_3, kSiR1_4, kSiR1_sum, kSiR2, 
		kMuSc, kMuScA };
	const short num_detector_enums=22;

	enum SlowFast_t{
		kAnySlowFast=0,
		kFast=1,
		kSlow=2,
		kNotApplicable=3 };
}

class IDs::channel{
   public:
	channel(Detector_t det=kAnyDetector, SlowFast_t type=kAnySlowFast);
	channel(const std::string& detector , const std::string& type="");
	virtual ~channel(){if(--sNumInstances<0) ClearNames();};

public:
	Detector_t Detector()const{return fDetector;};
	SlowFast_t SlowFast()const{return fSlowFast;};

	bool operator==(const channel& rhs)const;
	bool operator!=(const channel& rhs)const{return !(this->operator==(rhs));};
	
	// not intuitively meaningful but maybe useful for sorting
	bool operator<(const channel& rhs)const;
	// not intuitively meaningful but maybe useful for sorting
	bool operator>(const channel& rhs)const;

	// Get this ID as a string
	std::string str()const;

	// Get the Detector_t for a given string returns -1 if there was a problem
	static std::string GetDetectorName(Detector_t det);
	static Detector_t GetDetectorEnum(const std::string& det);

	private:
	static void InitialiseNames();
	static void ClearNames();
	static void NameDetector(const std::string& name,Detector_t val);

	private:
	Detector_t fDetector;
	SlowFast_t fSlowFast;

	static std::map<std::string, Detector_t> fStringToDetector; //!
	static std::map<Detector_t, std::string> fDetectorToStrings; //!
	static int sNumInstances; //!

	ClassDef(IDs::channel,1);
};

inline bool IDs::channel::operator==(const IDs::channel& rhs)const{
	return (fDetector==kAnyDetector || rhs.fDetector==kAnyDetector || fDetector==rhs.fDetector) 
	&& (fSlowFast==kAnySlowFast || rhs.fSlowFast==kAnySlowFast || fSlowFast==rhs.fSlowFast) ;
}

inline bool IDs::channel::operator>(const IDs::channel& rhs)const{
	return fDetector>rhs.fDetector || (fDetector == rhs.fDetector && fSlowFast>rhs.fSlowFast);
}

inline bool IDs::channel::operator<(const IDs::channel& rhs)const{
	return fDetector<rhs.fDetector || ( fDetector == rhs.fDetector && fSlowFast<rhs.fSlowFast );
}

inline IDs::channel::channel(Detector_t det, SlowFast_t type):fDetector(det),fSlowFast(type){
	sNumInstances++;
	if(fStringToDetector.empty()) InitialiseNames();
}

inline IDs::channel::channel(const std::string& detector , const std::string& type):fDetector(),fSlowFast(kAnySlowFast){
	if(fStringToDetector.empty()) InitialiseNames();
	fDetector=GetDetectorEnum(detector);
	if(type=="_S"|| type=="slow") fSlowFast=kSlow;
	else if(type=="_F"|| type=="fast") fSlowFast=kFast;
	else if(type=="*"|| type=="any") fSlowFast=kAnySlowFast;
	sNumInstances++;
}


inline  IDs::Detector_t IDs::channel::GetDetectorEnum(const std::string& det){
	if(fStringToDetector.empty()) InitialiseNames();
	std::map<std::string ,Detector_t>::iterator it=fStringToDetector.find(det);
	if(it==fStringToDetector.end()) return kAnyDetector;
	return it->second;
}

inline void IDs::channel::NameDetector(const std::string& name,Detector_t val){
	fDetectorToStrings[val]=name;
	fStringToDetector[name]=val;
}

#endif //IDCHANNEL_H_
