#ifndef TME_GENERATORS_TSILICONEVENT_H
#define TME_GENERATORS_TSILICONEVENT_H

#include <vector>

class TSiliconEvent{

   public:
     TSiliconEvent()fNThinHits(0),fDeltaE(0),fTotalE(0){};
     ~TSiliconEvent(){};

     void Reset(const char =""){ 
       fDeltaE=0;
       fNThinHits=0;
       fTotalE=0;
     } 

     void SetThick(double e){ fTotalE=e;} 
     void AddThin(double e){
        ++fNThinHits;
        fDeltaE+=e;
        fTotalE+=e;
     }

     int GetNThinHits()const{return fNThinHits;}
     double GetDeltaE()const{return fDeltaE;}
     double GetTotalE()const{return fTotalE;}

   private:
     int fNThinHits;
     double fDeltaE;
     double fTotalE;

};

typedef std::vector<TSiliconEvent> SiliconHitList;

#endif // TME_GENERATORS_TSILICONEVENT_H
