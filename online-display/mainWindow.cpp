#include <time.h>
#include <iostream>
#include <fstream>
// test
#include "TApplication.h"
#include "TGButton.h"
#include "TRootEmbeddedCanvas.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TThread.h"
#include "TPaveText.h"
#include "TPad.h"
#include "TGComboBox.h"
#include "TGLabel.h"
#include "TMath.h"

#include "getHist.h"

#define B_CHANGE_PAD -1
#define B_CHANGE_ERANGE -2

class TOnlineFrame : public TGMainFrame {

private:
  TGHorizontalFrame *fTopFrame;
  TGHorizontalFrame **fScreenFrames;
  TGButton *fUpdateButton;
  TGButton *fPrintButton;
  TRootEmbeddedCanvas *fEmbeddedCanvas;
  TGCheckButton *fCycleDisplays;
  TPaveText *fText;
  TPad *fTextPad;

  TGLabel *channelSelectionLabel;
  TGLabel *channelSelectionLabel2;
  TGHorizontalFrame *fExtraToolframe;

  int fCurrentDisplay;

  int fAnalyzerPad;
  int fPadERange;
  
public:
  TOnlineFrame(const TGWindow * p, const char* title);
  ~TOnlineFrame();
  
  void CloseWindow();
  Bool_t ProcessMessage(Long_t msg, Long_t param1, Long_t param2);
  void ConsiderCycling();
  void runMacro(const char *macro);

  TGComboBox *PadSelectionComboBox;
  TGComboBox *ERangeSelectionComboBox;

};

struct screen_info {
    const char *visibleName;
    const char *macroName;
};


struct screen_info screens[] = {
    {"Raw", "raw_parameter_display.C"},
    // Add more displays here after writing a ROOT macro, eg:
    {"ThinSiHeights", "thin_silicon_heights.C"},
    // {"FADC_display", "fadc_display.C"},
};

const int num_online=sizeof(screens) / sizeof(screen_info);

int width = 1200;
int height = 600;

int num_screens = sizeof(screens) / sizeof(screens[0]);

const int kButtonsPerRow = 12;
const int kNumRows = (int)TMath::Ceil(num_screens / (double) kButtonsPerRow);

#define B_UPDATE 1
#define B_CYCLE 2
#define B_PRINT 3   // SMC
#define SCREENS_BASE 10


TOnlineFrame::TOnlineFrame(const TGWindow * p, const char* title) 
  :TGMainFrame(p, width,height)
{

    num_screens = num_online;

    // a little frame at the top
    fTopFrame = new TGHorizontalFrame(this,width - 5, 30, kFixedWidth);
    AddFrame(fTopFrame);

    // an "update" button
    fUpdateButton = new TGTextButton(fTopFrame, "Update", B_UPDATE);
    fUpdateButton->Associate(this);
    fTopFrame->AddFrame(fUpdateButton, new TGLayoutHints(kLHintsLeft));

    // a menu for changing screens
    /*-- Editted on 11Oct2010 by MM for more button rows Backup:mainWindow.cpp.11Oct2010--*/
    fScreenFrames = new TGHorizontalFrame*[kNumRows];
    for(int j=0;j<kNumRows;j++){
      if(j==0) {
        fScreenFrames[j] = new TGHorizontalFrame(fTopFrame,1000,28,kFixedWidth);
        fTopFrame->AddFrame(fScreenFrames[j], new TGLayoutHints(kLHintsCenterX));
      
      }  else {
        fScreenFrames[j] = new TGHorizontalFrame(this,1000,28,kFixedWidth);
        AddFrame(fScreenFrames[j], new TGLayoutHints(kLHintsCenterX));
      }
    }
    for (int i=0; i < num_screens; i++) {
        TGTextButton *b = new TGTextButton(fScreenFrames[i/kButtonsPerRow], 
                                           screens[i].visibleName,
                		                       SCREENS_BASE + i);
      b->Associate(this);
	    fScreenFrames[i/kButtonsPerRow]->AddFrame(b, new TGLayoutHints(kLHintsLeft));
    }


    /////////////////////////////////////////////////////////////////////

    //Extra tool bar frame incase some of the modules need it
    fExtraToolframe = new TGHorizontalFrame(this, width - 10, 30, kFixedWidth |kRaisedFrame);
    AddFrame(fExtraToolframe);

    //Pad Number Selection Box///////////////////////////////////////

    PadSelectionComboBox = new TGComboBox(fExtraToolframe, "all", B_CHANGE_PAD);
    PadSelectionComboBox->SetWidth(50);
    PadSelectionComboBox->SetHeight(20);
    
    char buffer[5];
    int PadNumber=48;

    PadSelectionComboBox->AddEntry("all", 0);

    for (int i=1;i!=PadNumber+1;++i) {

      sprintf(buffer,"%d",i);

      PadSelectionComboBox->AddEntry(buffer, i);
    }

    PadSelectionComboBox->Associate(this);
    fExtraToolframe->AddFrame(PadSelectionComboBox, new TGLayoutHints(kLHintsRight));

    channelSelectionLabel = new TGLabel(fExtraToolframe, "\tPI of Pad #: ");
    fExtraToolframe->AddFrame(channelSelectionLabel, new TGLayoutHints(kLHintsRight));

    fAnalyzerPad = 0; //start at showing all pulses
    ////////////////////////////////////////////////////////////////////

    //Pad Number Selection Box///////////////////////////////////////

    ERangeSelectionComboBox = new TGComboBox(fExtraToolframe, "all", B_CHANGE_ERANGE);
    ERangeSelectionComboBox->SetWidth(60);
    ERangeSelectionComboBox->SetHeight(20);

    char buffer2[10];
    int ERanges=4;

    ERangeSelectionComboBox->AddEntry("all", 0);

    for (int i=0;i!=ERanges;++i) {

      sprintf(buffer2,"E%d-E%d",i,i+1);

      ERangeSelectionComboBox->AddEntry(buffer2, i+1);
    }

    ERangeSelectionComboBox->Associate(this);
    fExtraToolframe->AddFrame(ERangeSelectionComboBox, new TGLayoutHints(kLHintsRight));

    channelSelectionLabel2 = new TGLabel(fExtraToolframe, "\tERange of Pad: ");
    fExtraToolframe->AddFrame(channelSelectionLabel2, new TGLayoutHints(kLHintsRight));

    fPadERange = 0; //start at showing all pulses

    // a "cycle" checkbox
    fCycleDisplays = new TGCheckButton(fTopFrame, "Cycle", B_CYCLE);
    fCycleDisplays->Associate(this);
    fTopFrame->AddFrame(fCycleDisplays, new TGLayoutHints(kLHintsRight));

    // a "print" button
    fPrintButton = new TGTextButton(fTopFrame, "Print to File", B_PRINT);
    fPrintButton->Associate(this);
    fTopFrame->AddFrame(fPrintButton, new TGLayoutHints(kLHintsRight));

    // our big embedded canvas
    fEmbeddedCanvas =
	new TRootEmbeddedCanvas("Canvas", this, width - 10, height - 32);
    AddFrame(fEmbeddedCanvas);


    //AddFrame(fText);
    fTextPad = new TPad("fPad", "pad", 0.01, 0.01, 0.99, 0.99);
    fEmbeddedCanvas->GetCanvas()->cd();
    fTextPad->Draw();
    fTextPad->cd();
    fText = new TPaveText(0.2,0.52,0.85,0.68,"textt");
    char text[40];
    sprintf(text,"Welcome to AlCap Online Display");
    fText->AddText(text);
    fText->Draw();

    SetWindowName(title);
    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();
}

TOnlineFrame::~TOnlineFrame()
{
}

Bool_t TOnlineFrame::ProcessMessage(Long_t msg, Long_t param1,
				    Long_t param2)
{
    switch (GET_MSG(msg)) {
    case kC_COMMAND:

      switch (GET_SUBMSG(msg)) {

      case kCM_BUTTON:
	if(param1 >= SCREENS_BASE) {
	  fCurrentDisplay = param1 - SCREENS_BASE;
	  printf("fCurrentDisplay: %d \n",fCurrentDisplay); 


	}
	else if(param1 == B_PRINT) {  // SMC
	  // for pdf  exchange next two lines
	  bool png=1;
	  char pdfcommand[2000]="convert";

	  //bool png=0;
	  //char pdfcommand[2000]="gs -dBATCH -dNOPAUSE -sDEVICE=pdfwrite -sOutputFile=online.pdf";

	  for (int fCurr=0; fCurr < num_online; fCurr++){
	    runMacro(screens[fCurr].macroName);
	    char outfile[80]="";
	    if(png)
	      sprintf(outfile,"%s.png",screens[fCurr].visibleName);
	    else
	      sprintf(outfile,"%s.pdf",screens[fCurr].visibleName);
	    sprintf(pdfcommand,"%s %s",pdfcommand,outfile);
	    fEmbeddedCanvas->GetCanvas()->Print(outfile);
	  }

	  if(png)
	    sprintf(pdfcommand,"%s %s",pdfcommand,"online.pdf");
	  printf("%s \n", pdfcommand);
	  gSystem->Exec(pdfcommand);
	}

	break;

      case kCM_COMBOBOX:

	std::ofstream numfile;

	if (param1 == B_CHANGE_PAD)
	  {
	    fAnalyzerPad = param2;

	    numfile.open("boxnumber.txt");
	    numfile<<fAnalyzerPad;
	    numfile.close();
	  }

	else if (param1 == B_CHANGE_ERANGE)
	  {
	    fPadERange = param2;

	    numfile.open("ERangenumber.txt");
	    numfile<<fPadERange;
	    numfile.close();
	  }

	break;


      }

     if(param1 != B_PRINT) runMacro(screens[fCurrentDisplay].macroName);

    }

    return false;
}

void TOnlineFrame::CloseWindow()
{
   DeleteWindow();
   gApplication->Terminate(0);
   exit(1);
}

void TOnlineFrame::ConsiderCycling()
{
  if(fCycleDisplays->GetState() == kButtonDown) {
    fCurrentDisplay = fCurrentDisplay % num_screens;
    const char *macro = screens[fCurrentDisplay].macroName;
    runMacro(macro);
  }
}

void TOnlineFrame::runMacro(const char *macro)
{
  fEmbeddedCanvas->GetCanvas()->Clear();
  gROOT->Macro(macro);
}

int main(int argc, char **argv)
{
    int fake_argc = 1;
    char *fake_argv[] = {"./online-display"};

    TApplication theApp("RMidas", &fake_argc, fake_argv);

    if (gROOT->IsBatch()) {
	printf("%s: cannot run in batch mode\n", argv[0]);
	return 1;
    }

    if(argc < 2) {
      openHistSocket("cheetah");
    } else {
      openHistFile(argv[1]);
    }

    std::ofstream numfile;
    numfile.open("boxnumber.txt");
    numfile<<0;
    numfile.close();

    numfile.open("ERangenumber.txt");
    numfile<<0;
    numfile.close();

    char windowTitle[100]; 
    if(argc < 2) {
      sprintf(windowTitle,"MuSun Online Browser - Current Run");
    } else {
      sprintf(windowTitle,"MuSun Online Browser %s", argv[1]);
    }

    TOnlineFrame *onlineFrame = new TOnlineFrame(gClient->GetRoot(),
      windowTitle);

    time_t lastCycleTime = time(NULL);
    while(true) {
      theApp.Run(1);
      if(time(NULL) > lastCycleTime + 2) {
	printf("Considering cycling\n");
        onlineFrame->ConsiderCycling(); 
	lastCycleTime = time(NULL);
      } else {
        usleep(10000);
      }
    }

    return 0;
}
