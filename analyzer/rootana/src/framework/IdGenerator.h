#ifndef IDGENERATOR_H_
#define IDGENERATOR_H_

#include "TObject.h"
#include <map>
#include <string>

namespace IDs{
	class generator;
	typedef std::string Generator_t;
	typedef std::string Config_t;

	// String to use to represent any type or config
	const Generator_t kAnyGenerator="*";
	const Config_t kAnyConfig="*";

}

class IDs::generator:public TObject{
	public:
	generator(Generator_t t , Config_t c="");
	generator():fType(kAnyGenerator),fConfig(kAnyConfig){};
	virtual ~generator(){};

	public:
	Generator_t Type()const{return fType;};
	Config_t Config()const{return fConfig;};

	bool operator==(const generator& rhs)const;
	bool operator!=(const generator& rhs)const{return !(this->operator==(rhs));};
	
	// not intuitively meaningful but maybe useful for sorting
	bool operator<(const generator& rhs)const;
	// not intuitively meaningful but maybe useful for sorting
	bool operator>(const generator& rhs)const;

	// Get this ID as a string
	std::string str()const;

	private:
	Generator_t fType;
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
