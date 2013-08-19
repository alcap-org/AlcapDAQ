/********************************************************************\

  Name:         fmidas.c
  Created by:   Stefan Ritt

  Contents:     ROOT based Midas GUI for histo display and experiment
                control

  $Log: rmidas.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:37  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.5  2004/01/08 08:40:10  midas
  Implemented standard indentation

  Revision 1.4  2003/04/28 15:28:40  midas
  Added text entry box

  Revision 1.3  2003/04/25 14:36:49  midas
  Added clear button

  Revision 1.2  2003/04/17 15:12:02  midas
  Added tab panels

  Revision 1.1  2003/04/17 14:18:03  midas
  Initial revision

\*******************************************************************/

#include "TGButton.h"
#include "TRootEmbeddedCanvas.h"
#include "TGLayout.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TSocket.h"
#include "TMessage.h"
#include "TGMsgBox.h"
#include "TApplication.h"
#include "TROOT.h"
#include "TGListBox.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TGMenu.h"
#include "TGTab.h"
#include "TGLabel.h"
#include "TGTextEntry.h"

#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif

/*------------------------------------------------------------------*/

class TGTextDialog:public TGTransientFrame {

 protected:
   TGCompositeFrame * fF1, *fF2;        // sub frames
   TGButton *fOkButton;         // ok button
   TGButton *fCancelButton;     // cancel button
   TGLayoutHints *fL1, *fL5, *fL6, *fL21;       // layout hints
   TGTextEntry *fText;          // text entry widget
   TGTextBuffer *fBLabel;       // text buffer
   TGLabel *fLabel;             // label
   char *fRetStr;               // return string

 public:
    TGTextDialog(const TGWindow * p, const TGWindow * main, UInt_t w, UInt_t h,
                 char *label, char *ret_str, UInt_t options = kVerticalFrame);
    virtual ~ TGTextDialog();

   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

   //ClassDef(TGTextDialog, 0)
};

//ClassImp(TGTextDialog)

/*------------------------------------------------------------------*/

TGTextDialog::TGTextDialog(const TGWindow * p, const TGWindow * main,
                           UInt_t w, UInt_t h, char *label, char *ret_str,
                           UInt_t options):TGTransientFrame(p, main, w, h, options)
{
/* Create a dialog to enter a single line text entry */

   fRetStr = ret_str;

   ChangeOptions((GetOptions() & ~kVerticalFrame) | kHorizontalFrame);

   fF1 = new TGCompositeFrame(this, 60, 20, kVerticalFrame | kFixedWidth);
   fF2 = new TGCompositeFrame(this, 60, 20, kHorizontalFrame);

   fOkButton = new TGTextButton(fF1, new TGHotString("&Ok"), 1);
   fCancelButton = new TGTextButton(fF1, new TGHotString("&Cancel"), 2);
   fF1->Resize(fOkButton->GetDefaultWidth() + 40, GetDefaultHeight());

   fOkButton->Associate(this);
   fCancelButton->Associate(this);

   fL1 = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 3, 0);
   fL21 = new TGLayoutHints(kLHintsTop | kLHintsRight, 2, 5, 10, 0);

   fF1->AddFrame(fOkButton, fL1);
   fF1->AddFrame(fCancelButton, fL1);
   AddFrame(fF1, fL21);

   fLabel = new TGLabel(fF2, new TGHotString(label));

   fBLabel = new TGTextBuffer(50);
   if (fRetStr[0])
      fBLabel->AddText(0, fRetStr);
   else
      fOkButton->SetState(kButtonDisabled);

   fText = new TGTextEntry(fF2, fBLabel);
   fText->Associate(this);
   fText->Resize(220, fText->GetDefaultHeight());
   fText->SelectAll();

   fL5 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 3, 5, 0, 0);
   fL6 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 0, 2, 0, 0);

   fF2->AddFrame(fLabel, fL5);
   fF2->AddFrame(fText, fL5);
   AddFrame(fF2, fL1);

   MapSubwindows();
   Resize(GetDefaultSize());

   // position relative to the parent's window
   Int_t ax, ay;
   if (main) {
      Window_t wdum;
      gVirtualX->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
                                      (Int_t) (((TGFrame *) main)->GetWidth() -
                                               fWidth) >> 1,
                                      (Int_t) (((TGFrame *) main)->GetHeight() -
                                               fHeight) >> 1, ax, ay, wdum);
   } else {
      UInt_t root_w, root_h;
      gVirtualX->GetWindowSize(fClient->GetRoot()->GetId(), ax, ay, root_w, root_h);
      ax = (root_w - fWidth) >> 1;
      ay = (root_h - fHeight) >> 1;
   }

   Move(ax, ay);
   SetWMPosition(ax, ay);

   SetWindowName("Enter Text");

   SetMWMHints(kMWMDecorAll | kMWMDecorMaximize | kMWMDecorMenu,
               kMWMFuncAll | kMWMFuncMaximize | kMWMFuncResize, kMWMInputModeless);

   MapWindow();
   fClient->WaitFor(this);
}

/*------------------------------------------------------------------*/

TGTextDialog::~TGTextDialog()
{
   // Clean up text dialog

   delete fCancelButton;
   delete fOkButton;
   delete fText;
   delete fLabel;
   delete fF1;
   delete fF2;
   delete fL1;
   delete fL5;
   delete fL6;
   delete fL21;
}

/*------------------------------------------------------------------*/

void TGTextDialog::CloseWindow()
{
   // Close the dialog. On close the dialog will be deleted and cannot be
   // re-used.

   DeleteWindow();
}

Bool_t TGTextDialog::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   const char *string;

   switch (GET_MSG(msg)) {
   case kC_COMMAND:
      switch (GET_SUBMSG(msg)) {
      case kCM_BUTTON:
         switch (parm1) {
         case 1:
            if (fRetStr)
               strcpy(fRetStr, fBLabel->GetString());
            CloseWindow();
            break;
         case 2:
            fRetStr[0] = 0;
            CloseWindow();
            break;
         }
         break;

      default:
         break;
      }
      break;

   case kC_TEXTENTRY:
      switch (GET_SUBMSG(msg)) {
      case kTE_TEXTCHANGED:
         string = fBLabel->GetString();
         if (strlen(string) == 0)
            fOkButton->SetState(kButtonDisabled);
         else
            fOkButton->SetState(kButtonUp);
         break;
      case kTE_ENTER:
         if (fRetStr)
            strcpy(fRetStr, fBLabel->GetString());
         CloseWindow();
         break;
      default:
         break;
      }
      break;

   default:
      break;
   }

   return kTRUE;
}

/*------------------------------------------------------------------*/

enum RMidasCommandIdentifiers {
   M_FILE_EXIT,
   M_FILE_CONNECT,
   B_UPDATE,
   B_CLEAR,
};

/*------------------------------------------------------------------*/

class RMidasFrame:public TGMainFrame {

 private:
   TGMenuBar * fMenuBar;
   TGPopupMenu *fMenuFile;

   TGTab *fTab;
   TGCompositeFrame *fTabHisto;
   TGCompositeFrame *fTabStatus;

   TGHorizontalFrame *fHorz1;
   TGHorizontalFrame *fHorz2;
   TGHorizontalFrame *fHorz3;

   TGListBox *fListBox;
   TRootEmbeddedCanvas *fCanvas;

   TGTextButton *fBUpdate;
   TGTextButton *fBClear;

   char fHost[256];             // remote host name
   TSocket *fSock;

   TObjArray *fHistoNames;
   TH1 *fHisto;

   int ConnectServer();
   void GetHistoList();
   void GetHisto(const char *name);
   void ClearHisto(const char *name);

 public:
   RMidasFrame(const TGWindow * p, UInt_t w, UInt_t h, char *host = "localhost");
   ~RMidasFrame();

   void CloseWindow();
   Bool_t ProcessMessage(Long_t msg, Long_t param1, Long_t param2);
};

/*------------------------------------------------------------------*/

int RMidasFrame::ConnectServer()
{
   char str[80];

   if (fSock) {
      /* disconnect first */
      fSock->Close();
      delete fSock;
      fSock = NULL;

      SetWindowName("RMidas");

      fListBox->RemoveEntries(0, 999);
   }

   if (fHost[0] == 0) {
      new TGTextDialog(gClient->GetRoot(), this, 100, 100, "&Host name:", fHost);
   }

   /* Connect to RMidasServ */
   fSock = new TSocket(fHost, 9090);

   if (!fSock->IsValid()) {
      sprintf(str, "Cannot connect to RMidas server on host %s, port 9090\n", fHost);
      new TGMsgBox(gClient->GetRoot(), this, "Error", str, kMBIconExclamation, 0, NULL);

      SetWindowName("RMidas");
      return 0;
   } else {
      fSock->Recv(str, sizeof(str));
      if (strncmp(str, "RMSERV", 6)) {
         sprintf(str, "RMidas server not found on host %s, port 9090\n", fHost);
         new TGMsgBox(gClient->GetRoot(), this, "Error", str, kMBIconExclamation, 0,
                      NULL);

         fSock->Close();
         delete fSock;
         fSock = NULL;

         SetWindowName("RMidas");
         return 0;
      } else {
         sprintf(str, "RMidas connected to %s", fHost);
         SetWindowName(str);
         GetHistoList();
         return 1;
      }
   }
}

/*------------------------------------------------------------------*/

void RMidasFrame::GetHistoList()
{
   int i;

   fSock->Send("LIST");

   TMessage *m;
   fSock->Recv(m);

   if (fHistoNames)
      delete fHistoNames;

   fHistoNames = (TObjArray *) m->ReadObject(m->GetClass());

   for (i = 0; i < fHistoNames->GetLast() + 1; i++)
      fListBox->AddEntry((const char *) ((TObjString *) (fHistoNames->At(i)))->String(),
                         i);

   fListBox->MapSubwindows();
   fListBox->Layout();
   delete m;
}

/*------------------------------------------------------------------*/

void RMidasFrame::GetHisto(const char *name)
{
   TMessage *m;
   char str[32];

   sprintf(str, "GET %s", name);
   fSock->Send(str);
   fSock->Recv(m);

   if (fHisto)
      delete fHisto;

   fHisto = (TH1 *) m->ReadObject(m->GetClass());

   if (!fHisto)
      printf("Histo \"%s\" not available\n", name);
   else
      fHisto->Draw();

   fCanvas->GetCanvas()->Modified();
   fCanvas->GetCanvas()->Update();

   delete m;
}

/*------------------------------------------------------------------*/

void RMidasFrame::ClearHisto(const char *name)
{
   char str[32];

   sprintf(str, "CLEAR %s", name);
   fSock->Send(str);
   fSock->Recv(str, sizeof(str));
}

/*------------------------------------------------------------------*/

RMidasFrame::RMidasFrame(const TGWindow * p, UInt_t w, UInt_t h,
                         char *host):TGMainFrame(p, w, h)
{
   /* save host name */
   if (host)
      strcpy(fHost, host);
   else
      fHost[0] = 0;
   fSock = NULL;

   /* Create Menus */
   fMenuFile = new TGPopupMenu(fClient->GetRoot());
   fMenuFile->AddEntry("&Connect to ...", M_FILE_CONNECT);
   fMenuFile->AddEntry("E&xit", M_FILE_EXIT);
   fMenuFile->Associate(this);

   fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File", fMenuFile,
                      new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));

   AddFrame(fMenuBar,
            new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1));

   /* Create tab widget */
   fTab = new TGTab(this, 600, 400);

  /*---- histo tab ----*/

   TGCompositeFrame *tf = fTab->AddTab("Histo");
   fTabHisto = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);

   /* Create a horizonal frame containing histo selection tree and canvas */
   fHorz1 = new TGHorizontalFrame(fTabHisto, 600, 400);
   fTabHisto->AddFrame(fHorz1,
                       new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 3, 3, 3, 0));

   /* Create ListBox widget */
   fListBox = new TGListBox(fHorz1, 1);
   fListBox->Resize(100, 400);
   fListBox->Associate(this);
   fHorz1->AddFrame(fListBox, new TGLayoutHints(kLHintsExpandY, 0, 0, 0, 0));

   /* Create an embedded canvas and add to the main frame, centered in x and y */
   fCanvas = new TRootEmbeddedCanvas("Canvas", fHorz1, 400, 400);
   fHorz1->AddFrame(fCanvas,
                    new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));

   /* Create a horizonal frame containing text buttons */
   fHorz2 = new TGHorizontalFrame(fTabHisto, 600, 30);
   fTabHisto->AddFrame(fHorz2, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));

   /* Create "Update" button */
   fBUpdate = new TGTextButton(fHorz2, "Update", B_UPDATE);
   fBUpdate->Associate(this);
   fHorz2->AddFrame(fBUpdate, new TGLayoutHints(kLHintsCenterX, 10, 10, 4, 4));

   /* Create "Clear" button */
   fBClear = new TGTextButton(fHorz2, "Clear", B_CLEAR);
   fBClear->Associate(this);
   fHorz2->AddFrame(fBClear, new TGLayoutHints(kLHintsCenterX, 10, 10, 4, 4));

   tf->AddFrame(fTabHisto, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 0, 0));

  /*---- status tab ----*/

   tf = fTab->AddTab("Status");
   fTabStatus = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);
   /*
      fHorz3 = new TGHorizontalFrame(fTabStatus, 600, 400);
      fTabStatus->AddFrame(fHorz3, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY, 0, 0, 0, 0));
      fHorz3->AddFrame(new TGLabel(fTabStatus, "Test"), new TGLayoutHints(kLHintsExpandX|kLHintsExpandY, 0, 0, 0, 0));
    */
   fTabStatus->
       AddFrame(new TGLabel(fTabStatus, "Here will come the experiment status display"),
                new TGLayoutHints(kLHintsTop, 100, 100, 100, 0));
   tf->AddFrame(fTabStatus, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 0, 0));

  /*--------------------*/

   AddFrame(fTab, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));

   SetWindowName("RMidas");

   MapSubwindows();
   Resize(GetDefaultSize());
   MapWindow();

   fHisto = 0;
   fHistoNames = 0;

   /* connect to MRootServer */
   ConnectServer();
}

void RMidasFrame::CloseWindow()
{
   // Got close message for this MainFrame. Terminate the application
   // or returns from the TApplication event loop (depending on the
   // argument specified in TApplication::Run()).

   gApplication->Terminate(0);
}

Bool_t RMidasFrame::ProcessMessage(Long_t msg, Long_t param1, Long_t param2)
{
   // Process messages coming from widgets associated with the dialog.

   switch (GET_MSG(msg)) {
   case kC_COMMAND:

      switch (GET_SUBMSG(msg)) {
      case kCM_MENU:
         switch (param1) {
         case M_FILE_EXIT:
            CloseWindow();
            break;

         case M_FILE_CONNECT:
            fHost[0] = 0;
            ConnectServer();
            break;
         }
         break;

      case kCM_BUTTON:
         switch (param1) {
         case B_UPDATE:
            if (fHisto)
               GetHisto((char *) fHisto->GetName());
            break;

         case B_CLEAR:
            if (fHisto) {
               ClearHisto((char *) fHisto->GetName());
               GetHisto((char *) fHisto->GetName());
            }
            break;
         }
         break;

      case kCM_LISTBOX:
         GetHisto((const char *) ((TObjString *) (fHistoNames->At(param2)))->String());
         break;


      case kCM_TAB:
         //printf("Tab item %ld activated\n", param1);
         break;
      }
      break;

   }

   return kTRUE;
}

RMidasFrame::~RMidasFrame()
{
   // Clean up

   delete fMenuBar;
   delete fMenuFile;
   delete fHisto;
   delete fHistoNames;
   delete fSock;
   delete fBUpdate;
   delete fHorz1;
   delete fHorz2;
   delete fCanvas;
}

void rmidas()
{
   new RMidasFrame(gClient->GetRoot(), 200, 200, "pc2948");
}

int main(int argc, char **argv)
{
   TApplication theApp("RMidas", &argc, argv);

   if (gROOT->IsBatch()) {
      printf("%s: cannot run in batch mode\n", argv[0]);
      return 1;
   }

   new RMidasFrame(gClient->GetRoot(), 200, 200, argv[1]);

   theApp.Run();

   return 0;
}
