/********************************************************************\

  Name:         MCSVOutput
  Created by:   John R Quirk

  Contents:     Module to generate a CSV file from the data in gData

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

/* MIDAS includes */
#include "midas.h"

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"

/*-- Module declaration --------------------------------------------*/

INT  MCSVOutput_init(void);
INT  MCSVOutput_exit(void);
INT  MCSVOutput(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static const int NCRATE   = 10;
static const int NCHANWFD[NCRATE] = {0, 0, 0, 0, 8, 4, 0, 8, 8, 0};
static std::vector< std::vector<std::ofstream*> > vvpOutFiles;

ANA_MODULE MCSVOutput_module =
{
  "MCSVOutput",    /* module name           */
  "John R Quirk",  /* author                */
  MCSVOutput,      /* event routine         */
  NULL,            /* BOR routine           */
  NULL,            /* EOR routine           */
  MCSVOutput_init, /* init routine          */
  MCSVOutput_exit, /* exit routine          */
  NULL,            /* parameter structure   */
  0,               /* structure size        */
  NULL,            /* initial parameters    */
};

INT MCSVOutput_init() {
  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    std::vector<std::ofstream*> cr_pofiles;
    for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      char bank[5], ofname[128];
      sprintf(bank, "D%d%02d", icrate, ich);
      const std::string det = gSetup->GetDetectorName(bank);
      if (det.empty()) continue;
      sprintf(ofname, "%s_%s.csv", det.c_str(), bank);

      std::ofstream* pofile = new std::ofstream(ofname);
      std::ofstream& ofile = *pofile;
      if(ofile.good()) {
	cr_pofiles.push_back(pofile);
	ofile << "Timestamp Height PedSubHeight Samples" << std::endl;
      } else {
	printf("Could not open csv file %s!\n", ofname);
	delete pofile;
	cr_pofiles.push_back(NULL);
      }
    }
    vvpOutFiles.push_back(cr_pofiles);
  }
  return SUCCESS;
}

INT MCSVOutput(EVENT_HEADER *pheader, void *pevent) {
  const std::map< std::string, std::vector<TPulseIsland*> >& wfd_map =
    gData->fPulseIslandToChannelMap;

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      char bank[5]; sprintf(bank, "D%d%02d", icrate, ich);
      if (!wfd_map.count(bank))
	continue;

      const std::vector<TPulseIsland*>& tpis = wfd_map.at(bank);
      std::ofstream& ofile = *vvpOutFiles[icrate][ich];
      for (int itpi = 0; itpi < tpis.size(); ++itpi) {
	const std::vector<int>& samps = tpis[itpi]->GetSamples();
	ofile << tpis[itpi]->GetTimeStamp() << " ";
	ofile << (tpis[itpi]->GetTriggerPolarity() > 0 ?
		  *std::max_element(samps.begin(), samps.end()) :
		  *std::min_element(samps.begin(), samps.end())) << " ";
	ofile << tpis[itpi]->GetPulseHeight() << " ";
	for (int isamp = 0; isamp < samps.size(); ++isamp)
	  ofile << samps[isamp] << " ";
	ofile << std::endl;
      }
      ofile << std::endl;
    }
  }

  return SUCCESS;
}

INT MCSVOutput_exit() {
  for (int icrate = 0; icrate < vvpOutFiles.size(); ++icrate)
    for (int ich = 0; ich < vvpOutFiles[icrate].size(); ++ich)
      if (vvpOutFiles[icrate][ich])
	vvpOutFiles[icrate][ich]->close();
  return SUCCESS;
}
