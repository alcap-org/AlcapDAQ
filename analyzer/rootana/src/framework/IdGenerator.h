#ifndef IDGENERATOR_H_
#define IDGENERATOR_H_

#include "TObject.h"
#include <map>
#include <string>

namespace IDs{
	class generator;
	typedef std::string Type_t;
	typedef std::string Config_t;
}

class IDs::generator{
   public:
	generator(Type_t t , Config_t c):fType(t),fConfig(c){};
	~generator(){};

public:
	Type_t Type()const{return fType;};
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
	Type_t fType;
	Config_t fConfig;
	static const Type_t fAnyType;
	static const Config_t fAnyConfig;

	ClassDef(IDs::generator,1);
};

inline bool IDs::generator::operator==(const IDs::generator& rhs)const{
	return (fType==fAnyType || rhs.fType==fAnyType || fType==rhs.fType) 
		&& (fConfig==fAnyConfig || rhs.fConfig==fAnyConfig || fConfig==rhs.fConfig) ;
}

inline bool IDs::generator::operator>(const IDs::generator& rhs)const{
	int type_cmp=fType.compare(rhs.fType);
	return  type_cmp>0 || (type_cmp==0 && fConfig.compare(rhs.fConfig)>0);
}

inline bool IDs::generator::operator<(const IDs::generator& rhs)const{
	int type_cmp=fType.compare(rhs.fType);
	return  type_cmp<0 || (type_cmp==0 && fConfig.compare(rhs.fConfig)<0);
}
#endif //IDGENERATOR_H_
