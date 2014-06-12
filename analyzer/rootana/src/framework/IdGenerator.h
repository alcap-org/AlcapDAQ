#ifndef IDGENERATOR_H_
#define IDGENERATOR_H_

#include "TObject.h"
#include <map>
#include <string>

namespace IDs{
	class generator;
	typedef std::string Generator_t;
	typedef std::string Config_t;

	/// Standardize the value to use to represent any generator 
	const Generator_t kAnyGenerator="*";
	/// Standardize the value to use to represent any configuration
	const Config_t kAnyConfig="*";

}

/// Streamable identifier for a generator
class IDs::generator:public TObject{
	public:
	/// @brief Constructor taking two strings for the Generator type and its configuration.
	/// 
	/// @param g The type of the generator
	/// @param c The configuration of the generator

	generator(Generator_t g , Config_t c=kAnyConfig);
	/// @brief Default constructor for a generator ID.
	/// Will match true against all generator IDs
	generator():fType(kAnyGenerator),fConfig(kAnyConfig){};

	virtual ~generator(){};

	public:
	/// Returns the type of generator that this ID represents
	Generator_t Type()const{return fType;};

	/// Returns the configuration of the generator that this ID represents
	Config_t Config()const{return fConfig;};

	/// Is this generator the same as another, or is this or the other id
	/// have fields set to kAny
	bool operator==(const generator& rhs)const;

	/// Is this generator the different to another, and do both this and
	/// the other id have fields not set to kAny
	bool operator!=(const generator& rhs)const{return !(this->operator==(rhs));};
	
	/// Not intuitively meaningful but maybe useful for sorting
	bool operator<(const generator& rhs)const;
	/// Not intuitively meaningful but maybe useful for sorting
	bool operator>(const generator& rhs)const;

	/// Get this ID as a string
	std::string str()const;

	private:
	/// Stores the type of this generator
	Generator_t fType;
	/// Stores the configuration of this generator
	Config_t fConfig;

	ClassDef(IDs::generator,1);
};

inline bool IDs::generator::operator==(const IDs::generator& rhs)const{
	return (fType==kAnyGenerator || rhs.fType==kAnyGenerator || fType==rhs.fType) 
		&& (fConfig==kAnyConfig || rhs.fConfig==kAnyConfig || fConfig==rhs.fConfig) ;
}

inline bool IDs::generator::operator>(const IDs::generator& rhs)const{
	int type_cmp=fType.compare(rhs.fType);
	return  type_cmp>0 || (type_cmp==0 && fConfig.compare(rhs.fConfig)>0);
}

inline bool IDs::generator::operator<(const IDs::generator& rhs)const{
	int type_cmp=fType.compare(rhs.fType);
	return  type_cmp<0 || (type_cmp==0 && fConfig.compare(rhs.fConfig)<0);
}
inline IDs::generator::generator(Generator_t t , Config_t c):
	fType(t),fConfig(c){
		if(fConfig=="") fConfig=kAnyConfig;
}
#endif //IDGENERATOR_H_
