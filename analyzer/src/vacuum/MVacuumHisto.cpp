/********************************************************************\

Name:         MVacuumHisto.cpp
Created by:   Chen Wu

Contents:     Create histograms for vacuum monitor

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <time.h>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>
#include <TH2.h>

/* AlCap includes */
//#include "TOctalFADCIsland.h"
//#include "TOctalFADCBankReader.h"
#include "TVacuumData.h"

#define MAX_PRESSURE_POINTS 7200

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MVacuumHisto_init(void);
INT  MVacuumHisto(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TVacuumData* gVacuum;

static TH1F* hVacuumPressure;
static TH1I* hVacuumStatus;

ANA_MODULE MVacuumHisto_module =
{
	"MVacuumHisto",        /* module name           */
	"Chen Wu",              /* author                */
	MVacuumHisto,          /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MVacuumHisto_init,     /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
 */
INT MVacuumHisto_init()
{
	hVacuumPressure = new TH1F("Vacuum_Pressure","Pressure (mbar)",MAX_PRESSURE_POINTS,-(double)MAX_PRESSURE_POINTS/60,0);
	hVacuumPressure->GetXaxis()->SetTitle("Time to now (min)");
	hVacuumPressure->GetYaxis()->SetTitle("Pressure (mbar)");
	hVacuumStatus = new TH1I("Vacuum_Status","Status",MAX_PRESSURE_POINTS,-(double)MAX_PRESSURE_POINTS/60,0);
	hVacuumStatus->GetXaxis()->SetTitle("Time to now (min)");
	hVacuumStatus->GetYaxis()->SetTitle("Status of Vacuum Gauge");

	return SUCCESS;
}

INT MVacuumHisto(EVENT_HEADER *pheader, void *pevent)
{
	// Get the event number
	//int midas_event_number = pheader->serial_number;

	// Fill Diagnostic histogram
	//hNOctalFADCIslandsReadPerBlock->Fill(bank_name.c_str(), midas_event_number,
	//fadc_islands.size());

	float *ppressure;
	INT *pstatus;

	//  printf("In caen ER!\n");

	char bank_name[8];

	int eventid = EVENT_ID(pevent);
	if (eventid != 24) return SUCCESS;

	time_t block_time = pheader->time_stamp;
	int time_sec = (int) block_time;

//	char banklist[STRING_BANKLIST_MAX];
//	int nbanks = bk_list(pevent, banklist);
//	printf("nbanks = %d, %s\n", nbanks, banklist);
	printf("eventid = %d\n",eventid);
	int midas_event_number = pheader->serial_number;
	printf("serial_number = %d\n", midas_event_number);
//	char fdate[] = "%Y-%m-%d", ftime[] = "%H:%M:%S";
//	struct tm block_tm;
//	localtime_r(&block_time, &block_tm);
//	char sdate[100], stime[100];
//	strftime(sdate, 100, fdate, &block_tm);
//	strftime(stime, 100, ftime, &block_tm);
//	printf("Current block stored (%d) on %s at %s\n", 
//			time_sec, sdate, stime);

	sprintf(bank_name,"PRM%i",0); // only one gauge
	unsigned int bank_len = bk_locate(pevent, "PRM0", &ppressure);
//	printf("MIDAS bank [%s] size %d, %d, %f----------------------------------------\n",bank_name,bank_len,ppressure==NULL,ppressure==NULL?-456:*ppressure);

	sprintf(bank_name,"PRS%i",0); // only one gauge
	bank_len = bk_locate(pevent, bank_name, &pstatus);
//	printf("MIDAS bank [%s] size %d, %d , %d----------------------------------------\n",bank_name,bank_len,pstatus==NULL,pstatus==NULL?-123:*pstatus);

	// By default, banks size is 1
	if (ppressure!=NULL&&pstatus!=NULL){
		gVacuum->AddPoint(time_sec, *pstatus, *ppressure);
		for (int ibin = 1; ibin <= MAX_PRESSURE_POINTS; ibin++){
			if (ibin!=MAX_PRESSURE_POINTS){
				hVacuumPressure->SetBinContent(ibin,hVacuumPressure->GetBinContent(ibin+1));
				hVacuumStatus->SetBinContent(ibin,hVacuumPressure->GetBinContent(ibin+1));
			}
			else{
				hVacuumPressure->SetBinContent(ibin,*ppressure);
				hVacuumStatus->SetBinContent(ibin,*pstatus);
			}
		}
	}
	else{
		printf("###!!!Cannot find point!!!###\n");
	}

	return SUCCESS;
}
