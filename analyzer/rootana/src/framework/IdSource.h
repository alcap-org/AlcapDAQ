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

/// @brief Class to define the identity of a source.
/// A source, conceptually speaking, is the combination of a channel and a generator.
/// This then defines where each pulse at every stage of the reconstruction chain has come from.
/// Implemented by containing both a channelID and a generatorID.
class IDs::source:public TObject{
 public:
  /// Default constructor which just invokes default constructors for the
  /// Channel and generator.
  source()
    : fChannel(),fGenerator() {};

  /// @brief Construct a source ID from a single string
  /// @details Equivalent to calling the defualt constructor following by
  /// operator=(std::string)
  source(const std::string& s)
    : fChannel(),fGenerator() {
        operator=(s);
    };
    
  /// Construct using a given channel and generator ID
  source(const IDs::channel& ch,const IDs::generator& gen)
    : fChannel(ch),fGenerator(gen){};
      
  /// Construct using strings for each argument of the contained generator and
  /// channel IDs.
  ///
  /// @param det The name of the detector used in this source
  /// @param type The type of timing filter (Slow / Fast) used in this source
  /// @param gen The type of generator used to analyse the channel
  /// @param cfg The generator's configuration
  source(const std::string& det, const std::string& type,
         const std::string& gen, const std::string& cfg=IDs::kDefaultConfig)
    : fChannel(det,type), fGenerator(gen,cfg) {};

  /// Construct using enums for each argument of the contained generator and
  /// channel IDs.
  ///
  /// @param det The name of the detector used in this source
  /// @param type The type of timing filter (Slow / Fast) used in this source
  /// @param gen The type of generator used to analyse the channel
  /// @param cfg The generator's configuration
  source(Detector_t det, SlowFast_t type,const Generator_t& t ,const Config_t& c=IDs::kDefaultConfig)
  :  fChannel(det,type),fGenerator(t,c){};
  
  virtual ~source(){};

  void Reset(){fChannel.Reset(); fGenerator.Reset();}

  /// Returns true if this source matches another
  bool operator==(const source& rhs)const;
  
  /// Returns true if the generator part of this source matches
  /// another generator
  bool operator==(const generator& rhs)const{return fGenerator==rhs;};
  
  /// Returns true if the channel part of this source matches another channel
  bool operator==(const channel& rhs)const{return fChannel==rhs;};

  /// Returns true if this source matches another
	bool matches(const source& rhs)const;
  /// Returns true if the generator part of this source matches another generator
	bool matches(const generator& rhs)const{return fGenerator.matches(rhs);};
  /// Returns true if the channel part of this source matches another channel
	bool matches(const channel& rhs)const{return fChannel.matches(rhs);};

	/// Not intuitively meaningful but maybe useful for sorting
	bool operator>(const source& rhs)const;
	/// Not intuitively meaningful but maybe useful for sorting
	bool operator<(const source& rhs)const;

  /// Get a reference to the generator ID in this source
  generator& Generator(){return fGenerator;};
  
  /// Get a reference to the channel ID in this source
  channel& Channel(){return fChannel;};
  
  /// Get a const reference to the generator ID in this source
  const generator& Generator()const{return fGenerator;};
  
  /// Get a const reference to the channel ID in this source
  const channel& Channel()const{return fChannel;};

  /// Returns the source as a string
  std::string str(bool cpp_valid=false)const;

  IDs::source& operator=(const std::string& rhs);

  /// Check if the Channel is a wildcard
  bool isWildCardChannel() const {return Channel().isWildCard();}

  /// Check if the Generator is a wildcard
  bool isWildCardGenerator() const {return Generator().isWildCard();}

  /// Check if this source ID is a wildcard (either Generator
  /// or Channel is a wildcard). User must interrogate further
  /// to find out which.
  bool isWildCard() const 
  {return isWildCardChannel() || isWildCardGenerator();}

  /// Check if the Channel is fast
  bool isFast() const {return Channel().isFast();};

  /// Check if the Channel is slow
  bool isSlow() const {return Channel().isSlow();};

  /// Print the individual components of the source
  void Debug()const;

 private:
  channel fChannel;
  generator fGenerator;

  ClassDef(source,1);
};

inline bool IDs::source::operator==(const source& rhs)const
{
  return rhs.Generator()==fGenerator && rhs.Channel()==fChannel;
}
inline bool IDs::source::matches(const source& rhs)const{
	return fGenerator.matches(rhs.fGenerator) && fChannel.matches(rhs.fChannel);
}

inline bool IDs::source::operator>(const source& rhs)const
{
  return fChannel>rhs.Channel() || (fChannel==rhs.Channel() && fGenerator > rhs.Generator());
}

inline bool IDs::source::operator<(const source& rhs)const
{
  return fChannel<rhs.Channel() || (fChannel==rhs.Channel() && fGenerator < rhs.Generator());
}

std::ostream& operator<< (std::ostream& os ,const IDs::source& id);

#endif // IDSOURCE_H_

// emacs metadata
// Local Variables:
// mode: c++
// End:

