#include "TH1.h"
#include "TEllipse.h"

bool openHistFile(const char *filename);
bool openHistSocket(const char *hostname);
void closeHistSource();
TH1 *getHist(const char *histName);
