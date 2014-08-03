#ifndef FLYWEIGHT_H
#define FLYWEIGHT_H

#include <map>
#include <vector>
#include <iostream>
#include <string>

#include <TObject.h>
#include <TDirectory.h>

template <typename ValueType, typename UniqueTag=int>
class FlyWeightLists:public TObject{
    public:
        FlyWeightLists(){};
        typedef int Proxy_t;
        typedef std::map<ValueType,Proxy_t> ValueToProxyMap;
        typedef std::vector<ValueType> ProxyToValueMap;

        int GetProxy(const ValueType& v);
        const ValueType& GetValue(const Proxy_t& v);

        bool empty()const{return sProxyToVals.empty();}
        void DumpTable()const;

    private:
        ProxyToValueMap sProxyToVals;
        ValueToProxyMap sValsToProxies;

        ClassDef(FlyWeightLists,1);
};

/// \name FlyWeight
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

        FlyWeight():fProxy(-1){}
        FlyWeight(const ValueType& val):fProxy(-1){
            SetValue(val);
        }
        void Reset(){fProxy=-1;}

        void operator=(const ValueType& v){
            SetValue(v);
        }
        void SetValue(const ValueType&);
        const ValueType& GetValue()const;
        void DumpTable()const {fProxyList->DumpTable();}

        static int LoadProxyList(TDirectory* file, const std::string& name );
        static int SaveProxyList(TDirectory* file, const std::string& name );
        static void InitProxyList(){fProxyList=new FlyWeightLists<ValueType,UniqueTag>();};

    private:
        Proxy_t fProxy;

        static FlyWeightLists<ValueType,UniqueTag>* fProxyList;

        ClassDef(FlyWeight,2);
};

template <typename ValueType, typename UniqueTag>
inline const ValueType& FlyWeight<ValueType,UniqueTag>::GetValue()const{
    if(fProxy<0) std::cerr<<"Trying to get a value "
        "from an unitialized flyweight"<<std::endl;
    return fProxyList->GetValue(fProxy);
}

template <typename ValueType, typename UniqueTag>
inline void FlyWeight<ValueType,UniqueTag>::SetValue(const ValueType& v){
    if(!fProxyList)InitProxyList();
    fProxy=fProxyList->GetProxy(v);
}

template <typename ValueType, typename UniqueTag>
inline int FlyWeight<ValueType,UniqueTag>:: LoadProxyList(
        TDirectory* file, const std::string& name ){
    if(fProxyList && !fProxyList->empty()){
        std::cout<<"FlyWeight: Error: Proxy list is not empty but was asked to load the list from a file"<<std::endl;
        return 1;
    }
    if(!file) {
        std::cout<<"FlyWeight: Error: Cannot load a proxy list from a NULL file"<<std::endl;
        return 2;
    }
    file->GetObject(name.c_str(),fProxyList);
    if(!fProxyList){
        std::cout<<"FlyWeight: Error: Couldn't find proxy list '"<<name
            <<"' in file '"<<file->GetName()<<"'"<<std::endl;
        return 3;
    }
    return 0;
}

template <typename ValueType, typename UniqueTag>
inline int FlyWeight<ValueType,UniqueTag>::SaveProxyList(
        TDirectory* file, const std::string& name ){
    if(!file) {
        std::cout<<"FlyWeight: Error: Cannot save a proxy list from a NULL file"<<std::endl;
        return 1;
    }
    file->WriteObject(fProxyList,name.c_str());
    return 0;
}


template <typename ValueType, typename UniqueTag>
FlyWeightLists<ValueType,UniqueTag>* 
FlyWeight<ValueType,UniqueTag>::fProxyList=NULL;

template <typename ValueType, typename UniqueTag>
inline const ValueType& FlyWeightLists<ValueType,UniqueTag>::GetValue(
        const Proxy_t& v){
    return sProxyToVals.at(v);
}

template <typename ValueType, typename UniqueTag>
inline int FlyWeightLists<ValueType,UniqueTag>::GetProxy(const ValueType& v){
    // Is this proxy already contained in the maps
    typename ValueToProxyMap::const_iterator it=sValsToProxies.find(v);
    if(it!=sValsToProxies.end()){
        return it->second;
    } 
    // if it's a new kind, add the source to the hash table
    int proxy=sValsToProxies.size();
    sValsToProxies[v]=proxy;
    sProxyToVals.push_back(v);
    return proxy;
}

template <typename ValueType, typename UniqueTag>
inline void FlyWeightLists<ValueType,UniqueTag>::DumpTable()const{
    std::cout<<"Proxy | Value | Proxy"<<std::endl;
    for(unsigned i=0;i<sProxyToVals.size();i++){
        std::cout<<i<<" | "<<sProxyToVals[i]<<" | "<<sValsToProxies.find(sProxyToVals[i])->second<<std::endl;
    }
}
#endif //FLYWEIGHT_H
