#ifndef IDCHANNEL_H_
#define IDCHANNEL_H_

#include "TObject.h"
#include <map>
#include <string>

namespace IDs{
	class channel;
	enum Detector_t { 
		kErrorDetector=-1,
		kAnyDetector=0,
		kGe, kLiquidSc, kNDet, kNDet2, kScGe, kScL, kScR, 
		kScVe, kSiL1_1, kSiL1_2, kSiL1_3, kSiL1_4, kSiL2, 
		kSiR1_1, kSiR1_2, kSiR1_3, kSiR1_4, kSiR1_sum, kSiR2, 
		kMuSc, kMuScA };
	const short num_detector_enums=22;

	enum SlowFast_t{
		kErrorSlowFast=-1,
		kAnySlowFast=0,
		kFast=1,
		kSlow=2,
		kNotApplicable=3 };
}

class IDs::channel{
   public:
	channel(Detector_t det=kAnyDetector, SlowFast_t type=kAnySlowFast);
	channel(const std::string& detector , const std::string& type="");
	virtual ~channel(){};

public:
	Detector_t Detector()const{return fDetector;};
	SlowFast_t SlowFast()const{return fSlowFast;};

	channel& operator=(const std::string& rhs);

	bool operator==(const channel& rhs)const;
	bool operator!=(const channel& rhs)const{return !(this->operator==(rhs));};
	
	// not intuitively meaningful but maybe useful for sorting
	bool operator<(const channel& rhs)const;
	// not intuitively meaningful but maybe useful for sorting
	bool operator>(const channel& rhs)const;

	// Get this ID as a string
	std::string str()const;

	bool isValid(){return (fDetector!=kErrorDetector) && (fSlowFast != kErrorSlowFast);};

	static std::string GetDetectorString(Detector_t det);
	/// Get the Detector_t for a given string returns kAnyDetector if the
	/// named detector is not known
	static Detector_t GetDetectorEnum(const std::string& det);

	static std::string GetSlowFastString(SlowFast_t sf);
	/// Get the Detector_t for a given string returns kAnyDetector if the
	/// named detector is not known
	static SlowFast_t GetSlowFastEnum(const std::string& sf);

	private:
	Detector_t fDetector;
	SlowFast_t fSlowFast;

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
}

inline IDs::channel::channel(const std::string& detector , const std::string& type):fDetector(),fSlowFast(kAnySlowFast){
	fDetector=GetDetectorEnum(detector);
	fSlowFast=GetSlowFastEnum(type);
}

#endif //IDCHANNEL_H_
