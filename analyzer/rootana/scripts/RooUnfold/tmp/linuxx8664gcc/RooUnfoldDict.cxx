// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME dIhomedImmIwongdIRooUnfolddItmpdIlinuxx8664gccdIRooUnfoldDict

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "/home/m-wong/RooUnfold/src/RooUnfoldParms.h"
#include "/home/m-wong/RooUnfold/src/RooUnfoldSvd.h"
#include "/home/m-wong/RooUnfold/src/RooUnfoldBinByBin.h"
#include "/home/m-wong/RooUnfold/src/RooUnfoldTUnfold.h"
#include "/home/m-wong/RooUnfold/src/RooUnfoldInvert.h"
#include "/home/m-wong/RooUnfold/src/RooUnfoldErrors.h"
#include "/home/m-wong/RooUnfold/src/RooUnfoldBayes.h"
#include "/home/m-wong/RooUnfold/src/RooUnfold.h"
#include "/home/m-wong/RooUnfold/src/RooUnfoldResponse.h"
#include "/home/m-wong/RooUnfold/src/RooUnfoldIds.h"
#include "/home/m-wong/RooUnfold/src/TSVDUnfold_local.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_RooUnfoldResponse(void *p = 0);
   static void *newArray_RooUnfoldResponse(Long_t size, void *p);
   static void delete_RooUnfoldResponse(void *p);
   static void deleteArray_RooUnfoldResponse(void *p);
   static void destruct_RooUnfoldResponse(void *p);
   static void streamer_RooUnfoldResponse(TBuffer &buf, void *obj);
   static Long64_t merge_RooUnfoldResponse(void *obj, TCollection *coll,TFileMergeInfo *info);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::RooUnfoldResponse*)
   {
      ::RooUnfoldResponse *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::RooUnfoldResponse >(0);
      static ::ROOT::TGenericClassInfo 
         instance("RooUnfoldResponse", ::RooUnfoldResponse::Class_Version(), "RooUnfoldResponse.h", 29,
                  typeid(::RooUnfoldResponse), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::RooUnfoldResponse::Dictionary, isa_proxy, 17,
                  sizeof(::RooUnfoldResponse) );
      instance.SetNew(&new_RooUnfoldResponse);
      instance.SetNewArray(&newArray_RooUnfoldResponse);
      instance.SetDelete(&delete_RooUnfoldResponse);
      instance.SetDeleteArray(&deleteArray_RooUnfoldResponse);
      instance.SetDestructor(&destruct_RooUnfoldResponse);
      instance.SetStreamerFunc(&streamer_RooUnfoldResponse);
      instance.SetMerge(&merge_RooUnfoldResponse);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::RooUnfoldResponse*)
   {
      return GenerateInitInstanceLocal((::RooUnfoldResponse*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::RooUnfoldResponse*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_RooUnfold(void *p = 0);
   static void *newArray_RooUnfold(Long_t size, void *p);
   static void delete_RooUnfold(void *p);
   static void deleteArray_RooUnfold(void *p);
   static void destruct_RooUnfold(void *p);
   static void streamer_RooUnfold(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::RooUnfold*)
   {
      ::RooUnfold *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::RooUnfold >(0);
      static ::ROOT::TGenericClassInfo 
         instance("RooUnfold", ::RooUnfold::Class_Version(), "RooUnfold.h", 23,
                  typeid(::RooUnfold), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::RooUnfold::Dictionary, isa_proxy, 17,
                  sizeof(::RooUnfold) );
      instance.SetNew(&new_RooUnfold);
      instance.SetNewArray(&newArray_RooUnfold);
      instance.SetDelete(&delete_RooUnfold);
      instance.SetDeleteArray(&deleteArray_RooUnfold);
      instance.SetDestructor(&destruct_RooUnfold);
      instance.SetStreamerFunc(&streamer_RooUnfold);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::RooUnfold*)
   {
      return GenerateInitInstanceLocal((::RooUnfold*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::RooUnfold*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_RooUnfoldParms(void *p = 0);
   static void *newArray_RooUnfoldParms(Long_t size, void *p);
   static void delete_RooUnfoldParms(void *p);
   static void deleteArray_RooUnfoldParms(void *p);
   static void destruct_RooUnfoldParms(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::RooUnfoldParms*)
   {
      ::RooUnfoldParms *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::RooUnfoldParms >(0);
      static ::ROOT::TGenericClassInfo 
         instance("RooUnfoldParms", ::RooUnfoldParms::Class_Version(), "RooUnfoldParms.h", 22,
                  typeid(::RooUnfoldParms), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::RooUnfoldParms::Dictionary, isa_proxy, 4,
                  sizeof(::RooUnfoldParms) );
      instance.SetNew(&new_RooUnfoldParms);
      instance.SetNewArray(&newArray_RooUnfoldParms);
      instance.SetDelete(&delete_RooUnfoldParms);
      instance.SetDeleteArray(&deleteArray_RooUnfoldParms);
      instance.SetDestructor(&destruct_RooUnfoldParms);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::RooUnfoldParms*)
   {
      return GenerateInitInstanceLocal((::RooUnfoldParms*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::RooUnfoldParms*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void delete_TSVDUnfold_130729(void *p);
   static void deleteArray_TSVDUnfold_130729(void *p);
   static void destruct_TSVDUnfold_130729(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TSVDUnfold_130729*)
   {
      ::TSVDUnfold_130729 *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::TSVDUnfold_130729 >(0);
      static ::ROOT::TGenericClassInfo 
         instance("TSVDUnfold_130729", ::TSVDUnfold_130729::Class_Version(), "TSVDUnfold_local.h", 58,
                  typeid(::TSVDUnfold_130729), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::TSVDUnfold_130729::Dictionary, isa_proxy, 4,
                  sizeof(::TSVDUnfold_130729) );
      instance.SetDelete(&delete_TSVDUnfold_130729);
      instance.SetDeleteArray(&deleteArray_TSVDUnfold_130729);
      instance.SetDestructor(&destruct_TSVDUnfold_130729);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TSVDUnfold_130729*)
   {
      return GenerateInitInstanceLocal((::TSVDUnfold_130729*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TSVDUnfold_130729*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_RooUnfoldSvd(void *p = 0);
   static void *newArray_RooUnfoldSvd(Long_t size, void *p);
   static void delete_RooUnfoldSvd(void *p);
   static void deleteArray_RooUnfoldSvd(void *p);
   static void destruct_RooUnfoldSvd(void *p);
   static void streamer_RooUnfoldSvd(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::RooUnfoldSvd*)
   {
      ::RooUnfoldSvd *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::RooUnfoldSvd >(0);
      static ::ROOT::TGenericClassInfo 
         instance("RooUnfoldSvd", ::RooUnfoldSvd::Class_Version(), "RooUnfoldSvd.h", 27,
                  typeid(::RooUnfoldSvd), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::RooUnfoldSvd::Dictionary, isa_proxy, 17,
                  sizeof(::RooUnfoldSvd) );
      instance.SetNew(&new_RooUnfoldSvd);
      instance.SetNewArray(&newArray_RooUnfoldSvd);
      instance.SetDelete(&delete_RooUnfoldSvd);
      instance.SetDeleteArray(&deleteArray_RooUnfoldSvd);
      instance.SetDestructor(&destruct_RooUnfoldSvd);
      instance.SetStreamerFunc(&streamer_RooUnfoldSvd);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::RooUnfoldSvd*)
   {
      return GenerateInitInstanceLocal((::RooUnfoldSvd*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::RooUnfoldSvd*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_RooUnfoldBinByBin(void *p = 0);
   static void *newArray_RooUnfoldBinByBin(Long_t size, void *p);
   static void delete_RooUnfoldBinByBin(void *p);
   static void deleteArray_RooUnfoldBinByBin(void *p);
   static void destruct_RooUnfoldBinByBin(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::RooUnfoldBinByBin*)
   {
      ::RooUnfoldBinByBin *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::RooUnfoldBinByBin >(0);
      static ::ROOT::TGenericClassInfo 
         instance("RooUnfoldBinByBin", ::RooUnfoldBinByBin::Class_Version(), "RooUnfoldBinByBin.h", 23,
                  typeid(::RooUnfoldBinByBin), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::RooUnfoldBinByBin::Dictionary, isa_proxy, 4,
                  sizeof(::RooUnfoldBinByBin) );
      instance.SetNew(&new_RooUnfoldBinByBin);
      instance.SetNewArray(&newArray_RooUnfoldBinByBin);
      instance.SetDelete(&delete_RooUnfoldBinByBin);
      instance.SetDeleteArray(&deleteArray_RooUnfoldBinByBin);
      instance.SetDestructor(&destruct_RooUnfoldBinByBin);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::RooUnfoldBinByBin*)
   {
      return GenerateInitInstanceLocal((::RooUnfoldBinByBin*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::RooUnfoldBinByBin*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_RooUnfoldTUnfold(void *p = 0);
   static void *newArray_RooUnfoldTUnfold(Long_t size, void *p);
   static void delete_RooUnfoldTUnfold(void *p);
   static void deleteArray_RooUnfoldTUnfold(void *p);
   static void destruct_RooUnfoldTUnfold(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::RooUnfoldTUnfold*)
   {
      ::RooUnfoldTUnfold *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::RooUnfoldTUnfold >(0);
      static ::ROOT::TGenericClassInfo 
         instance("RooUnfoldTUnfold", ::RooUnfoldTUnfold::Class_Version(), "RooUnfoldTUnfold.h", 25,
                  typeid(::RooUnfoldTUnfold), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::RooUnfoldTUnfold::Dictionary, isa_proxy, 4,
                  sizeof(::RooUnfoldTUnfold) );
      instance.SetNew(&new_RooUnfoldTUnfold);
      instance.SetNewArray(&newArray_RooUnfoldTUnfold);
      instance.SetDelete(&delete_RooUnfoldTUnfold);
      instance.SetDeleteArray(&deleteArray_RooUnfoldTUnfold);
      instance.SetDestructor(&destruct_RooUnfoldTUnfold);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::RooUnfoldTUnfold*)
   {
      return GenerateInitInstanceLocal((::RooUnfoldTUnfold*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::RooUnfoldTUnfold*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_RooUnfoldInvert(void *p = 0);
   static void *newArray_RooUnfoldInvert(Long_t size, void *p);
   static void delete_RooUnfoldInvert(void *p);
   static void deleteArray_RooUnfoldInvert(void *p);
   static void destruct_RooUnfoldInvert(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::RooUnfoldInvert*)
   {
      ::RooUnfoldInvert *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::RooUnfoldInvert >(0);
      static ::ROOT::TGenericClassInfo 
         instance("RooUnfoldInvert", ::RooUnfoldInvert::Class_Version(), "RooUnfoldInvert.h", 25,
                  typeid(::RooUnfoldInvert), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::RooUnfoldInvert::Dictionary, isa_proxy, 4,
                  sizeof(::RooUnfoldInvert) );
      instance.SetNew(&new_RooUnfoldInvert);
      instance.SetNewArray(&newArray_RooUnfoldInvert);
      instance.SetDelete(&delete_RooUnfoldInvert);
      instance.SetDeleteArray(&deleteArray_RooUnfoldInvert);
      instance.SetDestructor(&destruct_RooUnfoldInvert);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::RooUnfoldInvert*)
   {
      return GenerateInitInstanceLocal((::RooUnfoldInvert*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::RooUnfoldInvert*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void delete_RooUnfoldErrors(void *p);
   static void deleteArray_RooUnfoldErrors(void *p);
   static void destruct_RooUnfoldErrors(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::RooUnfoldErrors*)
   {
      ::RooUnfoldErrors *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::RooUnfoldErrors >(0);
      static ::ROOT::TGenericClassInfo 
         instance("RooUnfoldErrors", ::RooUnfoldErrors::Class_Version(), "RooUnfoldErrors.h", 21,
                  typeid(::RooUnfoldErrors), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::RooUnfoldErrors::Dictionary, isa_proxy, 4,
                  sizeof(::RooUnfoldErrors) );
      instance.SetDelete(&delete_RooUnfoldErrors);
      instance.SetDeleteArray(&deleteArray_RooUnfoldErrors);
      instance.SetDestructor(&destruct_RooUnfoldErrors);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::RooUnfoldErrors*)
   {
      return GenerateInitInstanceLocal((::RooUnfoldErrors*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::RooUnfoldErrors*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_RooUnfoldBayes(void *p = 0);
   static void *newArray_RooUnfoldBayes(Long_t size, void *p);
   static void delete_RooUnfoldBayes(void *p);
   static void deleteArray_RooUnfoldBayes(void *p);
   static void destruct_RooUnfoldBayes(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::RooUnfoldBayes*)
   {
      ::RooUnfoldBayes *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::RooUnfoldBayes >(0);
      static ::ROOT::TGenericClassInfo 
         instance("RooUnfoldBayes", ::RooUnfoldBayes::Class_Version(), "RooUnfoldBayes.h", 24,
                  typeid(::RooUnfoldBayes), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::RooUnfoldBayes::Dictionary, isa_proxy, 4,
                  sizeof(::RooUnfoldBayes) );
      instance.SetNew(&new_RooUnfoldBayes);
      instance.SetNewArray(&newArray_RooUnfoldBayes);
      instance.SetDelete(&delete_RooUnfoldBayes);
      instance.SetDeleteArray(&deleteArray_RooUnfoldBayes);
      instance.SetDestructor(&destruct_RooUnfoldBayes);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::RooUnfoldBayes*)
   {
      return GenerateInitInstanceLocal((::RooUnfoldBayes*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::RooUnfoldBayes*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_RooUnfoldIds(void *p = 0);
   static void *newArray_RooUnfoldIds(Long_t size, void *p);
   static void delete_RooUnfoldIds(void *p);
   static void deleteArray_RooUnfoldIds(void *p);
   static void destruct_RooUnfoldIds(void *p);
   static void streamer_RooUnfoldIds(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::RooUnfoldIds*)
   {
      ::RooUnfoldIds *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::RooUnfoldIds >(0);
      static ::ROOT::TGenericClassInfo 
         instance("RooUnfoldIds", ::RooUnfoldIds::Class_Version(), "RooUnfoldIds.h", 18,
                  typeid(::RooUnfoldIds), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::RooUnfoldIds::Dictionary, isa_proxy, 17,
                  sizeof(::RooUnfoldIds) );
      instance.SetNew(&new_RooUnfoldIds);
      instance.SetNewArray(&newArray_RooUnfoldIds);
      instance.SetDelete(&delete_RooUnfoldIds);
      instance.SetDeleteArray(&deleteArray_RooUnfoldIds);
      instance.SetDestructor(&destruct_RooUnfoldIds);
      instance.SetStreamerFunc(&streamer_RooUnfoldIds);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::RooUnfoldIds*)
   {
      return GenerateInitInstanceLocal((::RooUnfoldIds*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::RooUnfoldIds*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr RooUnfoldResponse::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *RooUnfoldResponse::Class_Name()
{
   return "RooUnfoldResponse";
}

//______________________________________________________________________________
const char *RooUnfoldResponse::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldResponse*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int RooUnfoldResponse::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldResponse*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *RooUnfoldResponse::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldResponse*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *RooUnfoldResponse::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldResponse*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr RooUnfold::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *RooUnfold::Class_Name()
{
   return "RooUnfold";
}

//______________________________________________________________________________
const char *RooUnfold::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooUnfold*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int RooUnfold::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooUnfold*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *RooUnfold::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooUnfold*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *RooUnfold::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooUnfold*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr RooUnfoldParms::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *RooUnfoldParms::Class_Name()
{
   return "RooUnfoldParms";
}

//______________________________________________________________________________
const char *RooUnfoldParms::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldParms*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int RooUnfoldParms::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldParms*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *RooUnfoldParms::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldParms*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *RooUnfoldParms::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldParms*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr TSVDUnfold_130729::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *TSVDUnfold_130729::Class_Name()
{
   return "TSVDUnfold_130729";
}

//______________________________________________________________________________
const char *TSVDUnfold_130729::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TSVDUnfold_130729*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int TSVDUnfold_130729::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TSVDUnfold_130729*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *TSVDUnfold_130729::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TSVDUnfold_130729*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *TSVDUnfold_130729::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TSVDUnfold_130729*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr RooUnfoldSvd::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *RooUnfoldSvd::Class_Name()
{
   return "RooUnfoldSvd";
}

//______________________________________________________________________________
const char *RooUnfoldSvd::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldSvd*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int RooUnfoldSvd::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldSvd*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *RooUnfoldSvd::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldSvd*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *RooUnfoldSvd::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldSvd*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr RooUnfoldBinByBin::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *RooUnfoldBinByBin::Class_Name()
{
   return "RooUnfoldBinByBin";
}

//______________________________________________________________________________
const char *RooUnfoldBinByBin::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldBinByBin*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int RooUnfoldBinByBin::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldBinByBin*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *RooUnfoldBinByBin::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldBinByBin*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *RooUnfoldBinByBin::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldBinByBin*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr RooUnfoldTUnfold::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *RooUnfoldTUnfold::Class_Name()
{
   return "RooUnfoldTUnfold";
}

//______________________________________________________________________________
const char *RooUnfoldTUnfold::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldTUnfold*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int RooUnfoldTUnfold::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldTUnfold*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *RooUnfoldTUnfold::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldTUnfold*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *RooUnfoldTUnfold::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldTUnfold*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr RooUnfoldInvert::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *RooUnfoldInvert::Class_Name()
{
   return "RooUnfoldInvert";
}

//______________________________________________________________________________
const char *RooUnfoldInvert::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldInvert*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int RooUnfoldInvert::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldInvert*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *RooUnfoldInvert::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldInvert*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *RooUnfoldInvert::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldInvert*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr RooUnfoldErrors::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *RooUnfoldErrors::Class_Name()
{
   return "RooUnfoldErrors";
}

//______________________________________________________________________________
const char *RooUnfoldErrors::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldErrors*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int RooUnfoldErrors::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldErrors*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *RooUnfoldErrors::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldErrors*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *RooUnfoldErrors::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldErrors*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr RooUnfoldBayes::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *RooUnfoldBayes::Class_Name()
{
   return "RooUnfoldBayes";
}

//______________________________________________________________________________
const char *RooUnfoldBayes::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldBayes*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int RooUnfoldBayes::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldBayes*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *RooUnfoldBayes::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldBayes*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *RooUnfoldBayes::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldBayes*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr RooUnfoldIds::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *RooUnfoldIds::Class_Name()
{
   return "RooUnfoldIds";
}

//______________________________________________________________________________
const char *RooUnfoldIds::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldIds*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int RooUnfoldIds::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldIds*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *RooUnfoldIds::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldIds*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *RooUnfoldIds::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooUnfoldIds*)0x0)->GetClass(); }
   return fgIsA;
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_RooUnfoldResponse(void *p) {
      return  p ? new(p) ::RooUnfoldResponse : new ::RooUnfoldResponse;
   }
   static void *newArray_RooUnfoldResponse(Long_t nElements, void *p) {
      return p ? new(p) ::RooUnfoldResponse[nElements] : new ::RooUnfoldResponse[nElements];
   }
   // Wrapper around operator delete
   static void delete_RooUnfoldResponse(void *p) {
      delete ((::RooUnfoldResponse*)p);
   }
   static void deleteArray_RooUnfoldResponse(void *p) {
      delete [] ((::RooUnfoldResponse*)p);
   }
   static void destruct_RooUnfoldResponse(void *p) {
      typedef ::RooUnfoldResponse current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_RooUnfoldResponse(TBuffer &buf, void *obj) {
      ((::RooUnfoldResponse*)obj)->::RooUnfoldResponse::Streamer(buf);
   }
   // Wrapper around the merge function.
   static Long64_t  merge_RooUnfoldResponse(void *obj,TCollection *coll,TFileMergeInfo *) {
      return ((::RooUnfoldResponse*)obj)->Merge(coll);
   }
} // end of namespace ROOT for class ::RooUnfoldResponse

namespace ROOT {
   // Wrappers around operator new
   static void *new_RooUnfold(void *p) {
      return  p ? new(p) ::RooUnfold : new ::RooUnfold;
   }
   static void *newArray_RooUnfold(Long_t nElements, void *p) {
      return p ? new(p) ::RooUnfold[nElements] : new ::RooUnfold[nElements];
   }
   // Wrapper around operator delete
   static void delete_RooUnfold(void *p) {
      delete ((::RooUnfold*)p);
   }
   static void deleteArray_RooUnfold(void *p) {
      delete [] ((::RooUnfold*)p);
   }
   static void destruct_RooUnfold(void *p) {
      typedef ::RooUnfold current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_RooUnfold(TBuffer &buf, void *obj) {
      ((::RooUnfold*)obj)->::RooUnfold::Streamer(buf);
   }
} // end of namespace ROOT for class ::RooUnfold

//______________________________________________________________________________
void RooUnfoldParms::Streamer(TBuffer &R__b)
{
   // Stream an object of class RooUnfoldParms.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(RooUnfoldParms::Class(),this);
   } else {
      R__b.WriteClassBuffer(RooUnfoldParms::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_RooUnfoldParms(void *p) {
      return  p ? new(p) ::RooUnfoldParms : new ::RooUnfoldParms;
   }
   static void *newArray_RooUnfoldParms(Long_t nElements, void *p) {
      return p ? new(p) ::RooUnfoldParms[nElements] : new ::RooUnfoldParms[nElements];
   }
   // Wrapper around operator delete
   static void delete_RooUnfoldParms(void *p) {
      delete ((::RooUnfoldParms*)p);
   }
   static void deleteArray_RooUnfoldParms(void *p) {
      delete [] ((::RooUnfoldParms*)p);
   }
   static void destruct_RooUnfoldParms(void *p) {
      typedef ::RooUnfoldParms current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::RooUnfoldParms

//______________________________________________________________________________
void TSVDUnfold_130729::Streamer(TBuffer &R__b)
{
   // Stream an object of class TSVDUnfold_130729.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(TSVDUnfold_130729::Class(),this);
   } else {
      R__b.WriteClassBuffer(TSVDUnfold_130729::Class(),this);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_TSVDUnfold_130729(void *p) {
      delete ((::TSVDUnfold_130729*)p);
   }
   static void deleteArray_TSVDUnfold_130729(void *p) {
      delete [] ((::TSVDUnfold_130729*)p);
   }
   static void destruct_TSVDUnfold_130729(void *p) {
      typedef ::TSVDUnfold_130729 current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::TSVDUnfold_130729

namespace ROOT {
   // Wrappers around operator new
   static void *new_RooUnfoldSvd(void *p) {
      return  p ? new(p) ::RooUnfoldSvd : new ::RooUnfoldSvd;
   }
   static void *newArray_RooUnfoldSvd(Long_t nElements, void *p) {
      return p ? new(p) ::RooUnfoldSvd[nElements] : new ::RooUnfoldSvd[nElements];
   }
   // Wrapper around operator delete
   static void delete_RooUnfoldSvd(void *p) {
      delete ((::RooUnfoldSvd*)p);
   }
   static void deleteArray_RooUnfoldSvd(void *p) {
      delete [] ((::RooUnfoldSvd*)p);
   }
   static void destruct_RooUnfoldSvd(void *p) {
      typedef ::RooUnfoldSvd current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_RooUnfoldSvd(TBuffer &buf, void *obj) {
      ((::RooUnfoldSvd*)obj)->::RooUnfoldSvd::Streamer(buf);
   }
} // end of namespace ROOT for class ::RooUnfoldSvd

//______________________________________________________________________________
void RooUnfoldBinByBin::Streamer(TBuffer &R__b)
{
   // Stream an object of class RooUnfoldBinByBin.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(RooUnfoldBinByBin::Class(),this);
   } else {
      R__b.WriteClassBuffer(RooUnfoldBinByBin::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_RooUnfoldBinByBin(void *p) {
      return  p ? new(p) ::RooUnfoldBinByBin : new ::RooUnfoldBinByBin;
   }
   static void *newArray_RooUnfoldBinByBin(Long_t nElements, void *p) {
      return p ? new(p) ::RooUnfoldBinByBin[nElements] : new ::RooUnfoldBinByBin[nElements];
   }
   // Wrapper around operator delete
   static void delete_RooUnfoldBinByBin(void *p) {
      delete ((::RooUnfoldBinByBin*)p);
   }
   static void deleteArray_RooUnfoldBinByBin(void *p) {
      delete [] ((::RooUnfoldBinByBin*)p);
   }
   static void destruct_RooUnfoldBinByBin(void *p) {
      typedef ::RooUnfoldBinByBin current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::RooUnfoldBinByBin

//______________________________________________________________________________
void RooUnfoldTUnfold::Streamer(TBuffer &R__b)
{
   // Stream an object of class RooUnfoldTUnfold.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(RooUnfoldTUnfold::Class(),this);
   } else {
      R__b.WriteClassBuffer(RooUnfoldTUnfold::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_RooUnfoldTUnfold(void *p) {
      return  p ? new(p) ::RooUnfoldTUnfold : new ::RooUnfoldTUnfold;
   }
   static void *newArray_RooUnfoldTUnfold(Long_t nElements, void *p) {
      return p ? new(p) ::RooUnfoldTUnfold[nElements] : new ::RooUnfoldTUnfold[nElements];
   }
   // Wrapper around operator delete
   static void delete_RooUnfoldTUnfold(void *p) {
      delete ((::RooUnfoldTUnfold*)p);
   }
   static void deleteArray_RooUnfoldTUnfold(void *p) {
      delete [] ((::RooUnfoldTUnfold*)p);
   }
   static void destruct_RooUnfoldTUnfold(void *p) {
      typedef ::RooUnfoldTUnfold current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::RooUnfoldTUnfold

//______________________________________________________________________________
void RooUnfoldInvert::Streamer(TBuffer &R__b)
{
   // Stream an object of class RooUnfoldInvert.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(RooUnfoldInvert::Class(),this);
   } else {
      R__b.WriteClassBuffer(RooUnfoldInvert::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_RooUnfoldInvert(void *p) {
      return  p ? new(p) ::RooUnfoldInvert : new ::RooUnfoldInvert;
   }
   static void *newArray_RooUnfoldInvert(Long_t nElements, void *p) {
      return p ? new(p) ::RooUnfoldInvert[nElements] : new ::RooUnfoldInvert[nElements];
   }
   // Wrapper around operator delete
   static void delete_RooUnfoldInvert(void *p) {
      delete ((::RooUnfoldInvert*)p);
   }
   static void deleteArray_RooUnfoldInvert(void *p) {
      delete [] ((::RooUnfoldInvert*)p);
   }
   static void destruct_RooUnfoldInvert(void *p) {
      typedef ::RooUnfoldInvert current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::RooUnfoldInvert

//______________________________________________________________________________
void RooUnfoldErrors::Streamer(TBuffer &R__b)
{
   // Stream an object of class RooUnfoldErrors.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(RooUnfoldErrors::Class(),this);
   } else {
      R__b.WriteClassBuffer(RooUnfoldErrors::Class(),this);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_RooUnfoldErrors(void *p) {
      delete ((::RooUnfoldErrors*)p);
   }
   static void deleteArray_RooUnfoldErrors(void *p) {
      delete [] ((::RooUnfoldErrors*)p);
   }
   static void destruct_RooUnfoldErrors(void *p) {
      typedef ::RooUnfoldErrors current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::RooUnfoldErrors

//______________________________________________________________________________
void RooUnfoldBayes::Streamer(TBuffer &R__b)
{
   // Stream an object of class RooUnfoldBayes.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(RooUnfoldBayes::Class(),this);
   } else {
      R__b.WriteClassBuffer(RooUnfoldBayes::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_RooUnfoldBayes(void *p) {
      return  p ? new(p) ::RooUnfoldBayes : new ::RooUnfoldBayes;
   }
   static void *newArray_RooUnfoldBayes(Long_t nElements, void *p) {
      return p ? new(p) ::RooUnfoldBayes[nElements] : new ::RooUnfoldBayes[nElements];
   }
   // Wrapper around operator delete
   static void delete_RooUnfoldBayes(void *p) {
      delete ((::RooUnfoldBayes*)p);
   }
   static void deleteArray_RooUnfoldBayes(void *p) {
      delete [] ((::RooUnfoldBayes*)p);
   }
   static void destruct_RooUnfoldBayes(void *p) {
      typedef ::RooUnfoldBayes current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::RooUnfoldBayes

namespace ROOT {
   // Wrappers around operator new
   static void *new_RooUnfoldIds(void *p) {
      return  p ? new(p) ::RooUnfoldIds : new ::RooUnfoldIds;
   }
   static void *newArray_RooUnfoldIds(Long_t nElements, void *p) {
      return p ? new(p) ::RooUnfoldIds[nElements] : new ::RooUnfoldIds[nElements];
   }
   // Wrapper around operator delete
   static void delete_RooUnfoldIds(void *p) {
      delete ((::RooUnfoldIds*)p);
   }
   static void deleteArray_RooUnfoldIds(void *p) {
      delete [] ((::RooUnfoldIds*)p);
   }
   static void destruct_RooUnfoldIds(void *p) {
      typedef ::RooUnfoldIds current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_RooUnfoldIds(TBuffer &buf, void *obj) {
      ((::RooUnfoldIds*)obj)->::RooUnfoldIds::Streamer(buf);
   }
} // end of namespace ROOT for class ::RooUnfoldIds

namespace {
  void TriggerDictionaryInitialization_RooUnfoldDict_Impl() {
    static const char* headers[] = {
"src/RooUnfoldParms.h",
"src/RooUnfoldSvd.h",
"src/RooUnfoldBinByBin.h",
"src/RooUnfoldTUnfold.h",
"src/RooUnfoldInvert.h",
"src/RooUnfoldErrors.h",
"src/RooUnfoldBayes.h",
"src/RooUnfold.h",
"src/RooUnfoldResponse.h",
"src/RooUnfoldIds.h",
"src/TSVDUnfold_local.h",
0
    };
    static const char* includePaths[] = {
"/home/m-wong/RooUnfold/src/",
"/home/m-wong/RooUnfold",
"/usr/include/root",
"/home/m-wong/RooUnfold/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "RooUnfoldDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate(R"ATTRDUMP(Respose Matrix)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$RooUnfoldResponse.h")))  __attribute__((annotate("$clingAutoload$src/RooUnfoldParms.h")))  RooUnfoldResponse;
class __attribute__((annotate(R"ATTRDUMP(Unfolding base class: implementations in RooUnfoldBayes, RooUnfoldSvd, RooUnfoldBinByBin, RooUnfoldTUnfold, RooUnfoldInvert, RooUnfoldIds)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$RooUnfold.h")))  __attribute__((annotate("$clingAutoload$src/RooUnfoldParms.h")))  RooUnfold;
class __attribute__((annotate(R"ATTRDUMP(Optimisation of unfolding regularisation parameter)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$src/RooUnfoldParms.h")))  RooUnfoldParms;
class __attribute__((annotate(R"ATTRDUMP(Data unfolding using Singular Value Decomposition (hep-ph/9509307))ATTRDUMP"))) __attribute__((annotate("$clingAutoload$TSVDUnfold_local.h")))  __attribute__((annotate("$clingAutoload$src/RooUnfoldSvd.h")))  TSVDUnfold_130729;
class __attribute__((annotate(R"ATTRDUMP(SVD Unfolding (interface to TSVDUnfold))ATTRDUMP"))) __attribute__((annotate("$clingAutoload$src/RooUnfoldSvd.h")))  RooUnfoldSvd;
class __attribute__((annotate(R"ATTRDUMP(Bin-by-bin unfolding)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$src/RooUnfoldBinByBin.h")))  RooUnfoldBinByBin;
class __attribute__((annotate(R"ATTRDUMP(Interface to TUnfold)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$src/RooUnfoldTUnfold.h")))  RooUnfoldTUnfold;
class __attribute__((annotate(R"ATTRDUMP(Unregularised unfolding)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$src/RooUnfoldInvert.h")))  RooUnfoldInvert;
class __attribute__((annotate(R"ATTRDUMP(Show unfolding errors)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$src/RooUnfoldErrors.h")))  RooUnfoldErrors;
class __attribute__((annotate(R"ATTRDUMP(Bayesian Unfolding)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$src/RooUnfoldBayes.h")))  RooUnfoldBayes;
class __attribute__((annotate("$clingAutoload$src/RooUnfoldIds.h")))  RooUnfoldIds;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "RooUnfoldDict dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif
#ifndef HAVE_TSVDUNFOLD
  #define HAVE_TSVDUNFOLD 1
#endif
#ifndef MAKEBUILD
  #define MAKEBUILD 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "src/RooUnfoldParms.h"
#include "src/RooUnfoldSvd.h"
#include "src/RooUnfoldBinByBin.h"
#include "src/RooUnfoldTUnfold.h"
#include "src/RooUnfoldInvert.h"
#include "src/RooUnfoldErrors.h"
#include "src/RooUnfoldBayes.h"
#include "src/RooUnfold.h"
#include "src/RooUnfoldResponse.h"
#include "src/RooUnfoldIds.h"
#include "src/TSVDUnfold_local.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"RooUnfold", payloadCode, "@",
"RooUnfoldBayes", payloadCode, "@",
"RooUnfoldBinByBin", payloadCode, "@",
"RooUnfoldErrors", payloadCode, "@",
"RooUnfoldIds", payloadCode, "@",
"RooUnfoldInvert", payloadCode, "@",
"RooUnfoldParms", payloadCode, "@",
"RooUnfoldResponse", payloadCode, "@",
"RooUnfoldSvd", payloadCode, "@",
"RooUnfoldTUnfold", payloadCode, "@",
"TSVDUnfold_130729", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("RooUnfoldDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_RooUnfoldDict_Impl, {}, classesHeaders, /*has no C++ module*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_RooUnfoldDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_RooUnfoldDict() {
  TriggerDictionaryInitialization_RooUnfoldDict_Impl();
}
