#include "TH1.h"
#include "TEllipse.h"

bool openHistFile(const char *filename);
bool openHistSocket(const char *hostname, unsigned int port = 9090);
void closeHistSource();
TH1 *getHist(const char *histName);
TH1 *getHistFromSocket(const char *histName);
TH1 *getHistFromFile(const char *histName);
