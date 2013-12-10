#ifndef TONLINEFRAME_H_RBZGMWAC
#define TONLINEFRAME_H_RBZGMWAC

#include "TApplication.h"
#include "TGButton.h"
#include "TRootEmbeddedCanvas.h"
#include "TGStatusBar.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TGTextEntry.h"
#include "TGLabel.h"
#include "TG3DLine.h"
#include "TGFileDialog.h"
#include "TMessage.h"
#include "TSocket.h"
#include "TObjArray.h"
#include <set>
#include <map>

class TH1;

class TOnlineFrame : public TGMainFrame 
{
private:
  TGHorizontalFrame *fTopFrame;
  //TGHorizontalFrame *fScreenFrame;
  TGButton *fUpdateButton;
  TGButton *fPrintButton;
  TGButton *fConnectButton;
  TGButton *fFileOpenButton;
  TRootEmbeddedCanvas *fEmbeddedCanvas;
  TGCheckButton *fCycleDisplays;
  TGCheckButton *fAutoUpdate;
  TGTextEntry   *fAutoUpdateTime;
  TGTextEntry   *fFileName;
  int fCurrentDisplay;
  TGStatusBar *fStatusBar;
  TGTextEntry *fServerName;
  TGTextEntry *fServerPort;
  ULong_t      run_nr;
	TSocket *fpSock;

public:
	TOnlineFrame(const TGWindow * p,std::string module_file_name);
	~TOnlineFrame();

	void CloseWindow();
	Bool_t ProcessMessage(Long_t msg, Long_t param1, Long_t param2);
	void ConsiderCycling();
	void ConsiderAutoupdate(const Bool_t force = kFALSE);
	void runMacro(const char *macro);
	void print_msg(const char *msg, const Int_t partidx = 0);
	const char *getServerName() const;
	void setServerName(const char *name);
	const unsigned int getServerPort() const;
	void setServerPort(const unsigned int port_nr);
	const unsigned int getAutoUpdateTime() const;
	TSocket *ConnectToServer();
	TFile *OpenRootFile(const char *filename, const Bool_t update_filename = kTRUE );
	TSocket *GetSocketHandle() {return fpSock;}
	TH1* GetHist(const char * histname);
	std::vector<TString> GetHistTitles();
	//TH1 *GetHisto(TString histname) {return GetHisto(histname.Data());}
	void UpdateDisplay();

	std::set<std::string> fLoadedMacros;
	std::map<std::string, TGTextButton*> fButtons;
};

struct screen_info 
{
  char visibleName[32];
  std::string macroName;
  std::string macroArgs;
  bool hasArgs;
};


extern std::vector<screen_info> screens;
extern int width;
extern int height;

#define B_UPDATE     1
#define B_AUTOUPDATE 2 
#define B_CYCLE      3
#define B_PRINT      4 
#define B_CONNECT    5 
#define B_FILE_OPEN  6 
#define SCREENS_BASE 10

#define TE_ROOTFILE  1

#endif /* end of include guard: TONLINEFRAME_H_RBZGMWAC */

