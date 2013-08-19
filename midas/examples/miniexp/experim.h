/********************************************************************\

  Name:         experim.h
  Created by:   ODBedit program

  Contents:     This file contains C structures for the "Runinfo"
                tree in the ODB and the "/Analyzer/Parameters" tree.

                Additionally, it contains the "Settings" subtree for
                all items listed under "/Equipment" as well as their
                event definition.

                It can be used by the frontend and analyzer to work
                with these information.

                All C structures are accompanied with a string represen-
                tation which can be used in the db_create_record function
                to setup an ODB structure which matches the C structure.

  Created on:   Tue Sep 09 15:06:28 1997

\********************************************************************/

typedef struct {
   char comment[80];
} EXP_PARAM;

#define EXP_PARAM_STR(_name) char *_name[] = {\
"[.]",\
"Comment = STRING : [80] Test",\
"",\
NULL }

typedef struct {
   char comment[80];
} EXP_EDIT;

#define EXP_EDIT_STR(_name) char *_name[] = {\
"[.]",\
"Comment = LINK : [35] /Experiment/Run Parameters/Comment",\
"",\
NULL }

#ifndef EXCL_ADC_CALIBRATION

typedef struct {
   INT pedestal[8];
   float software_gain[8];
   double histo_threshold;
} ADC_CALIBRATION_PARAM;

#define ADC_CALIBRATION_PARAM_STR(_name) char *_name[] = {\
"[.]",\
"Pedestal = INT[8] :",\
"[0] 174",\
"[1] 194",\
"[2] 176",\
"[3] 182",\
"[4] 185",\
"[5] 215",\
"[6] 202",\
"[7] 202",\
"Software Gain = FLOAT[8] :",\
"[0] 1",\
"[1] 1",\
"[2] 1",\
"[3] 1",\
"[4] 1",\
"[5] 1",\
"[6] 1",\
"[7] 1",\
"Histo threshold = DOUBLE : 20",\
"",\
NULL }

#endif

#ifndef EXCL_ADC_SUMMING

typedef struct {
   float adc_threshold;
} ADC_SUMMING_PARAM;

#define ADC_SUMMING_PARAM_STR(_name) char *_name[] = {\
"[.]",\
"ADC threshold = FLOAT : 5",\
"",\
NULL }

#endif

#ifndef EXCL_GLOBAL

typedef struct {
   float adc_threshold;
} GLOBAL_PARAM;

#define GLOBAL_PARAM_STR(_name) char *_name[] = {\
"[.]",\
"ADC Threshold = FLOAT : 5",\
"",\
NULL }

#endif

#ifndef EXCL_TRIGGER

typedef struct {
   float sum;
} ASUM_BANK;

#define ASUM_BANK_STR(_name) char *_name[] = {\
"[.]",\
"Sum = FLOAT : 0",\
"",\
NULL }

typedef struct {
   BYTE io506;
} TRIGGER_SETTINGS;

#define TRIGGER_SETTINGS_STR(_name) char *_name[] = {\
"[.]",\
"IO506 = BYTE : 7",\
"",\
NULL }

#endif

#ifndef EXCL_SCALER

#endif
