#ifndef FLYWEIGHT_H
#define FLYWEIGHT_H

#include <map>
#include <vector>
#include <iostream>

#include <TObject.h>

/// \name SourceProxy
///
/// \brief
/// A generic flyweight to store relatively bulky objects that are massively
/// re-used, such as the source IDs in each TAP / TDP.
///
/// \details
/// Objects such as TAPs and TDPs often contain largely repeated data such as
/// the source ID for all pulses coming from the same algorithm applied to the
/// same detector channel.
/// To reduce the redundancy and therefore save memory and some processing time,
/// this class implements a simple version of the flyweight pattern, which
/// replaces each different value with an int, but provides a similar interface
/// to the object that it wraps.
/// When we want to store a new value, it is searched for in the ProxyToValueMap
/// and the index identifying where it is is saved is used instead. If the value
/// has not been used already, it is added to the list of values
/// @tparam ValueType The type of value this flyweight contains
/// @tparam UniqueTag An optional template parameter to make sure the flyweight
/// is unique, such as to separate the flyweight for TAPs from TDPs, even though
/// the ValueType is the same
template <typename ValueType, typename UniqueTag=int>
class FlyWeight:public TObject{
    public:
        typedef int Proxy_t;
        typedef std::map<ValueType,Proxy_t> ValueToProxyMap;
        typedef std::vector<ValueType> ProxyToValueMap;

        FlyWeight():fProxy(-1){}
        void Reset(){fProxy=-1;}

        void operator=(const ValueType& v){
            SetValue(v);
        }
        void SetValue(const ValueType&);
        const ValueType& GetValue()const;

    private:
        Proxy_t fProxy;
        static ProxyToValueMap sProxyToVals;
        static ValueToProxyMap sValsToProxies;

        ClassDef(FlyWeight,1);
};

template <typename ValueType, typename UniqueTag>
inline const ValueType& FlyWeight<ValueType,UniqueTag>::GetValue()const{
    if(fProxy<0) std::cerr<<"Trying to get a value "
        "from an unitialized flyweight"<<std::endl;
    return sProxyToVals.at(fProxy);
}

template <typename ValueType, typename UniqueTag>
inline void FlyWeight<ValueType,UniqueTag>::SetValue(const ValueType& v){
    // Is this proxy already contained in the maps
    typename ValueToProxyMap::const_iterator it=sValsToProxies.find(v);
    if(it!=sValsToProxies.end()){
        fProxy=it->second;
    } else {
        // if it's a new kind, add the source to the hash table
        fProxy=sValsToProxies.size();
        sValsToProxies[v]=fProxy;
        sProxyToVals.push_back(v);
    }
}

template <typename ValueType, typename UniqueTag>
  typename FlyWeight<ValueType,UniqueTag>::ProxyToValueMap 
  FlyWeight<ValueType,UniqueTag>::sProxyToVals;

template <typename ValueType, typename UniqueTag>
  typename FlyWeight<ValueType,UniqueTag>::ValueToProxyMap 
  FlyWeight<ValueType,UniqueTag>::sValsToProxies;

#endif //FLYWEIGHT_H
