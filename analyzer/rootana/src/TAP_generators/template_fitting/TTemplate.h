#ifndef TAP_GENERATORS_TEMPLATE_FITTING_TTEMPLATE_H
#define TAP_GENERATORS_TEMPLATE_FITTING_TTEMPLATE_H

#include <TH1D.h>
#include <TObject.h>
#include <TDirectory.h>

#include "IdChannel.h"
#include "TPulseIsland.h"

class TTemplate:public TObject{

   public:
      TTemplate(const std::string& det,int refine,int trigger_polarity,bool debug);
      ~TTemplate(){};
      
      bool HasConverged()const{return fConverged;}
      bool CheckConverged();
      bool Empty()const{return fTotalPulses==0;}
      bool PulsesMerged()const{return fTotalPulses;}
      void Initialize(int pulseID, TH1D* pulse, TDirectory* dir);
      void AddPulse(double x_offset, double y_scale, double y_offset,const TH1D*);
      void AddToDirectory(TDirectory* dir){
        dir->Append(this,true);
        fTemplatePulse->SetDirectory(dir);
        fErrors->SetDirectory(dir);
      }

      double GetPedestal()const;
      double GetTime()const;
      double GetAmplitude()const;

      const TH1D* GetHisto()const{return fTemplatePulse;}

   private:
      bool fDebug;
      bool fConverged; ///< have we converged or not?
      int fTotalPulses; ///< How many pulses have been averaged to make the template
      int fRefineFactor;
      int fTriggerPolarity;
      IDs::channel fChannel;
      TH1D *fErrors;
      TH1D *fTemplatePulse;
   
   ClassDef(TTemplate,1);
};

#endif // TAP_GENERATORS_TEMPLATE_FITTING_TTEMPLATE_H
