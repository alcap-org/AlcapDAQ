#ifndef TME_GENERATORS_TSILICONEVENT_H
#define TME_GENERATORS_TSILICONEVENT_H

#include <vector>

class TSiliconEvent{

   public:
     TSiliconEvent():fNThinHits(0),fDeltaE(0),fTotalE(0){};
     ~TSiliconEvent(){};

     void Reset(const char* o=""){ 
       fDeltaE=0;
       fNThinHits=0;
       fTotalE=0;
       fLastQuad=0;
     } 

     void SetThick(double e, double t){ fTotalE=e; fThickTime=t;} 
     void AddThin(int quad, double e){
        ++fNThinHits;
        fLastQuad=quad;
        fDeltaE+=e;
        fTotalE+=e;
     }

     int GetNThinHits()const{return fNThinHits;}
     int GetLastQuad()const{return fLastQuad;}
     double GetDeltaE()const{return fDeltaE;}
     double GetTotalE()const{return fTotalE;}
     double GetThickTime()const{return fThickTime;}

   private:
     int fNThinHits;
     int fLastQuad;
     double fDeltaE;
     double fTotalE;
     double fThickTime;

};

typedef std::vector<TSiliconEvent> SiliconHitList;

#endif // TME_GENERATORS_TSILICONEVENT_H
