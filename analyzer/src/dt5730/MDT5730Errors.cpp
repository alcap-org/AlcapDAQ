/********************************************************************\

Name:         MDT5730Errors
Created by:   John R Quirk

Contents:     Look at PLL lock and board fail bits

\********************************************************************/

/* Standard includes */
#include <stdio.h>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>

/*-- Module declaration --------------------------------------------*/
INT MDT5730Errors_init(void);
INT MDT5730Errors_bor(INT);
INT MDT5730Errors(EVENT_HEADER*, void*);

extern HNDLE hDB;

static TH1* hDT5730Errors;

ANA_MODULE MDT5730Errors_module =
{
	"MDT5730Errors",    /* module name           */
	"John R Quirk",     /* author                */
	MDT5730Errors,      /* event routine         */
	MDT5730Errors_bor,  /* BOR routine           */
	NULL,               /* EOR routine           */
	MDT5730Errors_init, /* init routine          */
	NULL,               /* exit routine          */
	NULL,               /* parameter structure   */
	0,                  /* structure size        */
	NULL,               /* initial parameters    */
};


INT MDT5730Errors_init() {
  hDT5730Errors = new TH1I("hDT5730Errors", "Errors in DT5730", 2, 0., 2.);
  hDT5730Errors->GetXaxis()->SetBinLabel(1, "PLL Loss");
  hDT5730Errors->GetXaxis()->SetBinLabel(2, "Board Full");
  hDT5730Errors->GetYaxis()->SetTitle("Occurences in Run");
  return SUCCESS;
}

INT MDT5730Errors_bor(INT run_number) {
  hDT5730Errors->Reset();
  return SUCCESS;
}

INT MDT5730Errors(EVENT_HEADER *pheader, void *pevent) {
  BOOL* pdata;
  std::string bank("CNS1");
  const int bank_len = bk_locate(pevent, bank.c_str(), &pdata);
  if (bank_len > 0) {
    if (pdata[0])
      hDT5730Errors->Fill(0);
    if (pdata[1])
      hDT5730Errors->Fill(1);
  }
  return SUCCESS;
}
