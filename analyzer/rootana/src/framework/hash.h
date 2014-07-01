#ifndef HASH_AC_H
#define HASH_AC_H

#include "Rtypes.h"
//ROOT is still our most convenient source of fixed width types

namespace hash_utils{  
  /// Functor copy of murmurhash2 (Public Domain / MIT liscence) by Austin Appleby
  /// If it's good enoiugh for gcc...
  struct murmurhash2 {
    murmurhash2(UInt_t seed) : fSeed(seed) {};
    UInt_t operator()(const void* key, int len) const;
    const UInt_t fSeed;
  };
  
  inline UInt_t void_to_int(const void* key, int length)
  {
    static murmurhash2 pet(0xe086c5ff);
    return pet(key, length);
  }
  
  class MurmurHash3;


};

class hash_utils::MurmurHash3{
public:
  typedef const Byte_t* MsgPtr_t;
  typedef UInt_t Hash_t;
  typedef Int_t Length_t;
  typedef Byte_t Rot_t; 

  MurmurHash3(unsigned int seed = 0xe086c5ff)
    : fHash(seed), fTail(0), fCount(0), fSize(0)
  {}
    
  void Add(MsgPtr_t msg_ptr, Length_t len);
  Hash_t End();

private:
  void MixTail (MsgPtr_t& data, Length_t& len);

  inline UInt_t rot32(UInt_t bits, Rot_t r)
  {
    return (bits << r) | (bits >> (32 - r));
  }
  
  inline void mmix(Hash_t& h, UInt_t& bits)
  { 
    bits *= kMply1;   bits = rot32(bits, 15);   bits *= kMply2;
    h ^= bits;   h = rot32(h, 13) * 5 + 0xe6546b64;
  }

  inline void final_mix(Hash_t& h)
  {
    h ^= h >> 16;   h *= 0x85ebca6b;
    h ^= h >> 13;   h *= 0xc2b2ae35;
    h ^= h >> 16;
  }

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

#endif //HASH_AC_H
