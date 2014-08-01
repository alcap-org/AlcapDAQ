#include "TAPGeneratorFactory.h"
#include "TemplateAPGenerator.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"

#include "utils/TemplateFitter.h"

#include <iostream>
using std::cout;
using std::endl;

TemplateAPGenerator::TemplateAPGenerator(TAPGeneratorOptions* opts):
	TVAnalysedPulseGenerator("Template",opts){
	// Do things to set up the generator here. 

  fTemplateArchive = new TemplateArchive("templates.root", "READ");
}

int TemplateAPGenerator::ProcessPulses( 
		const PulseIslandList& pulseList,
		AnalysedPulseList& analysedList){
  // Do something here that takes the TPIs in the PulseIslandList and
  // fills the list of TAPS

  // Loop over all the TPIs given to us
  double amplitude;
  TAnalysedPulse* tap;

  // Analyse each TPI
  for (PulseIslandList::const_iterator tpi=pulseList.begin();
       tpi!=pulseList.end(); tpi++){


    // Get the template from the archive
    std::string channel = GetChannel().str();
    std::string templatename = "hTemplate_" + channel;
    TH1D* hTemplate = fTemplateArchive->GetTemplate(templatename.c_str());

    // Then we try and fit this TPI to the template
    TemplateFitter* template_fitter = new TemplateFitter(channel, 5);

    // Create the histogram of the pulse
    TH1D* hPulseToFit = template_fitter->CreateRefinedPulseHistogram(tpi, "hPulseToFit", "hPulseToFit", false);


    // If successful, we then have the amplitude and time (offset?) of the pulse
    

    // Now that we've found the information we were looking for make a TAP to
    // hold it.  This method makes a TAP and sets the parent TPI info.  It needs
    // the index of the parent TPI in the container as an argument
    tap = MakeNewTAP(tpi-pulseList.begin());

    // Get the amplitude and time of the TAP
    //tap->SetAmplitude(amplitude);
    //tap->SetTime(time);

    // Finally add the new TAP to the output list
    analysedList.push_back(tap);
   }
	
  // Generators have a Debug method similar to modules
  if(Debug()){
    // They also have a unique ID, retrievable by GetSource and
    // a GetChannel method to get the ID of just the channel
    cout<<"Now running source: "<<GetSource().str()<<" which looks for TAPs on "
      "channel: "<<GetChannel().str()<<'\n';
  }
  
  // returning 0 tells the caller that we were successful, return non-zero if not
  return 0;
}

// Similar to the modules, this macro registers the generator with
// MakeAnalysedPulses. The first argument is compulsory and gives the name of
// this generator. All subsequent arguments will be used as names for arguments
// given directly within the modules file.  See the github wiki for more.
//
// NOTE: for TAP generators OMIT the APGenerator part of the class' name
ALCAP_TAP_GENERATOR(Template);
