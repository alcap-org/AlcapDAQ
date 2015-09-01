////////////////////////////////////////////////////////////////////////////////
/// \defgroup MExportShapes
///
/// @{
////////////////////////////////////////////////////////////////////////////////

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <map>
#include <algorithm>
#include <numeric>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include "TH1D.h"
#include "TH2D.h"
#include "TDirectory.h"

//JG: added alcap includes
/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"


/*-- Module declaration --------------------------------------------*/
static INT MExportShapes_init(void);
static INT MExportShapes(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static const int NCRATE = 10;
static const int NCHANWFD[NCRATE] = { 0, 0, 0, 0, 8,
                                      4, 0, 8, 8, 0 };
static const int NBIT[NCRATE] = { 0, 0, 0, 0, 14,
				   12, 0, 14, 12, 0 };
static TDirectory* HISTDIR[NCRATE][10];

ANA_MODULE MExportShapes_module =
{
  "MExportShapes",    /* module name           */
  "John R Quirk",     /* author                */
  MExportShapes,      /* event routine         */
  NULL,               /* BOR routine           */
  NULL,               /* EOR routine           */
  MExportShapes_init, /* init routine          */
  NULL,               /* exit routine          */
  NULL,               /* parameter structure   */
  0,                  /* structure size        */
  NULL,               /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MExportShapes_init() {
  return SUCCESS;
}


/*-- module event routine -----------------------------------------*/
INT MExportShapes(EVENT_HEADER *pheader, void *pevent) {
  const std::map< std::string, std::vector<TPulseIsland*> >& wfd_map =
    gData->fPulseIslandToChannelMap;

  TDirectory* cwd = gDirectory;
  
  static int evt = -1; ++evt;

  if (evt == 0) {
    gDirectory->mkdir("ExportShapes/")->cd();
    for (int icrate = 0; icrate < NCRATE; ++icrate) {
      char crdirname[16]; sprintf(crdirname, "Crate%d", icrate);
      TDirectory* crdir = gDirectory->mkdir(crdirname);
      for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
	char chdirname[8]; sprintf(chdirname, "Ch%02d", ich);
	HISTDIR[icrate][ich] = crdir->mkdir(chdirname);
      }
    }
  }
  
  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      HISTDIR[icrate][ich]->cd();
      char bank[5]; sprintf(bank, "D%d%02d", icrate, ich);
      if (!wfd_map.count(bank))
	continue;
      const std::vector<TPulseIsland*>& wfd = wfd_map.at(bank);
      for (int ipulse = 0; ipulse < wfd.size(); ++ipulse) {
	char name[64]; sprintf(name, "%s_Ev%04d_Hit%05d", bank, evt, ipulse);
	const std::vector<int>& samps = wfd[ipulse]->GetSamples();
	TH1* hist = new TH1D(name, name, samps.size(), 0, samps.size());
	for (int isamp = 0; isamp < samps.size(); ++isamp)
	  hist->Fill(isamp, samps[isamp]);
	hist->Write();
	delete hist;
      }
    }
  }
  cwd->cd();
  return SUCCESS;
}

/// @}
