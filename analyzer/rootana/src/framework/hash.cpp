#include "hash.h"
#include "iostream"

UInt_t hash_utils::murmurhash2::operator() (const void * key, int len) const
{
  // 'm' and 'r' are mixing constants generated offline.
  // They're not really 'magic', they just happen to work well.
  const UInt_t m = 0x5bd1e995;
  const Int_t r = 24;
  
  // Initialize the hash to a 'random' value
  UInt_t h = fSeed ^ len;
  
  // Mix 4 bytes at a time into the hash
  const Byte_t* data = (const Byte_t*) key;
  
  while(len >= 4){
    UInt_t k = *(UInt_t *)data;
    
    k *= m; 
    k ^= k >> r; 
    k *= m; 
    
    h *= m; 
    h ^= k;
    
    data += 4;
    len -= 4;
  }
  
  // Handle the last few bytes of the input array 
  // fallthrough is intended 
  switch(len) {
  case 3: h ^= data[2] << 16; 
  case 2: h ^= data[1] << 8;
  case 1: h ^= data[0];
    h *= m;
  };
  
  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.
  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;
  
  return h;
}


using hash_utils::MurmurHash3;
//----------------------------------------------------------------------
//Add more data to the hash
void MurmurHash3::Add(MsgPtr_t msg_ptr, Length_t len)
{
  fSize += len;
  
  while(len >= 4) {
    UInt_t k = *(reinterpret_cast<const UInt_t*>(msg_ptr));
    mmix(fHash, k);
    msg_ptr += 4;
    len -= 4;
  }
  
  MixTail(msg_ptr,len);
}


//----------------------------------------------------------------------  
//Before returning add any remaining tail, the message length 
//and mush up the bits a bit more
MurmurHash3::Hash_t MurmurHash3::End()
{
  if (fTail) mmix(fHash, fTail);
  fHash ^= fSize;
  final_mix(fHash);
  return fHash;
}

//----------------------------------------------------------------------
//Mixes in tailing chunks of up to 3 bytes. It accumulates chunks
//untill it has > 4 then mixes the completed set of 4 bytes into the
//hash.
void MurmurHash3::MixTail(MsgPtr_t& data, Length_t& len)
{
  while( len && ((len<4) || fCount)) {
    fTail |= (*data++) << (fCount * 8);
    fCount++;
    len--;
    
    if(fCount == 4){
      mmix(fHash, fTail);
      fTail = 0;
      fCount = 0;
    }
  }
}



