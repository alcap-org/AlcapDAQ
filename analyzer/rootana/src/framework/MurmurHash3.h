#ifndef MURMURHASH3_H
#define MURMURHASH3_H

#include "Rtypes.h"
#include <iostream>
namespace hash {
  class MurmurHash3;

  ///A raw data steam pointer for passing into a hash
  typedef const Byte_t* MsgPtr_t;

  template<typename T>
  MsgPtr_t MsgPtr(T* ptr){ 
    return reinterpret_cast<MsgPtr_t>(ptr);};
  
  /*  MsgPtr_t MsgPtr(void* ptr){
    std::cout << "void*" << std::endl;
    return reinterpret_cast<MsgPtr_t>(ptr);};
  */
  template<typename T> MsgPtr_t MsgPtr(const T& thing) 
  {
    return reinterpret_cast<MsgPtr_t>(&thing);};
};

class hash::MurmurHash3 {
public:
  enum {kDefaultSeed = 0xe086c5ff};

  typedef UInt_t Hash_t;
  typedef Int_t Length_t;
  typedef Byte_t Rot_t; 

  MurmurHash3(unsigned int seed = kDefaultSeed)
    : fHash(seed), fTail(0), fCount(0), fSize(0)
  {}
  
  inline void Reset(UInt_t seed = kDefaultSeed) 
  {
    fHash = seed;
    fTail = fCount = fSize = 0;
  }

  //MurmurHash3& 
  void Add(MsgPtr_t msg_ptr, Length_t len);
  
  template<typename T> //inline 
  MurmurHash3& Add(T* ptr, Length_t len) 
  {
    Add(reinterpret_cast<MsgPtr_t>(ptr), len);
    return *this;
  }

  template<typename T> //inline
  MurmurHash3& Add(const T& thing, Length_t len)
  {
    Add(reinterpret_cast<MsgPtr_t>(&thing), len);
    return *this;
  }

  Hash_t Value() const;

  Hash_t End(UInt_t seed = kDefaultSeed);

private:  

  inline Hash_t& EndImp(Hash_t& hash, UInt_t& tail) const;

  void MixTail (MsgPtr_t& data, Length_t& len);

  inline UInt_t rot32(UInt_t bits, Rot_t r) const
  {
    return (bits << r) | (bits >> (32 - r));
  }
  
  inline void mmix(Hash_t& h, UInt_t& bits) const
  { 
    bits *= kMply1;   bits = rot32(bits, 15);   bits *= kMply2;
    h ^= bits;   h = rot32(h, 13) * 5 + 0xe6546b64;
  }

  inline void final_mix(Hash_t& h) const
  {
    h ^= h >> 16;   h *= 0x85ebca6b;
    h ^= h >> 13;   h *= 0xc2b2ae35;
    h ^= h >> 16;
  }

  //These  2 constants are kept seperate in the original, but i'm not
  //really sure why posibly these were used for tuning
  static const UInt_t kMply1 = 0xcc9e2d51;
  static const UInt_t kMply2 = 0x1b873593;

  //static const UShort_t kRot1 = 15;
  //static const UShort_t kRot2 = 13;
  //static const UInt_t kFinalM = 5;
  //static const UInt_t kFinalN = 0xe6546b64;


  Hash_t fHash;
  UInt_t fTail;
  UInt_t fCount;
  UInt_t fSize;
};

#endif //MURMURHASH3_H;
