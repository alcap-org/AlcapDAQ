#ifndef TAP_GENERATORS_TEMPLATE_FITTING_TTEMPLATE_H
#define TAP_GENERATORS_TEMPLATE_FITTING_TTEMPLATE_H

#include <TH1D.h>
#include <TObject.h>
#include <TDirectory.h>

#include "IdChannel.h"
#include "TPulseIsland.h"

class TTemplate:public TObject{

   public:
      TTemplate();
      TTemplate(const std::string& det,int refine,int trigger_polarity,bool debug);
      virtual ~TTemplate();
      
      bool HasConverged()const{return fConverged;}
      bool CheckConverged();
      bool Empty()const{return fTotalPulses==0;}
      int PulsesMerged()const{return fTotalPulses;}
      void Initialize(int pulseID, TH1D* pulse, TDirectory* dir);
      void AddPulse(double x_offset, double y_scale, double y_offset,const TH1D*);
      void AddToDirectory(TDirectory* dir,TDirectory* hist_dir){
       TDirectory* curr=TDirectory::CurrentDirectory();
        dir->cd();
        TObject::Write();
        if(hist_dir){
          fTemplatePulse->SetDirectory(hist_dir);
          fErrors->SetDirectory(hist_dir);
	  //	  fTemplateErrors->SetDirectory(hist_dir);
        }
        curr->cd();
      }

      void NormaliseToAmplitude();
      void NormaliseToIntegral();
      void NormaliseToSumSquares();
      TH1* RebinToOriginalSampling();

      //      void CreateTemplateErrorsHist();

      double GetPedestal()const;
      double GetTime()const;
      double GetCFTime();
      double GetAmplitude()const;
      int GetPolarity()const{return fTriggerPolarity;};

      double GetRefineFactor()const{return fRefineFactor;}

      const TH1D* GetHisto()const{return fTemplatePulse;}

      const char* GetName()const {return fName.c_str();}
      static std::string MakeName(const IDs::channel& ch){return ch.str()+"_template";}
   private:
      void SubtractPedestal();
      void ScaleHist(double factor);

   private:
      bool fDebug;
      bool fConverged; ///< have we converged or not?
      int fTotalPulses; ///< How many pulses have been averaged to make the template
      int fRefineFactor; ///< How many samples in the template correspond to 1 sample in an actual wavform
      int fTriggerPolarity;
      int fCFTimeBin;
      IDs::channel fChannel;
      std::string fName;
      TH1D *fErrors;
      TH1D *fTemplatePulse;
      //      TH1D *fTemplateErrors;
   
   ClassDef(TTemplate,1);
};

#endif // TAP_GENERATORS_TEMPLATE_FITTING_TTEMPLATE_H
