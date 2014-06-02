#include "BankIter.h"
#include "definitions.h"


template class BankIter<PulseIslandList>;
typedef BankIter<PulseIslandList> PulseIslandBankIter;

template class BankIter<AnalysedPulseList>;
typedef BankIter<AnalysedPulseList> AnalysedPulseBankIter;

template class BankIter<DetectorPulseList>;
typedef BankIter<DetectorPulseList> DetectorPulseBankIter;

// probably won't ever need this
//template class BankIter<MuonEventList>;

