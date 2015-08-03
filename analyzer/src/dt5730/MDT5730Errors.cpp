/********************************************************************\

Name:         MDT5730Errors
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

enum BOARDERROR {
  PLL_LOSS = 1,
  BOARD_FULL = 2,
  INTERNAL_TIMEOUT = 3,
  OVER_TEMPERATURE = 4,
  NO_ADC_POWER = 5
};

INT MDT5730Errors_init() {
  TDirectory* cwd = gDirectory;
  if (!gDirectory->Cd("DataQuality_LowLevel"))
    gDirectory->mkdir("DataQuality_LowLevel/")->cd();
  gDirectory->mkdir("DT5730Errors/")->cd();

  hDT5730Errors = new TH1I("hDT5730Errors", "Errors in DT5730", 5, 0., 5.);
  hDT5730Errors->GetXaxis()->SetBinLabel(PLL_LOSS, "PLL Loss");
  hDT5730Errors->GetXaxis()->SetBinLabel(BOARD_FULL, "Board Full");
  hDT5730Errors->GetXaxis()->SetBinLabel(INTERNAL_TIMEOUT, "Internal Communication Timeout");
  hDT5730Errors->GetXaxis()->SetBinLabel(OVER_TEMPERATURE, "Over Temperature");
  hDT5730Errors->GetXaxis()->SetBinLabel(NO_ADC_POWER, "No ADC Power (Hot)");
  hDT5730Errors->GetYaxis()->SetTitle("Occurences in Run");

  cwd->cd();

  return SUCCESS;
}

INT MDT5730Errors_bor(INT run_number) {
  hDT5730Errors->Reset();
  return SUCCESS;
}

INT MDT5730Errors(EVENT_HEADER *pheader, void *pevent) {
  BYTE* pdata;
  std::string bank("CNS1");
  const int bank_len = bk_locate(pevent, bank.c_str(), &pdata);
  if (bank_len > 0) {
    if (pdata[0])
      hDT5730Errors->Fill(PLL_LOSS);
    if (pdata[1])
      hDT5730Errors->Fill(BOARD_FULL);
    if (bank_len > 2) {
      if (pdata[2])
        hDT5730Errors->Fill(INTERNAL_TIMEOUT);
      if (pdata[3])
        hDT5730Errors->Fill(OVER_TEMPERATURE);
      if (pdata[4])
        hDT5730Errors->Fill(NO_ADC_POWER);
    }
  }
  return SUCCESS;
}
