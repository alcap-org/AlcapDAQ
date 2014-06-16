#ifndef IDCHANNEL_H_
#define IDCHANNEL_H_

#include "TObject.h"
#include <map>
#include <string>

/// Namespace defining various identification objects used in the lists of pulses and elsewhere
namespace IDs{
	class channel;

	/// Enum for all possible detectors.  
	/// kAnyDetector will return true when compared to any other value
	/// kErrorDetector is used to flag an error identifying a detector
	enum Detector_t { 
		kErrorDetector=-1,
		kAnyDetector=0,
		kGe, kLiquidSc, kNDet, kNDet2, kScGe, kScL, kScR, 
		kScVe, kSiL1_1, kSiL1_2, kSiL1_3, kSiL1_4, kSiL2, 
		kSiR1_1, kSiR1_2, kSiR1_3, kSiR1_4, kSiR1_sum, kSiR2, 
		kMuSc, kMuScA };
	/// Used by some algorithms to loop over all Detector_t enum values
	const short num_detector_enums=22;

	/// Enum for timing filter types applied to a channel
	/// As for Detector_t, kErrorSlowFast is used to mark an error identifying the type,
	/// whilse kAnySlowFast will match positively agains all other values.
	/// kNotApplicable is used for detectors who don't have a specific filter like the scintillators
	/// or after the TDetectorPulse stage, when all detector pairs have been found.
	enum SlowFast_t{
		kErrorSlowFast=-1,
		kAnySlowFast=0,
		kFast=1,
		kSlow=2,
		kNotApplicable=3 };
}

/// Class to identify a channel.  A channel is one stream of data from one part
/// of a detector, for example: / SiR1_1-F, Ge-F, or ScVe.  Within a run, a
/// channel corresponds to a fixed midas bank, but between runs this mapping may
/// change.
class IDs::channel:public TObject{
   public:
	   /// @brief Construct using a Detector_t and SlowFast_t enum.  Also
	   /// acts as a default constructor, which produces a channel ID that
	   /// matches all other channel IDs.
	   ///
	   /// @param det The detector's name
	   /// @param type The type of timing filter
	channel(Detector_t det=kAnyDetector, SlowFast_t type=kAnySlowFast);

	/// @brief Constructs a channel ID using a pair of strings for the detector and filtering type
	///
	/// @param det The detector's name
	/// @param type The type of timing filter
	channel(const std::string& detector , const std::string& type);

	/// @brief Constructs a channel ID using a string containing both detector
  /// name and the timing filter type
  /// @details This is treated in the same way as operator=(const std::string&)
	/// @param channel The channel represented as a string
	channel(const std::string& channel );
	virtual ~channel(){};

public:
	/// Get the detector enum for this channel
	Detector_t Detector()const{return fDetector;};
	/// Get the SlowFast enum for this channel
	SlowFast_t SlowFast()const{return fSlowFast;};

	/// Set the value of this channel ID using just a single string.
	/// The string can be a complete channel specification, including the SlowFast type
	/// Like the inverse opteration to .str()
	/// Sets kErrorDetector or kErrorSlowFast if there is a problem decoding the string
	channel& operator=(const std::string& rhs);

	/// Set the value of this channel ID using just a single string.
	/// The string can be a complete channel specification, including the SlowFast type
	/// Like the inverse opteration to .str()
	/// Sets kErrorDetector or kErrorSlowFast if there is a problem decoding the string
	channel& operator=(const char* rhs){ return (*this=std::string(rhs));};

	/// Returns true if this channel is the same as another or has it's fields set to 'any'.
	bool operator==(const channel& rhs)const;
	/// Returns true if this channel is not the same as another and it's fields are not set to 'any'.
	bool operator!=(const channel& rhs)const{return !(this->operator==(rhs));};
	
	/// not intuitively meaningful but maybe useful for sorting
	bool operator<(const channel& rhs)const;
	/// not intuitively meaningful but maybe useful for sorting
	bool operator>(const channel& rhs)const;

	/// Returns the channel as a string
	std::string str()const;

	/// Check there there have been no errors creating this channel ID
	bool isValid(){return (fDetector!=kErrorDetector) && (fSlowFast != kErrorSlowFast);};

	/// Convert a Detector_t enum into the corresponding string
	static std::string GetDetectorString(Detector_t det);

	/// Get the Detector_t for a given string.
	/// returns kErrorDetector if the given string doesn't match any known value
	static Detector_t GetDetectorEnum(const std::string& det);

	/// Convert a SlowFast_t enum into the corresponding string
	static std::string GetSlowFastString(SlowFast_t sf);

	/// Get the SlowFast_t for a given string.
	/// Returns kErrorSlowFast if the given string doesn't match any known value
	static SlowFast_t GetSlowFastEnum(const std::string& sf);

	private:
	/// The detector that produced this channel
	Detector_t fDetector;
	/// The type of timing filter used, if any, on this channel
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

inline IDs::channel::channel(const std::string& channel ):fDetector(),fSlowFast(kAnySlowFast){
  *this=channel;
}

ostream& operator<< (ostream& os , IDs::channel& id);

#endif //IDCHANNEL_H_
