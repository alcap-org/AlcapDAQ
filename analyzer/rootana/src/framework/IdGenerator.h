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
	const Generator_t kAnyGenerator="any";
	/// Standardize the value to use to represent any configuration
	const Config_t kAnyConfig="any";
	/// Standardize the value to use to represent the default configuration of a
    /// module
	const Config_t kDefaultConfig="default";

    const std::string field_separator="#";
}

/// Streamable identifier for an instance of a generator
class IDs::generator:public TObject{
	public:
    /// @brief Constructor taking two strings for the Generator type and its
    /// configuration.
	/// 
	/// @param g The type of the generator
	/// @param c The configuration of the generator
	generator(Generator_t g , Config_t c);

	/// @brief Default constructor for a generator ID.
	/// Will match true against all generator IDs
	generator():fType(kAnyGenerator),fConfig(kAnyConfig){};

    /// @brief Construct a source ID from a single string
    /// @details Equivalent to calling the defualt constructor following by
    /// operator=(std::string)
    generator(const std::string& s)
        : fType(kAnyGenerator),fConfig(kAnyConfig) {
            operator=(s);
        };

	virtual ~generator(){};

        void Reset(){fType=kAnyGenerator; fConfig=kAnyConfig;};

	public:
	/// Returns the type of generator that this ID represents
	Generator_t Type()const{return fType;};

    /// Set the type of generator that this ID represents
	void Type(const Generator_t& g){ fType=g;};

	/// Returns the configuration of the generator that this ID represents
	Config_t Config()const{return fConfig;};

    /// Set the configuration of the generator that this ID represents
	void Config(const Config_t& c){fConfig=c;};

	/// Is this generator the same as another, or is this or the other id
	/// have fields set to kAny
	bool operator==(const generator& rhs)const;
	bool matches(const generator& rhs)const;

	/// Is this generator the different to another, and do both this and
	/// the other id have fields not set to kAny
	bool operator!=(const generator& rhs)const{return !(this->operator==(rhs));};
	
	/// Not intuitively meaningful but maybe useful for sorting
	bool operator<(const generator& rhs)const;
	/// Not intuitively meaningful but maybe useful for sorting
	bool operator>(const generator& rhs)const;

	/// Get this ID as a string
	std::string str()const;

    IDs::generator& operator=(const std::string& rhs);

    /// Check if the Generator_t part of the ID is a wildcard
    bool isWildCardType() const { return fType == kAnyGenerator; }

    /// Check if the Config_t part of the ID is a wildcard
    bool isWildCardConfig() const { return fConfig == kAnyConfig; }

    /// Check if any part of the ID is a wildcard
    bool isWildCard() const { return isWildCardConfig() || isWildCardType(); }

    /// Print the individual components of the ID
    void Debug()const;

	private:
	/// Stores the type of this generator
	Generator_t fType;
	/// Stores the configuration of this generator
	Config_t fConfig;

	ClassDef(generator,1);
};

inline bool IDs::generator::operator==(const IDs::generator& rhs)const{
	return (fType==rhs.fType) && (fConfig==rhs.fConfig) ;
}

inline bool IDs::generator::matches(const generator& rhs)const{
	return (isWildCardType() || rhs.isWildCardType() || fType==rhs.fType) 
		&& (isWildCardConfig() || rhs.isWildCardConfig() || fConfig==rhs.fConfig) ;
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

std::ostream& operator<< (std::ostream& os ,const IDs::generator& id) ;

#endif //IDGENERATOR_H_

// emacs metadata
// Local Variables:
// mode: c++
// End:
