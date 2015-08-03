/********************************************************************\

Name:         MDT5720Errors
Created by:   John R Quirk

Contents:     Look at PLL lock and board full bits

\********************************************************************/

/* Standard includes */
#include <stdio.h>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>
#include "TDirectory.h"

/*-- Module declaration --------------------------------------------*/
INT MDT5720Errors_init(void);
INT MDT5720Errors_bor(INT);
INT MDT5720Errors(EVENT_HEADER*, void*);

extern HNDLE hDB;

static TH1* hDT5720Errors;

ANA_MODULE MDT5720Errors_module =
{
	"MDT5720Errors",    /* module name           */
	"John R Quirk",     /* author                */
	MDT5720Errors,      /* event routine         */
	MDT5720Errors_bor,  /* BOR routine           */
	NULL,               /* EOR routine           */
	MDT5720Errors_init, /* init routine          */
	NULL,               /* exit routine          */
	NULL,               /* parameter structure   */
	0,                  /* structure size        */
	NULL,               /* initial parameters    */
};

enum BOARDERROR {
  PLL_LOSS = 1,
  BOARD_FULL = 2,
  INTERNAL_TIMEOUT = 3,
  OVER_TEMPERATURE = 4,
  NO_ADC_POWER = 5
};

INT MDT5720Errors_init() {
  TDirectory* cwd = gDirectory;
  if (!gDirectory->Cd("DataQuality_LowLevel"))
    gDirectory->mkdir("DataQuality_LowLevel/")->cd();
  gDirectory->mkdir("DT5720Errors/")->cd();

  hDT5720Errors = new TH1I("hDT5720Errors", "Errors in DT5720", 5, 0., 5.);
  hDT5720Errors->GetXaxis()->SetBinLabel(PLL_LOSS, "PLL Loss");
  hDT5720Errors->GetXaxis()->SetBinLabel(BOARD_FULL, "Board Full");
  hDT5720Errors->GetXaxis()->SetBinLabel(INTERNAL_TIMEOUT, "Internal Communication Timeout");
  hDT5720Errors->GetXaxis()->SetBinLabel(OVER_TEMPERATURE, "Over Temperature");
  hDT5720Errors->GetXaxis()->SetBinLabel(NO_ADC_POWER, "No ADC Power (Hot)");
  hDT5720Errors->GetYaxis()->SetTitle("Occurences in Run");

  cwd->cd();

  return SUCCESS;
}

INT MDT5720Errors_bor(INT run_number) {
  hDT5720Errors->Reset();
  return SUCCESS;
}

INT MDT5720Errors(EVENT_HEADER *pheader, void *pevent) {
  BYTE* pdata;
  std::string bank("CNS0");
  const int bank_len = bk_locate(pevent, bank.c_str(), &pdata);
  if (bank_len > 0) {
    if (pdata[0])
      hDT5720Errors->Fill(PLL_LOSS);
    if (pdata[1])
      hDT5720Errors->Fill(BOARD_FULL);
    if (bank_len > 2) {
      if (pdata[2])
        hDT5720Errors->Fill(INTERNAL_TIMEOUT);
      if (pdata[3])
        hDT5720Errors->Fill(OVER_TEMPERATURE);
      if (pdata[4])
        hDT5720Errors->Fill(NO_ADC_POWER);
    }
  }
  return SUCCESS;
}
