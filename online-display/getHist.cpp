/**
 * @file    getHist.cpp
 * @author  Original author uknown. Taken over by Vladimir Tishchenko <tishenko@pa.uky.edu>
 * @date    Sat Feb  4 16:57:34 2012
 * @date    Last-Updated: Mon Nov  4 22:07:22 2013 (-0500)
 *          By : Vladimir Tishchenko
 *          Update #: 148 
 * @version $Id$
 * 
 * @copyright (c) new (g-2) collaboration
 * 
 * @addtogroup inprogress InProgress 
 *  - \ref getHist.cpp
 * 
 * @page   getHist.cpp
 * 
 * @brief   
 * 
 * @details 
 * 
 * @todo Document this code 
 * 
 * @section Changelog
 * @verbatim
 * $Log$
 * @endverbatim
 */


/*
 * Obtains a histogram from the current source, whether that is a ROOT
 * file on disk or an online server.
 */

#include "TSocket.h"
#include "TMessage.h"
#include "TFile.h"
#include "TFolder.h"
#include "TH1I.h"

#include "getHist.h"

static TSocket *r_socket = NULL;  
static TFile   *r_file   = NULL;

#if 0
// top folder in root file
static TFolder *r_folder = NULL;
#endif

// Folder which contains all received objects. We will clean
// periodically to avoid memory leakage
static TFolder *folder = NULL;


// socket timeout, ms
static const int timeout = 5000; // 5s

// status. Non-zero is error occured.
static int gl_status = 0;

// function declaration
TObject *getObjectFromSocket(const char *name);
TObject *getObjectFromFile(const char *name);


/** 
 * Opens ROOT file on disk
 * 
 * @param filename 
 * 
 * @return pointer to opened file
 */

TFile *openRootFile(const char *filename) 
{
	closeSource();

	r_file = new TFile(filename);
	if ( r_file->IsZombie() )
	{
		printf("***ERROR! Cannot open ROOT file [%s]\n",filename);
		delete r_file;
		r_file = NULL;
#if 0
		r_folder = NULL;
#endif
		gl_status = RDISP_ERR_NO_FILE;
	}
#if 0
	else 
	{
		// *** get top folder ***
		r_file->GetObject("histos",r_folder);
		if ( !r_folder )
		{      
			printf("**ERROR! Cannot load folder 'histos' from file [%filename]\n",filename);
			delete r_file;
			r_file = NULL;
			r_folder = NULL;  
			gl_status = RDISP_ERR_NO_FOLDER;
		}
		else
		{
			r_folder->SetOwner( kTRUE );
		}
	}
#endif

	printf("Connected to source file :[%s]\n",filename);

	return r_file;
}

/** 
 * Connects to ROOT server (MIDAS analyzer)
 * 
 * @param hostname 
 * @param port_nr 
 * 
 * @return opened socket or zero 
 */

TSocket *openSocket(const char *hostname, const int port_nr)
{
	Int_t status;

	closeSource();

	r_socket = new TSocket(hostname, port_nr);

	if ( ! r_socket->IsValid()  ) 
	{
		delete r_socket;
		r_socket = NULL;
		gl_status = RDISP_ERR_NO_CON;
		return r_socket;
	}

	char str[64];
	sprintf(str,"ERROR");
	status = r_socket->Select(TSocket::kRead,timeout);
	if ( status == 1 ) 
	{
		r_socket->Recv(str, sizeof(str));
		if ( strncmp(str, "RMSERV", 6) != 0 ) 
		{
			// *** incorrect responce from the server ***
			printf("***ERROR! Unexpected response from the server [%s] on port %i: [%s]",hostname,port_nr,str);
			r_socket->Close();
			delete r_socket;
			r_socket = NULL;
			gl_status = RDISP_ERR_BAD_RESPONSE;
			return r_socket;
		}      
	}
	else
	{
		// *** no response from the server ***
		printf("***ERROR! No response from the server [%s] on port %i\n",hostname, port_nr);
		r_socket->Close();
		delete r_socket;
		r_socket = NULL;
		gl_status = RDISP_ERR_NO_RESPONSE;
		return r_socket;
	}

	printf("Successfully connected to server on host [%s] port %i\n",hostname,port_nr);
	gl_status = 0;

	return r_socket;
}

/** 
 * close opened sources (socket or file )
 * 
 */
void closeSource()
{
	if (r_socket != NULL) 
	{
		r_socket->Close(); 
		delete r_socket;
		r_socket = NULL;
	}

	if (r_file != NULL) 
	{
#if 0
		r_folder->Clear();
		delete r_folder;
		r_folder = NULL;
#endif
		r_file->Close("R");
		delete r_file;
		r_file = NULL;
	}

	clear_recv_buffer();
}

TObject *getObject(const char *name)
{
	if (r_socket != NULL) 
	{
		return getObjectFromSocket( name );
	} 
	else if (r_file != NULL) 
	{
		return getObjectFromFile( name );
	} 

	/// no sources available
	if ( gl_status == 0 )
	{
		printf("No connection\n");
		gl_status = RDISP_ERR_NO_SOURCE;
	}
	return NULL;
}

/*
	 void *getPointer(const char *name)
	 {

	 TMessage *m;
	 char str[256];

	 sprintf(str, "GetPointer %s", Name);
	 histSocket->Send(str);
	 histSocket->Recv(m);

	 unsigned long p;
	 m->ReadULong( p );

	 delete m;

	 return (void*)p;

	 }
	 */

TObject *getObjectFromSocket(const char *name)
{
	TMessage *m;
	char str[256];
	TObject *obj;

	if ( strlen(name) > 230 )
	{
		printf("***ERROR! Best object name\n");	
		return NULL;
	}

	if ( ! folder ) 
	{
		folder = new TFolder("heap","heap");
		folder->SetOwner( kTRUE );
	}

	sprintf(str, "GET %s", name);

	// make sure that the socket is alive
	Int_t status = r_socket->Select(TSocket::kWrite,timeout);
	if ( status != 1 )
	{
		printf("***ERROR! Broken connection to socket\n");
		closeSource();
		gl_status = RDISP_ERR_NO_CON;
		return NULL;
	}
	r_socket->Send(str);

	// receive reply from server
	//printf("Trying socket...\n");
	status = r_socket->Select(TSocket::kRead,timeout);
	if ( status != 1 )
	{
		printf("***ERROR! Broken connection to socket\n");
		closeSource();
		gl_status = RDISP_ERR_NO_CON;
		return NULL;
	}
	//printf("Alive!\n");
	Int_t n = r_socket->Recv(m);

	if ( n <= 0 )
	{
		printf("***ERROR! Broken pipe\n");
		closeSource();
		gl_status = RDISP_ERR_NO_CON;
		return NULL;
	}

	obj = (TObject*)m->ReadObject(m->GetClass());

	if (obj == 0) 
	{
		printf("Unable to get object [%s] from socket\n", name);
	}
	else
	{
		folder->Add( obj );
	}

	delete m;

	return obj;
}

TObject *getObjectFromFile(const char *name)
{

#if 0
	if ( ! r_file || ! r_folder )
#endif
		if ( ! r_file )
		{
			printf("***ERROR! File is not open\n");
			if ( gl_status == 0 )
			{
				gl_status = RDISP_ERR_NO_SOURCE;
			}
			return NULL;
		}


	/*
		 if ( ! folder ) 
		 {
		 folder = new TFolder("heap","heap");
		 folder->SetOwner( kTRUE );
		 }
		 */
	/*
		 TFolder *f;
		 r_file->GetObject("histos",f);
		 if ( !f )
		 {
		 printf("***ERROR! Cannot load folder histos from the ROOT file\n");
		 return NULL;
		 }
		 */

#if 0
	TObject *obj = r_folder->FindObject(name);
#else
	TObject *obj = r_file->Get(name);
#endif

	/*
		 if ( obj )
		 {
		 folder->Add( obj );
		 }
		 */

	return obj;
}

/** 
 * Frees memory occupied by received objects
 * 
 */
void clear_recv_buffer()
{
	if (folder) folder->Clear();
}

/** 
 * Returns error code. 
 * 
 * 
 * @return 0 if no error
 */

int get_status_code()
{
	switch ( gl_status )
	{
		case 0:
			// no error
			break;
		case RDISP_ERR_NO_CON:
			printf("No connection to remote server\n");
			break;
		case RDISP_ERR_BAD_RESPONSE:
			printf("Bad response from the server\n");
			break;
		case RDISP_ERR_NO_RESPONSE:
			printf("No response from the server\n");
			break;
		case RDISP_ERR_NO_SOURCE:
			printf("No sources opened\n");
			break;
		case RDISP_ERR_BROKEN_PIPE:
			printf("Broken pipe\n");
			break;
		case RDISP_ERR_NO_FILE:
			printf("Cannot open ROOT file\n");
			break;
		case RDISP_ERR_NO_FOLDER:
			printf("Cannot load root folder from the ROOT file\n");
			break;
		default:
			printf("Unknown error\n");
			break;
	}

	return gl_status;
}


/** 
 * Clears error flag. All operations will be blocked untill ther error is cleared.
 * 
 */
void reset_error_code()
{
	gl_status = 0;
}

/**
 *  Return current run number
 */
ULong_t get_run_number()
{

	ULong_t run_nr = 0;

	if (r_socket != NULL) 
	{
		TMessage *m;
		char str[256];

		sprintf(str, "GetRunNumber");
		Int_t status = r_socket->Select(TSocket::kWrite,timeout);
		if ( status != 1 )
		{
			printf("***ERROR! Broken connection to socket\n");
			closeSource();
			gl_status = RDISP_ERR_NO_CON;
			return 0;
		}
		r_socket->Send(str);

		status = r_socket->Select(TSocket::kRead,timeout);
		if ( status != 1 )
		{
			printf("***ERROR! Broken connection to socket\n");
			closeSource();
			gl_status = RDISP_ERR_NO_CON;
			return 0;
		}
		Int_t n = r_socket->Recv(m);

		if ( n <= 0 )
		{
			printf("***ERROR! Broken pipe\n");
			closeSource();
			gl_status = RDISP_ERR_NO_CON;
			return 0;
		}
		m->ReadULong( run_nr );

		delete m;

	}
	else if ( r_file != NULL) 
	{
		TH1I *h1 = (TH1I*)getObjectFromFile( "run_number/h1_run_number" );
		if ( h1 )
		{
			run_nr = h1->GetBinContent(1);
		}
	}

	return run_nr;


}
