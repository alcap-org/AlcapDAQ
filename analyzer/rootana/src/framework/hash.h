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
  
  class MurmurHash2;


};

class hash_utils::MurmurHash2{
public:
  typedef const Byte_t* MsgPtr_t;
  typedef UInt_t Hash_t;
  typedef Int_t Length_t;

  MurmurHash2(unsigned int seed = 0xe086c5ff)
    : fHash(seed), fTail(0), fCount(0), fSize(0)
  {}
    
  void Add(MsgPtr_t msg_ptr, Length_t len);
  Hash_t End();

private:
  void MixTail (MsgPtr_t& data, Length_t& len);
  inline void mmix(Hash_t& h, UInt_t& a) 
  { 
    a *= gkMply;  a ^= a >> gkRot;  a *= gkMply; 
    h *= gkMply;  h ^= a;
  }

  static const UInt_t gkMply = 0x5bd1e995;
  static const Int_t gkRot = 24;

  Hash_t fHash;
  UInt_t fTail;
  UInt_t fCount;
  UInt_t fSize;
};

#endif //HASH_AC_H
