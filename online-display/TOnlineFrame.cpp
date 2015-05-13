#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <libgen.h>
#include <iostream>

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
#include "TObjString.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TH1I.h"

#include "getHist.h"
#include "TOnlineFrame.h"

using std::ifstream;
using std::cout;
using std::endl;

TOnlineFrame::TOnlineFrame(const TGWindow * p,std::string module_file_name):TGMainFrame(p, width,
		height)
{

	fCurrentDisplay = 0;
	run_nr = 0;

	// read macroses from the external file
	//const char *is_name = "/home/daq/DAQ/online-display/MODULES";
	const char *env_name = "DAQdir";
	std::string env_value = "";
	std::string is_name;
	if ( getenv(env_name) )
	{
                if (module_file_name.empty() ) module_file_name="MODULES";
		env_value = getenv(env_name);
		is_name = env_value + "/online-display/"+module_file_name;
	}
	else
	{
		std::cerr << "***ERROR! shell variable " << env_name << " is not set. The program will not be able to find display modules." << std::endl;      
	}

	//const char *is_name = "/home/daq/DAQ/online-display/MODULES";
	std::cout << "Reading modules from file [" << is_name << "]" << std::endl;
	ifstream *is = new ifstream( is_name.c_str() );
	char buf[1024];
	std::string args;
	while ( is->good() )
	{
		*is >> std::ws;
		is->getline(buf,1024);
		if ( buf[0] == '#' ) continue;
		if ( is->gcount() < 2 ) break;
		std::istringstream iss (buf,std::istringstream::in);
		screen_info info={"","","",false};
		iss >> std::ws >> info.visibleName >> std::ws >> info.macroName>> std::ws >>args;
		if (!args.empty()) {
			info.macroArgs=args;
			info.hasArgs =true;
		}
		screens.push_back(info);
		std::cout << buf << std::endl;
		args="";
	}
	delete is;

	// configure macro path
	std::string macro_path = gROOT->GetMacroPath();
	macro_path += std::string(":")+std::string(env_value)+std::string("/online-display/modules");
	gROOT->SetMacroPath( macro_path.c_str() );


	// a little frame at the top
	fTopFrame = new TGHorizontalFrame(this, width - 10, 30, kChildFrame);//, kFixedWidth);
	AddFrame(fTopFrame, new TGLayoutHints(kLHintsExpandX) );

	// an "update" button
	fUpdateButton = new TGTextButton(fTopFrame, "Update", B_UPDATE);
	fUpdateButton->Associate(this);
	fTopFrame->AddFrame(fUpdateButton, new TGLayoutHints(kLHintsLeft,2,2,2,2));

	// Autoupdate checkbox
	fAutoUpdate = new TGCheckButton(fTopFrame, "AutoUpdate", B_AUTOUPDATE);
	fAutoUpdate->Associate(this);
	fTopFrame->AddFrame(fAutoUpdate, new TGLayoutHints(kLHintsLeft|kLHintsCenterY));

	TGLabel *label = new TGLabel(fTopFrame,"every");
	fTopFrame->AddFrame(label, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,0,2,2,2));

	fAutoUpdateTime = new TGTextEntry(fTopFrame,"10");
	fAutoUpdateTime->SetWidth(20);
	fAutoUpdateTime->SetMaxLength(4);
	fTopFrame->AddFrame(fAutoUpdateTime, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,2,2,2,2));

	label = new TGLabel(fTopFrame,"s");
	fTopFrame->AddFrame(label, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,1,2,2,2));

	// a "cycle" checkbox
	fCycleDisplays = new TGCheckButton(fTopFrame, "Cycle", B_CYCLE);
	fCycleDisplays->Associate(this);
	fTopFrame->AddFrame(fCycleDisplays, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,2,1,2,2));


	// separator
	TGVertical3DLine *vline = new TGVertical3DLine(fTopFrame);
	fTopFrame->AddFrame(vline, new TGLayoutHints(kLHintsLeft|kLHintsExpandY,1,2,2,2));

	// server name
	label = new TGLabel(fTopFrame,"Server");
	fTopFrame->AddFrame(label, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,2,1,2,2));

	fServerName = new TGTextEntry(fTopFrame,"");
	fServerName->SetWidth(150);
	fServerName->SetMaxLength(64);
	fTopFrame->AddFrame(fServerName, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,2,2,2,2));

	// server port
	label = new TGLabel(fTopFrame,"port");
	fTopFrame->AddFrame(label, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,2,2,2,2));

	fServerPort = new TGTextEntry(fTopFrame,"");
	fServerPort->SetWidth(50);
	fServerPort->SetMaxLength(6);
	fTopFrame->AddFrame(fServerPort, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,2,2,2,2));

	// connect to server button
	fConnectButton = new TGTextButton(fTopFrame, "Connect", B_CONNECT);
	fConnectButton->Associate(this);
	fTopFrame->AddFrame(fConnectButton, new TGLayoutHints(kLHintsLeft,2,2,2,2));

	// separator
	vline = new TGVertical3DLine(fTopFrame);
	fTopFrame->AddFrame(vline, new TGLayoutHints(kLHintsLeft|kLHintsExpandY,1,2,2,2));

	// ROOT file
	label = new TGLabel(fTopFrame,"File");
	fTopFrame->AddFrame(label, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,2,2,2,2));

	fFileName = new TGTextEntry(fTopFrame,"");
	fFileName->SetWidth(100);
	fFileName->SetMaxLength(32);
	//fFileName->SetState(kFALSE);
	fFileName->Associate(this);
	fTopFrame->AddFrame(fFileName, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,2,2,2,2));


	// Open File button
	fFileOpenButton = new TGTextButton(fTopFrame, "Open", B_FILE_OPEN);
	fFileOpenButton->Associate(this);
	fTopFrame->AddFrame(fFileOpenButton, new TGLayoutHints(kLHintsLeft,2,2,2,2));

	// separator
	vline = new TGVertical3DLine(fTopFrame);
	fTopFrame->AddFrame(vline, new TGLayoutHints(kLHintsLeft|kLHintsExpandY,1,2,2,2));



	// a "print" button
	fPrintButton = new TGTextButton(fTopFrame, "Print to File", B_PRINT);
	fPrintButton->Associate(this);
	fTopFrame->AddFrame(fPrintButton, new TGLayoutHints(kLHintsRight,2,2,2,2));



	// containter for user buttons and canvas
	TGHorizontalFrame *frame_aux = new TGHorizontalFrame(this, 50, 80);
	AddFrame(frame_aux, new TGLayoutHints(kLHintsExpandY|kLHintsExpandX,1,1,1,1) );


	// left frame for user script buttons
	TGVerticalFrame *frame_user = new TGVerticalFrame(frame_aux, 10, 30);
	frame_aux->AddFrame(frame_user, new TGLayoutHints(kLHintsExpandY,1,1,1,1) );



	// a menu for changing screens
	//fScreenFrame = new TGHorizontalFrame(fTopFrame, 480, 28, kChildFrame);//, kFixedWidth); 
	//fTopFrame->AddFrame(fScreenFrame, new TGLayoutHints(kLHintsLeft));
	for (unsigned int i = 0; i < screens.size(); i++) 
	{
		TGTextButton *b = new TGTextButton(frame_user, screens[i].visibleName, 
				SCREENS_BASE + i);
		b->SetToolTipText((screens[i].macroName + screens[i].macroArgs).c_str());
		b->Associate(this);
		frame_user->AddFrame(b, new TGLayoutHints(kLHintsLeft|kFitWidth));
		fButtons[screens[i].visibleName]=b;
	}

	// our big embedded canvas
	fEmbeddedCanvas =
		new TRootEmbeddedCanvas("Canvas", frame_aux, width - 10, height - 32,kSunkenFrame|kDoubleBorder|kFitWidth|kFitHeight);
	fEmbeddedCanvas->SetAutoFit( kTRUE );
	frame_aux->AddFrame(fEmbeddedCanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 1, 1, 1, 1));

	fStatusBar = new TGStatusBar(this);
	AddFrame(fStatusBar,  new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
	int parts[] = {70,10,20};
	fStatusBar->SetParts(parts,3);

	SetWindowName("MIDAS Online Display");
	MapSubwindows();
	Resize(GetDefaultSize());
	MapWindow();
	SetMinWidth(300);

// Define the histogram 
	gROOT->LoadMacro("modules/common/get_histogram.C");
        gROOT->LoadMacro("modules/common/get_histogram_2d.C");

}

TOnlineFrame::~TOnlineFrame()
{
}

Bool_t TOnlineFrame::ProcessMessage(Long_t msg, Long_t param1,
		Long_t param2)
{
	Bool_t ret = false;
	char name[1024];
	char text[1024];

	switch (GET_MSG(msg)) 
	{
		case kC_COMMAND:      
			switch (GET_SUBMSG(msg)) 
			{	
				case kCM_BUTTON:
					if (param1 >= SCREENS_BASE) 
					{
						ULong_t ucolor_black;
						gClient->GetColorByName("#000000",ucolor_black);
						fButtons[screens[fCurrentDisplay].visibleName]->SetTextColor(ucolor_black);
						fCurrentDisplay = param1 - SCREENS_BASE;
					}
					if ( param1 == B_UPDATE || param1 >= SCREENS_BASE )
					{
						UpdateDisplay();
					}
					else if (param1 == B_PRINT) 
					{  
						unsigned long int t = time(NULL);
						sprintf(name,"%s_run_%06ld_%lu.pdf",screens[fCurrentDisplay].visibleName,run_nr,t);
						fEmbeddedCanvas->GetCanvas()->Print(name);
						sprintf(name,"%s_run_%06ld_%lu.png",screens[fCurrentDisplay].visibleName,run_nr,t);
						fEmbeddedCanvas->GetCanvas()->Print(name);
						sprintf(text,"Canvas printed to file [%s]",name);
						print_msg(text);
					}
					else if ( param1 == B_CONNECT )
					{
						if ( ConnectToServer() )
						{
							ConsiderAutoupdate( kTRUE );
						}
					}
					else if ( param1 == B_FILE_OPEN )
					{
						TGFileInfo fi;
						const char *file_types[] = {"hist files", "h*.root",
							0, 0};
						fi.fFileTypes = file_types;
						fi.fMultipleSelection = kFALSE;
						fi.SetMultipleSelection( kFALSE );
						//TGFileDialog *f = new TGFileDialog(gClient->GetRoot(),this,kFDOpen,&fi);
						if ( fi.fFilename )
						{
							if ( OpenRootFile(fi.fFilename) )
							{
								ConsiderAutoupdate( kTRUE );
							}
						}
					}
					ret = true;
					break;
				default:
					printf("Unknown command received\n");
					break;	  
			} // switch (GET_SUBMSG(msg))
			break; // case kC_COMMAND
		case kC_TEXTENTRY:
			switch (GET_SUBMSG(msg)) 
			{	
				case kTE_ENTER:
					if ( OpenRootFile( fFileName->GetText(), kFALSE ) )
					{
						ConsiderAutoupdate( kTRUE );
					}
					break;	  
			}
			break; // case kC_TEXTENTRY
	} // switch (GET_MSG(msg)) 



	return ret;
}

void TOnlineFrame::UpdateDisplay(){
	screen_info &current_display=screens[fCurrentDisplay];
	//runMacro(macro);
	if(fLoadedMacros.count(current_display.macroName) ==0) {
		gROOT->LoadMacro(current_display.macroName.c_str());
		cout<<"Loaded macro: "<<current_display.macroName<<endl;
		fLoadedMacros.insert(current_display.macroName);
	}
	int last_slash=current_display.macroName.rfind("/")+1;
	std::string command=current_display.macroName.substr(last_slash);
	int extension_start=command.find(".");
	command=command.substr(0,extension_start);
	if(!current_display.hasArgs) command+="()";
	else command+=current_display.macroArgs;
	cout<<"Processing Line: "<<command<<endl;
	gROOT->ProcessLine(command.c_str());

	cout<< "about to set color"<<endl;
		ULong_t ucolor_red;
		gClient->GetColorByName("#FF0000",ucolor_red);
	fButtons[screens[fCurrentDisplay].visibleName]->SetTextColor(ucolor_red);
	gPad->Update();
}

void TOnlineFrame::CloseWindow()
{
	DeleteWindow();
	gApplication->Terminate(0);
	exit(1);
}

void TOnlineFrame::ConsiderCycling()
{
	if(fCycleDisplays->GetState() == kButtonDown) 
	{
		fCurrentDisplay = (fCurrentDisplay+1) % screens.size();
		const char *macro = screens[fCurrentDisplay].macroName.c_str();
		runMacro(macro);
	}
}

/** 
 * 
 * 
 * @param force Force update if true
 */

void TOnlineFrame::ConsiderAutoupdate(const Bool_t force)
{
	if (((fAutoUpdate->GetState() == kButtonDown) && fCycleDisplays->GetState()) != kButtonDown || force ) 
	{
		const char *macro = screens[fCurrentDisplay].macroName.c_str();
		runMacro(macro);
	}
}

const char *TOnlineFrame::getServerName() const
{
	return fServerName->GetText();
}

void TOnlineFrame::setServerName(const char *name)
{
	return fServerName->SetText( name );
}

const unsigned int TOnlineFrame::getServerPort() const
{  
	return atol(fServerPort->GetText());
}

void TOnlineFrame::setServerPort(const unsigned int port_nr)
{
	char txt[32];
	sprintf(txt,"%d",port_nr);
	return fServerPort->SetText( txt );
}

const unsigned int TOnlineFrame::getAutoUpdateTime() const
{  
	unsigned int t = atol(fAutoUpdateTime->GetText());
	if ( t < 1 ) t = 1;
	return t;
}

TSocket *TOnlineFrame::ConnectToServer()
{
	TSocket *s = NULL;
	const char *hostname = getServerName();
	const unsigned int port_nr = getServerPort();
	char msg[1024];

	gROOT->DeleteAll();
	fEmbeddedCanvas->GetCanvas()->Clear();
	fEmbeddedCanvas->Clear();

	ULong_t ucolor_white;
	gClient->GetColorByName("#ffffff",ucolor_white);


	if ( (s = openSocket(hostname,port_nr)) == 0 )
	{
		sprintf(msg,"Cannot connect to port %d on host [%s]",port_nr,hostname);
		print_msg(msg);
		print_msg("ERROR",2);
		exit(1);
		//fServerName->ChangeBackground(ucolor_white);      
	}
	else
	{
		sprintf(msg,"Connected to remote server %s:%d",hostname,port_nr);
		print_msg(msg);
		sprintf(msg,"%s:%d",hostname,port_nr);
		print_msg(msg,2);
		//ConsiderAutoupdate( kTRUE );
		ULong_t ucolor_green;
		gClient->GetColorByName("#A0F5A0",ucolor_green);
		fServerName->ChangeBackground(ucolor_green);
	}
	//fFileName->SetText("");
	fFileName->ChangeBackground(ucolor_white);
	fpSock = s;
	return s;
}

TFile *TOnlineFrame::OpenRootFile(const char *filename, const Bool_t update_filename)
{
	TFile *f = NULL;
	char msg[1024];

	gROOT->DeleteAll();
	fEmbeddedCanvas->GetCanvas()->Clear();
	fEmbeddedCanvas->Clear();

	ULong_t ucolor_white;
	gClient->GetColorByName("#ffffff",ucolor_white);


	if ( (f = openRootFile(filename)) == 0 )
	{
		sprintf(msg,"Cannot open ROOT file [%s]",filename);
		print_msg(msg);
		print_msg("ERROR",1);
		print_msg("ERROR",2);
		exit(1);
		//fFileName->SetText("");
		//fFileName->ChangeBackground(ucolor_white);
	}
	else
	{
		sprintf(msg,"Connected to ROOT file %s",filename);
		print_msg(msg);
		print_msg(basename((char*)filename),1);
		print_msg("File",2);
		//fFileName->SetText(basename(filename));
		//ConsiderAutoupdate( kTRUE );
		ULong_t ucolor_green;
		gClient->GetColorByName("#A0FfA0",ucolor_green);
		fFileName->ChangeBackground(ucolor_green);
	}

	if ( update_filename )
		fFileName->SetText(basename((char*)filename));

	fServerName->ChangeBackground(ucolor_white);

	return f;
}


void TOnlineFrame::runMacro(const char *macro)
{

	char txt[1024];

	run_nr = get_run_number();
	sprintf(txt,"%ld",run_nr);
	print_msg(txt,1);
	sprintf(txt,"Executing  macro [%s]",macro);
	print_msg(txt);

	gROOT->DeleteAll();
	clear_recv_buffer();
	fEmbeddedCanvas->GetCanvas()->Clear();
	fEmbeddedCanvas->Clear();
	gROOT->cd();
	gROOT->Macro(macro);  
	// check status
	int status = get_status_code();
	if ( status != 0 )
	{
		const char *msg = "";
		switch ( status )
		{
			case RDISP_ERR_NO_CON:
				msg = "No connection to remote server";	   
				break;
			case RDISP_ERR_BAD_RESPONSE:
				msg = "Bad response from the server";
				break;
			case RDISP_ERR_NO_RESPONSE:	  
				msg = "No response from the server";
				break;
			case RDISP_ERR_NO_SOURCE:
				msg = "No sources opened";
				break;
			case RDISP_ERR_BROKEN_PIPE:
				msg = "Broken pipe";
				break;
			case RDISP_ERR_NO_FILE:
				msg = "Cannot open ROOT file";
				break;
			case RDISP_ERR_NO_FOLDER:
				msg = "Cannot load root folder from the ROOT file";
				break;
			default:
				msg = "Unknown error";
				break;
		}
		print_msg( msg );
	}
}

void TOnlineFrame::print_msg(const char *msg, const Int_t partidx)
{
	printf("%s\n",msg);
	fStatusBar->SetText(msg, partidx);
}

//TObjArray *TOnlineFrame::GetHistTitles()
//{
	//if(!fpSock)
		//return NULL;

	//TMessage *msg;
	//fpSock->Send("LIST");
	//fpSock->Recv(msg);
	//TObjArray *objArray = (TObjArray*)msg->ReadObject(msg->GetClass());
	//delete msg;
	//return objArray;
//}



TH1* TOnlineFrame::GetHist(const char * histname)
{
	if (!fpSock)
		return NULL;

	char req[2048];
	sprintf(req, "GET %s", histname);
	TMessage *msg;

	fpSock->Send(req);
	fpSock->Recv(msg);
	TH1 *hist = (TH1*) msg->ReadObject(msg->GetClass());
	//hist->Print();
	//hist->Draw();

	delete msg;
	return hist;
}

std::vector<TString> TOnlineFrame::GetHistTitles()
{
	std::vector<TString> v;
	if(!fpSock)
		return v;

	fpSock->Send("LIST");
	TMessage *msg;
	fpSock->Recv(msg);
	TObjArray *objArray = (TObjArray *)msg->ReadObject(msg->GetClass());
	for (int i = 0; i < objArray->GetEntries(); ++i)
	{
		TObjString *title = (TObjString *)objArray->At(i);
		v.push_back((TString)title->GetString());
	}
	delete msg;
	return v;
}
