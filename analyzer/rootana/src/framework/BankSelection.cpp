#include "BankSelection.tpl"
#include "definitions.h"

template class BankSelection<PulseIslandList>;
template class BankSelection<AnalysedPulseList>;
template class BankSelection<DetectorPulseList>;

// probably won't ever need this
//template class BankSelection<MuonEventList>;
