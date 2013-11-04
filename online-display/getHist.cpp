/*
* Obtains a histogram from the current source, whether that is a ROOT
* file on disk or an online server.
*/

#include "TSocket.h"
#include "TMessage.h"
#include "TFile.h"
#include "TH1.h"

#include "getHist.h"

static TSocket *histSocket = NULL;
static TFile *histFile = NULL;

bool openHistFile(const char *filename)
{
  closeHistSource();

  histFile = new TFile(filename);
  bool isOpen = histFile->IsOpen();
  if(!isOpen) {
    delete histFile;
    histFile = NULL;
  }
  return isOpen;
}

bool openHistSocket(const char *hostname, unsigned int port)
{
  closeHistSource();

  histSocket = new TSocket(hostname, port);
  bool isValid = histSocket->IsValid();
  if(!isValid) {
    delete histSocket;
    histSocket = NULL;
		printf("Cannot establish connection to %s:%d\n", hostname, port);
		exit(1);
  }

  char str[20];
  histSocket->Recv(str, sizeof(str));
  if (strncmp(str, "RMSERV", 6)) {
    histSocket->Close();
    delete histSocket;
    histSocket = NULL;
    isValid = false;
  }

  return isValid;
}

void closeHistSource()
{
  if(histSocket != NULL) {
    histSocket->Close();
    delete histSocket;
    histSocket = NULL;
  }

  if(histFile != NULL) {
    histFile->Close();
    delete histFile;
    histFile = NULL;
  }
}

TH1 *getHist(const char *histName)
{
  if(histSocket != NULL) {
    return getHistFromSocket(histName);
  } else if(histFile != NULL) {
    return getHistFromFile(histName);
  } else {
    return NULL;
  }
}

TH1 *getHistFromSocket(const char *histName)
{
  // copied from rmidas.c

  TMessage *m;
  char str[256];
  TH1 *hist;

  sprintf(str, "GET %s", histName);
  histSocket->Send(str);
  histSocket->Recv(m);

  hist = (TH1*)m->ReadObject(m->GetClass());

  if(hist == 0) {
    printf("Unable to get %s\n", histName);
  }

  delete m;

  return hist;
}

TH1 *getHistFromFile(const char *histName)
{
  // Get() searches only the list of objects in memory, and the
  // highest directory in file.
  // FindObject() searches only the list of objects in memory
  // FindObjectAny() searchs the current directory and sub-directories
  return (TH1 *) histFile->FindObjectAny(histName);
}
