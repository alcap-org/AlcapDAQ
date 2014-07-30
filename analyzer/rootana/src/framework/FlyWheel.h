#ifndef FLYWHEEL_H
#define FLYWHEEL_H

/// \name SourceProxy
///
/// \brief
/// A flyweight for identifying which source was used to make
/// the TAP without copying the entire source.
///
/// \details
/// Because many TAPs will be using the same source, and we don't want to
/// waste the space storing copies of them or the time making them, we
/// instead store a single copy in a vector of each source used throughout
/// the invocation of this program in the making of TAPs.
/// If a TAP generator uses a source, that source is searched for in
/// ProxyToSourceMap and the index identifying where it is is saved
/// with this TAP. If the source does not exists, it is created and
/// appended to the end.
template <typename ValueType, typename UniqueTag=int>
class FlyWheel{
    public:
        typedef int Proxy_t;
        typedef std::map<ValueType,Proxy_t> ValueToProxyMap;
        typedef std::vector<ValueType> ProxyToValueMap;

        ValueType& operator=(const ValueType& v){
            SetValue(v);
            return v;
        }
        void SetValue(const ValueType&);
        const ValueType& GetValue()const;

    private:
        Proxy_t fProxy;
        static ProxyToValueMap sProxyToVals;
        static ValueToProxyMap sValsToProxies;
};

template <typename ValueType, typename UniqueTag>
inline const ValueType& FlyWheel<ValueType,UniqueTag>::GetValue()const{
    return sProxyToVals.at(fProxy);
}

template <typename ValueType, typename UniqueTag>
inline void FlyWheel<ValueType,UniqueTag>::SetValue(const ValueType& v){
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
typename FlyWheel<ValueType,UniqueTag>::ProxyToValueMap FlyWheel<ValueType,UniqueTag>::sProxyToVals;

template <typename ValueType, typename UniqueTag>
typename FlyWheel<ValueType,UniqueTag>::ValueToProxyMap FlyWheel<ValueType,UniqueTag>::sValsToProxies;

#endif //FLYWHEEL_H
