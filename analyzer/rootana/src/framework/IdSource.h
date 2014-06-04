#ifndef IDSOURCE_H_
#define IDSOURCE_H_

#include "TObject.h"
#include <map>
#include <string>
#include "IdChannel.h"
#include "IdGenerator.h"

namespace IDs{
	class source;
//	class generator;
//	class channel;
}

class IDs::source:public TObject{
	public:
	source(){};
	source(const IDs::channel& ch,const IDs::generator& gen):fChannel(ch),fGenerator(gen){};
	source(const std::string& det, const std::string& type,
			const std::string& gen, const std::string& cfg=IDs::kAnyConfig):
		fChannel(det,type),fGenerator(gen,cfg){};
	source(Detector_t det, SlowFast_t type,const Generator_t& t ,const Config_t& c):
		fChannel(det,type),fGenerator(t,c){};
	virtual ~source(){};

	bool operator==(const source& rhs)const;
	bool operator==(const generator& rhs)const{return fGenerator==rhs;};
	bool operator==(const channel& rhs)const{return fChannel==rhs;};

	bool operator>(const source& rhs)const;
	bool operator<(const source& rhs)const;

	generator& Generator(){return fGenerator;};
	channel& Channel(){return fChannel;};
	const generator& Generator()const{return fGenerator;};
	const channel& Channel()const{return fChannel;};

	std::string str()const;

	private:
	channel fChannel;
	generator fGenerator;

	ClassDef(IDs::source,1);
};

inline bool IDs::source::operator==(const source& rhs)const{
	return rhs.Generator()==fGenerator && rhs.Channel()==fChannel;
}

inline bool IDs::source::operator>(const source& rhs)const{
	return fChannel>rhs.Channel() || (fChannel==rhs.Channel() && fGenerator > rhs.Generator());
}

inline bool IDs::source::operator<(const source& rhs)const{
	return fChannel<rhs.Channel() || (fChannel==rhs.Channel() && fGenerator < rhs.Generator());
}

#endif // IDSOURCE_H_
