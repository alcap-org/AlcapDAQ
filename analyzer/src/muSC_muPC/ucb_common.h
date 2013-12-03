#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "midas.h"
#include "Mql.h"
#include "MqlArray.h"

// ###############################################################

// MuSC Constants
static const int kMuSCBookendWidth         =  40500; // ns
static const int k25usPP                   =  25000; // ns
static const int k35usPP                   =  35000; // ns
static const int k40usPP                   =  40000; // ns
// MuPC1 Constants
static const int kMuPC1XClusterMaxGap      =  2;
static const int kMuPC1XClusterInterval    =  260;  // ns
static const int kMuPC1XSparkSizeCutoff    =  21;
static const int kMuPC1YClusterMaxGap      =  2;
static const int kMuPC1YClusterInterval    =  260;  // ns
static const int kMuPC1YSparkSizeCutoff    =  21;
// TPC Constants
static const int kTPCFirstAmpAnode         =  5;
static const int kTPCLastAmpAnode          =  73;
static const int kTPCAnodeFlag             =  1;
static const int kTPCStripFlag             =  2;
static const double kTPCXDimension         =  140.0; // mm
static const double kTPCYDimension         =  120.0; // mm
static const double kTPCZDimension         =  300.0; // mm
static const double kTPCXOffset            =  0.0;   // mm
static const double kTPCYOffset            = -2.0;   // mm
static const double kTPCZOffset            =  8.5;   // mm
static const double kTPCMWPCOffset         =  375.0;   // ns
static const double kTPCMaxDriftTime       =  22420.0; // ns
static const double kTPCSVDriftInterval    =  (kTPCMaxDriftTime-kTPCMWPCOffset); // ns
// TPC Segment Search Definitions
static const int kSegMaxNumClusters        =  5;
static const int kSegMinClustSep           =  2;
static const int kSegMaxNumEndpts          =  5;
static const int kSegUpFlag                =  1;
static const int kSegDownFlag              = -1;
// ePC1 Constants
static const double kePC1ActiveLength              =  580; // mm
static const int kePC1InnerStripClusterMaxGap      =  2;
static const int kePC1InnerStripClusterMaxSize     =  11;
static const int kePC1InnerStripADInterval1        =  245; // ns
static const int kePC1InnerStripClusterTInterval1  =  240; // ns
static const int kePC1InnerStripADInterval2        =  1000; // ns
static const int kePC1InnerStripClusterTInterval2  =  300; // ns
static const int kePC1InnerStripSparkSizeCutoff    =  90;
static const double kePC1InnerStripDiameter        =  376; // mm
static const double kePC1InnerStripCircum          =  (M_PI * kePC1InnerStripDiameter); // mm
static const double kePC1InnerStripPhiOffset       = -0.048; // rad
static const double kePC1InnerStripScrew           =  46.194; // degrees
static const double kePC1InnerStripTanScrew        =  1.0426;
static const int kePC1AnodeClusterMaxGap           =  1;
static const int kePC1AnodeClusterMaxSize          =  10;
static const int kePC1AnodeADInterval1             =  215; // ns
static const int kePC1AnodeClusterTInterval1       =  210; // ns
static const int kePC1AnodeADInterval2             =  900; // ns
static const int kePC1AnodeClusterTInterval2       =  300; // ns
static const int kePC1AnodeSparkSizeCutoff         =  110;
static const double kePC1AnodeDiameter             =  384; // mm
static const double kePC1AnodeAbsPhiOffset         = -1.411; // rad
static const int kePC1OuterStripClusterMaxGap      =  2;
static const int kePC1OuterStripClusterMaxSize     =  11;
static const int kePC1OuterStripADInterval1        =  215; // ns
static const int kePC1OuterStripClusterTInterval1  =  210; // ns
static const int kePC1OuterStripADInterval2        =  1000; // ns
static const int kePC1OuterStripClusterTInterval2  =  300; // ns
static const int kePC1OuterStripSparkSizeCutoff    =  90;
static const double kePC1OuterStripDiameter        =  392; // mm
static const double kePC1OuterStripCircum          =  (M_PI * kePC1OuterStripDiameter); // mm
static const double kePC1OuterStripPhiOffset       = -0.171; // rad
static const double kePC1OuterStripScrew           =  43.811; // degrees
static const double kePC1OuterStripTanScrew        =  0.9593;
static const double kePC1HardcutLowZLimit          = -300; // mm
static const double kePC1HardcutHighZLimit         =  300; // mm
// ePC2 Constants
static const double kePC2ActiveLength              =  800; // mm
static const int kePC2InnerStripClusterMaxGap      =  1;
static const int kePC2InnerStripClusterMaxSize     =  10;
static const int kePC2InnerStripADInterval1        =  245; // ns
static const int kePC2InnerStripClusterTInterval1  =  240; // ns
static const int kePC2InnerStripADInterval2        =  1000; // ns
static const int kePC2InnerStripClusterTInterval2  =  300; // ns
static const int kePC2InnerStripSparkSizeCutoff    =  110;
static const double kePC2InnerStripDiameter        =  632; // mm
static const double kePC2InnerStripCircum          =  (M_PI * kePC2InnerStripDiameter); // mm
static const double kePC2InnerStripPhiOffset       =  0.026; // rad
static const double kePC2InnerStripScrew           =  45.738; // degrees
static const double kePC2InnerStripTanScrew        =  1.026;
static const int kePC2AnodeClusterMaxGap           =  1;
static const int kePC2AnodeClusterMaxSize          =  10;
static const int kePC2AnodeADInterval1             =  245; // ns
static const int kePC2AnodeClusterTInterval1       =  240; // ns
static const int kePC2AnodeADInterval2             =  1000; // ns
static const int kePC2AnodeClusterTInterval2       =  300; // ns
static const int kePC2AnodeSparkSizeCutoff         =  110;
static const double kePC2AnodeDiameter             =  640; // mm
static const double kePC2AnodeAbsPhiOffset         = -1.501; // rad
static const int kePC2OuterStripClusterMaxGap      =  1;
static const int kePC2OuterStripClusterMaxSize     =  10;
static const int kePC2OuterStripADInterval1        =  245; // ns
static const int kePC2OuterStripClusterTInterval1  =  240; // ns
static const int kePC2OuterStripADInterval2        =  900; // ns
static const int kePC2OuterStripClusterTInterval2  =  300; // ns
static const int kePC2OuterStripSparkSizeCutoff    =  110;
static const double kePC2OuterStripDiameter        =  648; // mm
static const double kePC2OuterStripCircum          =  (M_PI * kePC2OuterStripDiameter); // mm
static const double kePC2OuterStripPhiOffset       = -0.003; // rad
static const double kePC2OuterStripScrew           =  44.308; // degrees
static const double kePC2OuterStripTanScrew        =  0.976;
static const double kePC2HardcutLowZLimit          = -410; // mm
static const double kePC2HardcutHighZLimit         =  410; // mm
// eSC Constants
static const int keSCNumGondolas                   =  16;
static const int keSCClusterInterval               =  65;  // ns
static const int keSCSparkSizeCutoff               =  55;
static const double keSCDiameter                   =  772; // mm
static const double keSCActiveLength               =  900; // mm
static const double keSCAbsPhiOffset               =  (M_PI / 2.0); // rad
extern double keSCInnerTOffset[keSCNumGondolas+1];
extern double keSCInnerTSigma[keSCNumGondolas+1];
extern double keSCOuterTOffset[keSCNumGondolas+1];
extern double keSCOuterTSigma[keSCNumGondolas+1];
extern double keSCGondolaTOffset[keSCNumGondolas+1];
extern double keSCGondolaTSigma[keSCNumGondolas+1];
extern double keSCCaenCompTOffset;
extern double keSCCaenCompTSigma;
// Miscellaneous
static const int kSparkCutDeadtime    =  50000; // ns
static const double kEPS              =  0.001;
static const double kePC1eSCPhiSigma  =  0.20; // rad
static const double kePC2eSCPhiSigma  =  0.13; // rad
static const double PV_wall_radius    =  140; // mm


// ###############################################################

typedef struct cluster_info {
  int leftindex;
  int rightindex;
} cluster_info;

typedef struct endpoint_info {
  int leftindex;
  int rightindex;
  int row;
} endpoint_info;

//typedef struct segment_elt {
//#include "SEGS.h"
//} segment_elt;

//typedef struct track_elt {
//#include "TRKS.h"
//} track_elt;

//typedef struct mu_track_elt {
//#include "MUTK.h"
//} mu_track_elt;

//typedef struct impurity_capture_elt {
//#include "CAPT.h"
//} impurity_capture_elt;

//typedef struct ePC1_spark_info_elt {
//#include "E1CS.h"
//} ePC1_spark_info_elt;

//typedef struct ePC2_spark_info_elt {
//#include "E2CS.h"
//} ePC2_spark_info_elt;

typedef struct XYZ {
  double x, y, z;
} XYZ;

typedef struct RPZ {
  double r, phi, z;
} RPZ;

typedef struct RPT {
  double r, phi, theta;
} RPT;

typedef struct TPCextrap {
  double x, xprime, y, yprime, r;
} TPCextrap;

typedef struct muscatter_struct {
  int    pixelcount;
  double spread;
  double impact;
  double length;
  double maxfracdistance;
  double angle;
} muscatter_struct;


// ###############################################################

bool hit_search_TPCstack(stack_elt *stackArray,
			 int index,
			 int THRESHCODE,
			 int ROWTYPE,
			 int row1,
			 int row2,
			 int MINROW,
			 int MAXROW);
void segment_search_TPCstack(stack_elt *stackArray,
			     int num_stack_elts,
			     int index,
			     int *leftmost,
			     int *rightmost,
			     int *nendpts,
			     endpoint_info *endptA,
			     int start_row,
			     int THRESHCODE,
			     int ROWTYPE,
			     int DIR,
			     int ROWS_AHEAD,
			     int STRETCH);
void segment_search_TPCstack_custom(stack_elt *stackArray,
				    int num_stack_elts,
				    int index,
				    int *leftmost,
				    int *rightmost,
				    int *nendpts,
				    endpoint_info *endptA,
				    int start_row,
				    int THRESHCODE,
				    int ROWTYPE,
				    int DIR,
				    int ROWS_AHEAD,
				    int STRETCH,
				    int *ELcount,
				    int *EHcount);
XYZ fillXYZ(double x, double y, double z);
RPZ fillRPZ(double radius, double phi, double z);
RPT fillRPT(double radius, double phi, double theta);
XYZ convertRPZtoXYZ(RPZ RPZpoint);
XYZ convertRPTtoXYZ(RPT RPTpoint);
RPZ convertXYZtoRPZ(XYZ XYZpoint);
double dotproductXYZ(XYZ vector1,
		     XYZ vector2);
double magnitudeXYZ(XYZ vectorXYZ);
double ePC_anode_phi_fcn(int ePCNumber, double anode);
double ePC_inner_strip_z(int ePCNumber, double anode_phi, double inner_strip);
double ePC_outer_strip_z(int ePCNumber, double anode_phi, double outer_strip);
bool ePC1ePC2_PhiCoinc(double ePC1AbsPhi, 
		       double ePC2AbsPhi,
		       double max_phi_diff);
bool ePCeSC_PhiCoinc(int ePCNumber, double ePC_phi, int eSC_gondola);
XYZ ePC1ePC2_Tracking_ZaxisClosestApproach(XYZ ePC1, XYZ ePC2);
RPZ extrapolate_ePC1ePC2_to_eSC(XYZ ePC1, XYZ ePC2);
bool intersection_ePC1ePC2_circle(XYZ ePC1,
				  XYZ ePC2,
				  double x_coord,
				  double y_coord,
				  double radius);
bool ePC1ePC2eSC_PhiCoinc(RPZ ePC1ePC2_eSCInt, int gondola);
double calculate_eSC_cosmic_angle(int gondolaA, int gondolaB);
RPT calculate_ePC1ePC2_chord(XYZ p1, XYZ p2, XYZ p3, XYZ p4);
XYZ calculate_muDet_eDet_impact(double endstrip,
				double drifttime,
				double endanode,
				XYZ ePC1,
				XYZ ePC2);
XYZ calculate_TPC_subvolume_indices(double endstrip,
				    double drifttime,
				    double endanode,
				    int numXsubdivisions,
				    int numYsubdivisions,
				    int numZsubdivisions);
TPCextrap muSC_TPC_extrapolate(char *plane, double muSCTime,
                              double beginstrip, double endstrip,
                              double beginLtime, double endLtime,
                              double beginanode, double endanode);
void print_muscatter_info(muscatter_struct muscatter,
			  double driftLtime,
			  int mustop_anode);
muscatter_struct search_for_muscatter_track(EVENT_HEADER *pheader, 
					    void *pevent, 
					    double driftLtime,
					    double driftRtime,
					    int    mustop_Lindex,
					    int    mustop_Rindex,
					    int    mustop_anode,
					    int    mustop_mostLindex,
					    int    mustop_mostRindex);
muscatter_struct fill_muscatter_struct(int    pixelcount,
				       double spread,
				       double impact,
				       double length,
				       double maxfracdistance,
				       double angle);
