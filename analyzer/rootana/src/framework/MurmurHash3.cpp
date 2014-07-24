#include "MurmurHash3.h"

using hash::MurmurHash3;

//----------------------------------------------------------------------
//Add more data to the hash
void MurmurHash3::Add(MsgPtr_t msg_ptr, Length_t len)
{
  fSize += len;
  MixTail(msg_ptr,len);
  
  while(len >= 4) {
    UInt_t k = *(reinterpret_cast<const UInt_t*>(msg_ptr));
    mmix(fHash, k);
    msg_ptr += 4;
    len -= 4;
  }
  
  MixTail(msg_ptr,len);
}


//----------------------------------------------------------------------
//Make a copy of the current hash, add any remaining tail, the message
//length and mush up the bits a bit more.  We want a copy so that
//repeated calls to Value() give the same result
MurmurHash3::Hash_t MurmurHash3::Value() const
{
  //these must be copies so we dont touch our internal state
  Hash_t hash = fHash;
  UInt_t tail = fTail;
  return EndImp(hash, tail);
}


//----------------------------------------------------------------------
///Before returning, add any remaining tail, the message length and
///mush up the bits a bit more.  We never want to apply the end mushing
///repeatedly - that would be ambiguous - so this also Resets the object.
MurmurHash3::Hash_t MurmurHash3::End(UInt_t seed)
{
  Hash_t ret_hash = EndImp(fHash, fTail);
  this->Reset(seed);
  return ret_hash;
}


//----------------------------------------------------------------------
///Takes parameters by reference and will mush them.
MurmurHash3::Hash_t& MurmurHash3::EndImp(Hash_t& hash, UInt_t& tail) const
{
  if (tail) mmix(hash,tail);
  hash ^= fSize;
  final_mix(hash);
  return hash;
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
    } // if fCount =4
  } // while
}



