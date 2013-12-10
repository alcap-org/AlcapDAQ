#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <libgen.h>
// test
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
#include "TSocket.h"
#include "TMessage.h"
#include "TH1D.h"
#include "TH1.h"
#include "TObjArray.h"
#include "TObjString.h"

#include "getHist.h"
#include "TOnlineFrame.h"

std::vector<screen_info>screens;

int width = 800;
int height = 600;

/*
	 void cycleThread(void *v)
	 {
	 TOnlineFrame *onlineFrame = (TOnlineFrame *) v;

	 printf("Entering cycleThread\n");
	 while(1)
	 {
	 TThread::Sleep(20,0);
	 onlineFrame->ConsiderCycling();
	 }
	 }
	 */

void HelpMessage();

int main(int argc, char **argv)
{
	int c;
	std::string server_name = "localhost";
	int server_port = 9090;
	std::string root_file_name = "";
	std::string module_file_name = "";

	while ((c = getopt(argc, argv, "hH:p:i:M:")) != -1)
	{
		switch (c)
		{
			case 'h':
				HelpMessage();
				return 0;
			case 'H':
				server_name = optarg;
				break;
			case 'p':
				server_port = atoi(optarg);
				break;
			case 'i':
				root_file_name = optarg;
				break;
			case 'M':
				module_file_name = optarg;
				break;
			default:
				break;
		}
	}

  int fake_argc = 1;
  char fake_arg_str[256];
  sprintf(fake_arg_str,"./online-display");
  char *fake_argv[] = {fake_arg_str};
  
  TApplication theApp("RMidas", &fake_argc, fake_argv);
  
  if (gROOT->IsBatch()) {
    printf("%s: cannot run in batch mode\n", argv[0]);
    return 1;
  }

 std::cout<<module_file_name<<std::endl;
  TOnlineFrame *onlineFrame = new TOnlineFrame(gClient->GetRoot(),module_file_name);   
	
	if (root_file_name.length() != 0)
		onlineFrame->OpenRootFile(root_file_name.c_str());
	else
	{
		onlineFrame->setServerName(server_name.c_str());
		onlineFrame->setServerPort(server_port);
		onlineFrame->ConnectToServer();
		onlineFrame->Print();

		// Get list of all histograms, then copy them to the current directory
		std::vector<TString> vHistTitles = onlineFrame->GetHistTitles();
		for (unsigned int i = 0; i < vHistTitles.size(); ++i)
		{
			//printf("%d: %s\n", i, vHistTitles.at(i).Data());
			TH1 *hist = onlineFrame->GetHist(vHistTitles.at(i).Data());
			if(hist)
			{
				//hist->Draw();
				hist->Clone(vHistTitles.at(i).Data());
			}
		}
	}

	//onlineFrame->runMacro("modules/common/root_init.C");

  /*
    TThread *ct = new TThread(cycleThread, onlineFrame);
    ct->Run();
  */
  
  //theApp.SetIdleTimer(1,".q");

		onlineFrame->UpdateDisplay();
		theApp.Run(1);
#if 0
  time_t lastCycleTime = 0;//time(NULL);
  while(true) 
	{
		theApp.Run(1);
		if(time(NULL) > lastCycleTime + onlineFrame->getAutoUpdateTime()) 
		{
			//printf("Considering cycling\n");
			onlineFrame->ConsiderCycling(); 
			onlineFrame->ConsiderAutoupdate(); 
			lastCycleTime = time(NULL);
		}
		else 
		{
			usleep(10000);
		}
	}
#endif
  
  return 0;
}

void HelpMessage()
{
	printf("Default connection is to localhost:9090.\n");
	printf("To specify another host, use: \n");
	printf("\t ./online-display -H hostname -p port\n");
	printf("Or, to open a ROOT file:\n");
	printf("\t ./online-display -i filename\n");
}
