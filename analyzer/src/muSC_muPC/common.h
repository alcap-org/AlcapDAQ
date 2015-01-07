#include "midas.h"
#include <math.h>

#include "TPCcommon.h"

// ###############################################################

// DAQ Constants
static const int kMaxEvents              = 31000;
static const int kMaxCrates              =  8;
static const int kMaxTDC400s             =  9;
static const int kMaxCaens               =  3;
static const int kMaxComps               =  3;
static const int kMaxCompFpgas           =  26;
static const int kMaxFadcs               =  4;
static const int kMaxFadcTriggers        =  5;   // max triggers per block
static const int kMaxCamacAdcs           =  10;
static const int kNumCaenChannels        =  128;
static const int kNumCompFpgaChannels    =  64;
static const int kNumFadcChannels        =  4;
static const int kMaxNumTPCPads          =  80;

// extern double kFadcPedestalOffset[(kMaxFadcs*kNumFadcChannels)+1];
// extern double kFadcPedestalSigma[(kMaxFadcs*kNumFadcChannels)+1];

// CAEN trailing edge and rollover check parameters
static const int kCaenTrailingEdgeLimit  = 100;
extern double rolloverPeriod;
extern double rolloverWidth;
static const int kNumRollovers           = 3;
static const int kNumRolloverPairs       = 3;
extern double rolloverTimeOffset[kNumRollovers];

// MuPC Constants
static const int kMuPC1NumXWires           =  24;
static const int kMuPC1NumYWires           =  24;

// MuSC and MuSCA Parameters
static const int kMuSCADInterval             =  29;    // ns
static const int kMuSCloADInterval           =  29;    // ns
static const int kMuSCAD2Interval            =  120;   // ns
static const int kMuSCAADInterval            =  90;    // ns
static const int kMuSCMismatchThreshold      =  3;
static const int kMuSCSignalsTCoincHalfWidth =  20;    // ns
extern double muSCCopy2TimeOffset;
extern double muSCATimeOffset;
extern double muSCGateBTimeOffset;
extern double muSCGateBBarTimeOffset;
extern double muSCRoutedTimeOffset[4];

// ePC Constants
static const int kePC1NumInnerStrips            =  192;
static const int kePC1NumAnodes                 =  512;
static const int kePC1NumOuterStrips            =  192;
static const int kePC2NumInnerStrips            =  320;
static const int kePC2NumAnodes                 =  1024;
static const int kePC2NumOuterStrips            =  320;

// Spark thresholds used in the "skimming" process
static const int kePC1AnodeSparkSizeLowCutoff = 35;
static const int kePC1AnodeSparkSizeHighCutoff = 490;
static const int kePC1IstripSparkSizeLowCutoff = 35;
static const int kePC1IstripSparkSizeHighCutoff = 180;
static const int kePC1OstripSparkSizeLowCutoff = 35;
static const int kePC1OstripSparkSizeHighCutoff = 180;
static const int kePC2AnodeSparkSizeLowCutoff = 35;
static const int kePC2AnodeSparkSizeHighCutoff = 990;
static const int kePC2IstripSparkSizeLowCutoff = 35;
static const int kePC2IstripSparkSizeHighCutoff = 300;
static const int kePC2OstripSparkSizeLowCutoff = 35;
static const int kePC2OstripSparkSizeHighCutoff = 300;
static const int keSCSparkSizeLowCutoff = 10;
static const int keSCSparkSizeHighCutoff = 55;
static const int kMuPC1XSparkSizeLowCutoff = 10;
static const int kMuPC1XSparkSizeHighCutoff = 24;
static const int kMuPC1YSparkSizeLowCutoff = 10;
static const int kMuPC1YSparkSizeHighCutoff = 24;

// eSC CAEN/COMP consistency cuts
static const int keSCCaenOnlyCutThreshold       =  10;
static const int keSCCompOnlyCutThreshold       =  25;

// Time intervals associated with skimming
static const double kSkimPP = 25000;
static const double kSkimInterval = 55000;
static const double kGeneralSkimInterval = 200000;

// Miscellaneous
static const double kPi                 =  M_PI;
static const double k2Pi                 =  (2.0 * M_PI);
static const bool ON                     =  1;
static const bool OFF                    =  0;

// ###############################################################

typedef struct caen_word {
  unsigned int time:20, edge:1, r2:2, start:1, channel:7, r1:1;
} caen_word;

typedef union comp_word {
  struct {
    unsigned int time:24, fpga:6, a:1, b:1;
  } r1;
  unsigned char bytes[4];
  unsigned int  word;
} comp_word;

typedef struct channel_hit {
#include "HITS.h"
} channel_hit;

typedef struct caen_hit {
#include "CAET.h"
} caen_hit;

typedef struct adc_hit {
#include "ADCS.h"
} adc_hit;

typedef struct fadc_word {
  unsigned char channel[4];
} fadc_word;

typedef struct FADC_info {
#include "FDCR.h"
} FADC_info;

typedef struct TPCWFD_info {
#include "TPCW.h"
} TPCWFD_info;

//typedef struct NEUTRON_info {
//#include "NDET.h"
//} NEUTRON_info;

typedef struct ndet_elt {
#include "NDET.h"
} ndet_elt;

typedef struct TPC_map_elt {
  int tdc400;
  int word;
  unsigned short bitmask;
  int bitshift;
} TPC_map_elt;

typedef struct tdc400_word {
  unsigned short time;
  union {
    unsigned short words[3];
    unsigned char bytes[6];
  } bits;
} tdc400_word;

typedef struct td_elt {
  double time;
  unsigned short word[3];
} td_elt;

typedef struct thresh_bits {
  unsigned short Aword[6]; // anode sections
  unsigned short Sword[3]; // strip sections
} thresh_bits;

typedef struct stack_elt {
  double time;
  bool  thrbits[3];
  thresh_bits thr[3];
} stack_elt;

typedef struct tpc_spark_word {
#include "TSPK.h"
} tpc_spark_word;

typedef struct skim_elt {
#include "SKIM.h"
} skim_elt;


// ###############################################################

bool parameter_exists(int par);
void print_TPCstack_interval(void *pevent, long index1, long index2);
void print_TPCstack_interval_by_time(void *pevent, double time1, double time2);
void print_TPCstack_section(void *pevent, long index);
void print_TPCstack_section_by_time(void *pevent, double time);
bool has_Ehigh_and_strip_hits(EVENT_HEADER *pheader, void *pevent, double time);
bool has_VH_hit(EVENT_HEADER *pheader, void *pevent, double time);
