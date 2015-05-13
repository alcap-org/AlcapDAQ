/********************************************************************\

  Name:         experim.h
  Created by:   ODBedit program

  Contents:     This file contains C structures for the "Experiment"
                tree in the ODB and the "/Analyzer/Parameters" tree.

                Additionally, it contains the "Settings" subtree for
                all items listed under "/Equipment" as well as their
                event definition.

                It can be used by the frontend and analyzer to work
                with these information.

                All C structures are accompanied with a string represen-
                tation which can be used in the db_create_record function
                to setup an ODB structure which matches the C structure.

  Created on:   Thu Apr 30 15:36:18 2015

\********************************************************************/

#ifndef EXCL_CRATE_9

#define CRATE_9_SETTINGS_DEFINED

typedef struct {
  BOOL      master;
  INT       diagnostic_print_level;
  INT       operator_message_level;
  BOOL      enabled;
  BOOL      synchronous;
  struct {
    BOOL      enabled;
    INT       interface;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_0;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_1;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_2;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_3;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_4;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_5;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_6;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_7;
  } nfadc_80;
  struct {
    BOOL      enabled;
    INT       interface;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_0;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_1;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_2;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_3;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_4;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_5;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_6;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_7;
  } nfadc_81;
  struct {
    BOOL      enabled;
    INT       interface;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_0;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_1;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_2;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_3;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_4;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_5;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_6;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_7;
  } nfadc_82;
  struct {
    BOOL      enabled;
    INT       interface;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_0;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_1;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_2;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_3;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_4;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_5;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_6;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_7;
  } nfadc_83;
  struct {
    BOOL      enabled;
    INT       interface;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_0;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_1;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_2;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_3;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_4;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_5;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_6;
    struct {
      INT       led_mode;
      INT       lower_threshold;
      INT       upper_threshold;
      INT       pulser_period;
      INT       trigger_mask;
      INT       presamples;
      INT       stretch_samples;
      INT       dcm_phase;
    } channel_7;
  } nfadc_84;
} CRATE_9_SETTINGS;

#define CRATE_9_SETTINGS_STR(_name) char *_name[] = {\
"[.]",\
"Master = BOOL : n",\
"Diagnostic Print Level = INT : 0",\
"Operator Message Level = INT : -1",\
"Enabled = BOOL : y",\
"Synchronous = BOOL : y",\
"",\
"[NFADC 80]",\
"Enabled = BOOL : n",\
"Interface = INT : 7",\
"",\
"[NFADC 80/Channel 0]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 4",\
"Presamples = INT : 7",\
"Stretch samples = INT : 60",\
"DCM phase = INT : 10",\
"",\
"[NFADC 80/Channel 1]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 4",\
"Presamples = INT : 7",\
"Stretch samples = INT : 60",\
"DCM phase = INT : 10",\
"",\
"[NFADC 80/Channel 2]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 4",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 10",\
"",\
"[NFADC 80/Channel 3]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 4",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 10",\
"",\
"[NFADC 80/Channel 4]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 4",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 10",\
"",\
"[NFADC 80/Channel 5]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 4",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 10",\
"",\
"[NFADC 80/Channel 6]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 4",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 10",\
"",\
"[NFADC 80/Channel 7]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 4",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 10",\
"",\
"[NFADC 81]",\
"Enabled = BOOL : n",\
"Interface = INT : 2",\
"",\
"[NFADC 81/Channel 0]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2500",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000",\
"Trigger mask = INT : 4",\
"Presamples = INT : 7",\
"Stretch samples = INT : 30",\
"DCM phase = INT : 10",\
"",\
"[NFADC 81/Channel 1]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 50",\
"Trigger mask = INT : 4",\
"Presamples = INT : 7",\
"Stretch samples = INT : 60",\
"DCM phase = INT : 10",\
"",\
"[NFADC 81/Channel 2]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 4",\
"Presamples = INT : 7",\
"Stretch samples = INT : 0",\
"DCM phase = INT : 10",\
"",\
"[NFADC 81/Channel 3]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 4",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 0",\
"",\
"[NFADC 81/Channel 4]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 4",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 10",\
"",\
"[NFADC 81/Channel 5]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 4",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 10",\
"",\
"[NFADC 81/Channel 6]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 0",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 10",\
"",\
"[NFADC 81/Channel 7]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 4",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 10",\
"",\
"[NFADC 82]",\
"Enabled = BOOL : y",\
"Interface = INT : 6",\
"",\
"[NFADC 82/Channel 0]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2000",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000",\
"Trigger mask = INT : 1",\
"Presamples = INT : 5",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 10",\
"",\
"[NFADC 82/Channel 1]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2000",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000",\
"Trigger mask = INT : 1",\
"Presamples = INT : 5",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 10",\
"",\
"[NFADC 82/Channel 2]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 1200",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 1",\
"Presamples = INT : 5",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 10",\
"",\
"[NFADC 82/Channel 3]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 1500",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 1",\
"Presamples = INT : 5",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 10",\
"",\
"[NFADC 82/Channel 4]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2400",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 1",\
"Presamples = INT : 5",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 10",\
"",\
"[NFADC 82/Channel 5]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2400",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 1",\
"Presamples = INT : 5",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 10",\
"",\
"[NFADC 82/Channel 6]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2400",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 1",\
"Presamples = INT : 5",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 10",\
"",\
"[NFADC 82/Channel 7]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2400",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 1",\
"Presamples = INT : 5",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 10",\
"",\
"[NFADC 83]",\
"Enabled = BOOL : n",\
"Interface = INT : 1",\
"",\
"[NFADC 83/Channel 0]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 1",\
"Presamples = INT : 5",\
"Stretch samples = INT : 30",\
"DCM phase = INT : 20",\
"",\
"[NFADC 83/Channel 1]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 0",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 5",\
"",\
"[NFADC 83/Channel 2]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 0",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 5",\
"",\
"[NFADC 83/Channel 3]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 0",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 5",\
"",\
"[NFADC 83/Channel 4]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 0",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 5",\
"",\
"[NFADC 83/Channel 5]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 0",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 5",\
"",\
"[NFADC 83/Channel 6]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 0",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 5",\
"",\
"[NFADC 83/Channel 7]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 0",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 5",\
"",\
"[NFADC 84]",\
"Enabled = BOOL : n",\
"Interface = INT : 0",\
"",\
"[NFADC 84/Channel 0]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 1",\
"Presamples = INT : 5",\
"Stretch samples = INT : 30",\
"DCM phase = INT : 20",\
"",\
"[NFADC 84/Channel 1]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 0",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 5",\
"",\
"[NFADC 84/Channel 2]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 0",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 5",\
"",\
"[NFADC 84/Channel 3]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 0",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 5",\
"",\
"[NFADC 84/Channel 4]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 0",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 5",\
"",\
"[NFADC 84/Channel 5]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 0",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 5",\
"",\
"[NFADC 84/Channel 6]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 0",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 5",\
"",\
"[NFADC 84/Channel 7]",\
"LED Mode = INT : 4",\
"Lower threshold = INT : 2300",\
"Upper threshold = INT : 4095",\
"Pulser period = INT : 5000000",\
"Trigger mask = INT : 0",\
"Presamples = INT : 7",\
"Stretch samples = INT : 20",\
"DCM phase = INT : 5",\
"",\
NULL }

#define CRATE_9_COMMON_DEFINED

typedef struct {
  WORD      event_id;
  WORD      trigger_mask;
  char      buffer[32];
  INT       type;
  INT       source;
  char      format[8];
  BOOL      enabled;
  INT       read_on;
  INT       period;
  double    event_limit;
  DWORD     num_subevents;
  INT       log_history;
  char      frontend_host[32];
  char      frontend_name[32];
  char      frontend_file_name[256];
} CRATE_9_COMMON;

#define CRATE_9_COMMON_STR(_name) char *_name[] = {\
"[.]",\
"Event ID = WORD : 1000",\
"Trigger mask = WORD : 65535",\
"Buffer = STRING : [32] BUF9",\
"Type = INT : 66",\
"Source = INT : 16777215",\
"Format = STRING : [8] MIDAS",\
"Enabled = BOOL : y",\
"Read on = INT : 1",\
"Period = INT : 500",\
"Event limit = DOUBLE : 0",\
"Num subevents = DWORD : 0",\
"Log history = INT : 0",\
"Frontend host = STRING : [32] fe1",\
"Frontend name = STRING : [32] Crate 9",\
"Frontend file name = STRING : [256] crate.cpp",\
"",\
NULL }

#endif

#ifndef EXCL_EB

#define EB_COMMON_DEFINED

typedef struct {
  WORD      event_id;
  WORD      trigger_mask;
  char      buffer[32];
  INT       type;
  INT       source;
  char      format[8];
  BOOL      enabled;
  INT       read_on;
  INT       period;
  double    event_limit;
  DWORD     num_subevents;
  INT       log_history;
  char      frontend_host[32];
  char      frontend_name[32];
  char      frontend_file_name[256];
} EB_COMMON;

#define EB_COMMON_STR(_name) char *_name[] = {\
"[.]",\
"Event ID = WORD : 1",\
"Trigger mask = WORD : 0",\
"Buffer = STRING : [32] SYSTEM",\
"Type = INT : 0",\
"Source = INT : 0",\
"Format = STRING : [8] MIDAS",\
"Enabled = BOOL : y",\
"Read on = INT : 0",\
"Period = INT : 0",\
"Event limit = DOUBLE : 0",\
"Num subevents = DWORD : 0",\
"Log history = INT : 0",\
"Frontend host = STRING : [32] abner",\
"Frontend name = STRING : [32] Ebuilder",\
"Frontend file name = STRING : [256] ebuser.cpp",\
"",\
NULL }

#define EB_SETTINGS_DEFINED

typedef struct {
  INT       number_of_fragment;
  BOOL      user_build;
  char      user_field[64];
  BOOL      fragment_required[5];
} EB_SETTINGS;

#define EB_SETTINGS_STR(_name) char *_name[] = {\
"[.]",\
"Number of Fragment = INT : 5",\
"User build = BOOL : y",\
"User Field = STRING : [64] 100",\
"Fragment Required = BOOL[5] :",\
"[0] y",\
"[1] y",\
"[2] y",\
"[3] y",\
"[4] y",\
"",\
NULL }

#endif

#ifndef EXCL_CRATE_4

#define CRATE_4_SETTINGS_DEFINED

typedef struct {
  BOOL      master;
  INT       diagnostic_print_level;
  INT       operator_message_level;
  BOOL      enabled;
  BOOL      synchronous;
  struct {
    DWORD     vme_base;
    BOOL      enabled;
    DWORD     waveform_length;
    BYTE      acquisition_mode;
    BYTE      software_trigger_mode;
    BYTE      hardware_trigger_mode;
    BYTE      trigger_edge__0_ris_1_fal_;
    INT       down_sampling_factor;
    char      roc_firmware[128];
    char      amc_firmware[128];
    struct {
      BOOL      enabled;
      DWORD     dc_offset;
      BYTE      self_trigger_mode;
      DWORD     trigger_threshhold;
    } ch0;
    struct {
      BOOL      enabled;
      DWORD     dc_offset;
      BYTE      self_trigger_mode;
      DWORD     trigger_threshhold;
    } ch1;
    struct {
      BOOL      enabled;
      DWORD     dc_offset;
      BYTE      self_trigger_mode;
      DWORD     trigger_threshhold;
    } ch2;
    struct {
      BOOL      enabled;
      DWORD     dc_offset;
      BYTE      self_trigger_mode;
      DWORD     trigger_threshhold;
    } ch3;
    struct {
      BOOL      enabled;
      DWORD     dc_offset;
      BYTE      self_trigger_mode;
      DWORD     trigger_threshhold;
    } ch4;
    struct {
      BOOL      enabled;
      DWORD     dc_offset;
      BYTE      self_trigger_mode;
      DWORD     trigger_threshhold;
    } ch5;
    struct {
      BOOL      enabled;
      DWORD     dc_offset;
      BYTE      self_trigger_mode;
      DWORD     trigger_threshhold;
    } ch6;
    struct {
      BOOL      enabled;
      DWORD     dc_offset;
      BYTE      self_trigger_mode;
      DWORD     trigger_threshhold;
    } ch7;
  } caen0;
} CRATE_4_SETTINGS;

#define CRATE_4_SETTINGS_STR(_name) char *_name[] = {\
"[.]",\
"Master = BOOL : n",\
"Diagnostic Print Level = INT : -1076627060",\
"Operator Message Level = INT : -1076627060",\
"Enabled = BOOL : y",\
"Synchronous = BOOL : y",\
"",\
"[CAEN0]",\
"VME base = DWORD : 839909376",\
"enabled = BOOL : y",\
"waveform length = DWORD : 64",\
"acquisition mode = BYTE : 1",\
"software trigger mode = BYTE : 0",\
"hardware trigger mode = BYTE : 0",\
"trigger edge (0-ris 1-fal) = BYTE : 1",\
"down sampling factor = INT : 1",\
"ROC firmware = STRING : [128] -",\
"AMC firmware = STRING : [128] -",\
"",\
"[CAEN0/Ch0]",\
"enabled = BOOL : y",\
"DC offset = DWORD : 32768",\
"self-trigger mode = BYTE : 1",\
"trigger threshhold = DWORD : 25856",\
"",\
"[CAEN0/Ch1]",\
"enabled = BOOL : y",\
"DC offset = DWORD : 32768",\
"self-trigger mode = BYTE : 1",\
"trigger threshhold = DWORD : 25856",\
"",\
"[CAEN0/Ch2]",\
"enabled = BOOL : y",\
"DC offset = DWORD : 0",\
"self-trigger mode = BYTE : 1",\
"trigger threshhold = DWORD : 0",\
"",\
"[CAEN0/Ch3]",\
"enabled = BOOL : y",\
"DC offset = DWORD : 0",\
"self-trigger mode = BYTE : 1",\
"trigger threshhold = DWORD : 0",\
"",\
"[CAEN0/Ch4]",\
"enabled = BOOL : y",\
"DC offset = DWORD : 0",\
"self-trigger mode = BYTE : 1",\
"trigger threshhold = DWORD : 0",\
"",\
"[CAEN0/Ch5]",\
"enabled = BOOL : y",\
"DC offset = DWORD : 0",\
"self-trigger mode = BYTE : 1",\
"trigger threshhold = DWORD : 0",\
"",\
"[CAEN0/Ch6]",\
"enabled = BOOL : y",\
"DC offset = DWORD : 0",\
"self-trigger mode = BYTE : 1",\
"trigger threshhold = DWORD : 0",\
"",\
"[CAEN0/Ch7]",\
"enabled = BOOL : y",\
"DC offset = DWORD : 0",\
"self-trigger mode = BYTE : 1",\
"trigger threshhold = DWORD : 0",\
"",\
NULL }

#define CRATE_4_COMMON_DEFINED

typedef struct {
  WORD      event_id;
  WORD      trigger_mask;
  char      buffer[32];
  INT       type;
  INT       source;
  char      format[8];
  BOOL      enabled;
  INT       read_on;
  INT       period;
  double    event_limit;
  DWORD     num_subevents;
  INT       log_history;
  char      frontend_host[32];
  char      frontend_name[32];
  char      frontend_file_name[256];
} CRATE_4_COMMON;

#define CRATE_4_COMMON_STR(_name) char *_name[] = {\
"[.]",\
"Event ID = WORD : 1000",\
"Trigger mask = WORD : 65535",\
"Buffer = STRING : [32] BUF4",\
"Type = INT : 66",\
"Source = INT : 16777215",\
"Format = STRING : [8] MIDAS",\
"Enabled = BOOL : y",\
"Read on = INT : 1",\
"Period = INT : 500",\
"Event limit = DOUBLE : 0",\
"Num subevents = DWORD : 0",\
"Log history = INT : 0",\
"Frontend host = STRING : [32] abner",\
"Frontend name = STRING : [32] Crate 4",\
"Frontend file name = STRING : [256] crate.cpp",\
"",\
NULL }

#endif

#ifndef EXCL_CRATE_5

#define CRATE_5_SETTINGS_DEFINED

typedef struct {
  BOOL      master;
  INT       diagnostic_print_level;
  INT       operator_message_level;
  BOOL      enabled;
  BOOL      synchronous;
  struct {
    BYTE      software_trigger_mode;
    BYTE      hardware_trigger_mode;
    BYTE      self_trigger_mode;
    BOOL      trigger_positive_edge;
    DWORD     max_events_per_block;
    BOOL      event_aligned_readout;
    char      logic_level[4];
    BYTE      post_trigger_size;
    BOOL      gpi_acquisition_mode;
    BYTE      zero_suppression_mode;
    BOOL      event_packing;
    BOOL      ext_clock;
    DWORD     waveform_length;
    struct {
      BOOL      enable;
      float     offset;
      float     self_trigger_threshhold;
    } ch00;
    struct {
      BOOL      enable;
      float     offset;
      float     self_trigger_threshhold;
    } ch01;
    struct {
      BOOL      enable;
      float     offset;
      float     self_trigger_threshhold;
    } ch02;
    struct {
      BOOL      enable;
      float     offset;
      float     self_trigger_threshhold;
    } ch03;
  } caen;
} CRATE_5_SETTINGS;

#define CRATE_5_SETTINGS_STR(_name) char *_name[] = {\
"[.]",\
"Master = BOOL : n",\
"Diagnostic Print Level = INT : -1079790036",\
"Operator Message Level = INT : -1079790036",\
"Enabled = BOOL : y",\
"Synchronous = BOOL : y",\
"",\
"[CAEN]",\
"software_trigger_mode = BYTE : 0",\
"hardware_trigger_mode = BYTE : 0",\
"self_trigger_mode = BYTE : 1",\
"trigger_positive_edge = BOOL : y",\
"max_events_per_block = DWORD : 1024",\
"event_aligned_readout = BOOL : y",\
"logic_level = STRING : [4] NIM",\
"post_trigger_size = BYTE : 20",\
"gpi_acquisition_mode = BOOL : y",\
"zero_suppression_mode = BYTE : 0",\
"event_packing = BOOL : n",\
"ext_clock = BOOL : y",\
"waveform length = DWORD : 64",\
"",\
"[CAEN/Ch00]",\
"enable = BOOL : y",\
"offset = FLOAT : 0",\
"self_trigger_threshhold = FLOAT : -0.2",\
"",\
"[CAEN/Ch01]",\
"enable = BOOL : y",\
"offset = FLOAT : 0",\
"self_trigger_threshhold = FLOAT : -0.2",\
"",\
"[CAEN/Ch02]",\
"enable = BOOL : y",\
"offset = FLOAT : 0",\
"self_trigger_threshhold = FLOAT : -0.2",\
"",\
"[CAEN/Ch03]",\
"enable = BOOL : y",\
"offset = FLOAT : 0",\
"self_trigger_threshhold = FLOAT : -0.2",\
"",\
NULL }

#define CRATE_5_COMMON_DEFINED

typedef struct {
  WORD      event_id;
  WORD      trigger_mask;
  char      buffer[32];
  INT       type;
  INT       source;
  char      format[8];
  BOOL      enabled;
  INT       read_on;
  INT       period;
  double    event_limit;
  DWORD     num_subevents;
  INT       log_history;
  char      frontend_host[32];
  char      frontend_name[32];
  char      frontend_file_name[256];
} CRATE_5_COMMON;

#define CRATE_5_COMMON_STR(_name) char *_name[] = {\
"[.]",\
"Event ID = WORD : 1000",\
"Trigger mask = WORD : 65535",\
"Buffer = STRING : [32] BUF5",\
"Type = INT : 66",\
"Source = INT : 16777215",\
"Format = STRING : [8] MIDAS",\
"Enabled = BOOL : y",\
"Read on = INT : 1",\
"Period = INT : 500",\
"Event limit = DOUBLE : 0",\
"Num subevents = DWORD : 0",\
"Log history = INT : 0",\
"Frontend host = STRING : [32] abner",\
"Frontend name = STRING : [32] Crate 5",\
"Frontend file name = STRING : [256] crate.cpp",\
"",\
NULL }

#endif

#ifndef EXCL_VACUUMMONITOR

#define VACUUMMONITOR_COMMON_DEFINED

typedef struct {
  WORD      event_id;
  WORD      trigger_mask;
  char      buffer[32];
  INT       type;
  INT       source;
  char      format[8];
  BOOL      enabled;
  INT       read_on;
  INT       period;
  double    event_limit;
  DWORD     num_subevents;
  INT       log_history;
  char      frontend_host[32];
  char      frontend_name[32];
  char      frontend_file_name[256];
} VACUUMMONITOR_COMMON;

#define VACUUMMONITOR_COMMON_STR(_name) char *_name[] = {\
"[.]",\
"Event ID = WORD : 24",\
"Trigger mask = WORD : 0",\
"Buffer = STRING : [32] SYSTEM",\
"Type = INT : 1",\
"Source = INT : 0",\
"Format = STRING : [8] MIDAS",\
"Enabled = BOOL : y",\
"Read on = INT : 511",\
"Period = INT : 1000",\
"Event limit = DOUBLE : 0",\
"Num subevents = DWORD : 0",\
"Log history = INT : 0",\
"Frontend host = STRING : [32] pony",\
"Frontend name = STRING : [32] VacuumMonitor",\
"Frontend file name = STRING : [256] frontend.c",\
"",\
NULL }

#endif

#ifndef EXCL_CRATE_6

#define CRATE_6_SETTINGS_DEFINED

typedef struct {
  BOOL      enabled;
  BOOL      master;
  BOOL      synchronous;
  INT       diagnostic_print_level;
  INT       operator_message_level;
  struct {
    BOOL      enabled_status;
    DWORD     vme_address;
    INT       ext_clock_frequency_mhz;
    BOOL      active_readout_mode;
    BOOL      record_trailing_edges;
    INT       half_full_level;
    INT       active_readout_buffer_size;
    struct {
      INT       channel[128];
    } wiring;
    struct {
      BOOL      channel_enabled[128];
    } channels;
  } caen_0;
} CRATE_6_SETTINGS;

#define CRATE_6_SETTINGS_STR(_name) char *_name[] = {\
"[.]",\
"Enabled = BOOL : y",\
"Master = BOOL : n",\
"Synchronous = BOOL : y",\
"Diagnostic Print Level = INT : 0",\
"Operator Message Level = INT : -1",\
"",\
"[CAEN 0]",\
"enabled status = BOOL : y",\
"vme address = DWORD : 134283264",\
"ext clock frequency MHz = INT : 25",\
"active readout mode = BOOL : y",\
"Record trailing edges = BOOL : n",\
"Half-full level = INT : 16000",\
"Active readout buffer size = INT : 131072",\
"",\
"[CAEN 0/wiring]",\
"channel = INT[128] :",\
"[0] 6500",\
"[1] 11549",\
"[2] 6011",\
"[3] 11551",\
"[4] 11552",\
"[5] 11553",\
"[6] 11554",\
"[7] 6805",\
"[8] 6050",\
"[9] 6051",\
"[10] 11500",\
"[11] 11501",\
"[12] 66803",\
"[13] 11503",\
"[14] 11504",\
"[15] 66002",\
"[16] 11197",\
"[17] 11198",\
"[18] 11199",\
"[19] 11200",\
"[20] 11201",\
"[21] 11202",\
"[22] 11203",\
"[23] 11204",\
"[24] 11205",\
"[25] 11206",\
"[26] 11207",\
"[27] 11208",\
"[28] 11209",\
"[29] 11210",\
"[30] 11211",\
"[31] 11212",\
"[32] 4017",\
"[33] 4018",\
"[34] 4019",\
"[35] 4020",\
"[36] 4021",\
"[37] 4022",\
"[38] 4023",\
"[39] 4024",\
"[40] 4067",\
"[41] 4068",\
"[42] 4069",\
"[43] 4070",\
"[44] 4071",\
"[45] 4072",\
"[46] 4073",\
"[47] 4074",\
"[48] 4051",\
"[49] 4052",\
"[50] 4053",\
"[51] 4054",\
"[52] 4055",\
"[53] 4056",\
"[54] 4057",\
"[55] 4058",\
"[56] 4059",\
"[57] 4060",\
"[58] 4061",\
"[59] 4062",\
"[60] 4063",\
"[61] 4064",\
"[62] 4065",\
"[63] 4066",\
"[64] 4001",\
"[65] 4002",\
"[66] 4003",\
"[67] 4004",\
"[68] 4005",\
"[69] 4006",\
"[70] 4007",\
"[71] 4008",\
"[72] 4009",\
"[73] 4010",\
"[74] 4011",\
"[75] 4012",\
"[76] 4013",\
"[77] 4014",\
"[78] 4015",\
"[79] 4016",\
"[80] 4108",\
"[81] 4107",\
"[82] 4106",\
"[83] 4105",\
"[84] 4104",\
"[85] 4103",\
"[86] 4102",\
"[87] 4101",\
"[88] 4167",\
"[89] 4168",\
"[90] 4169",\
"[91] 4170",\
"[92] 4171",\
"[93] 4172",\
"[94] 4173",\
"[95] 4174",\
"[96] 4151",\
"[97] 4152",\
"[98] 4153",\
"[99] 4154",\
"[100] 4155",\
"[101] 4156",\
"[102] 4157",\
"[103] 4158",\
"[104] 4159",\
"[105] 4160",\
"[106] 4161",\
"[107] 4162",\
"[108] 4163",\
"[109] 4164",\
"[110] 4165",\
"[111] 4166",\
"[112] 11293",\
"[113] 11294",\
"[114] 11295",\
"[115] 11296",\
"[116] 11297",\
"[117] 11298",\
"[118] 11299",\
"[119] 11300",\
"[120] 11301",\
"[121] 11302",\
"[122] 11303",\
"[123] 11304",\
"[124] 11305",\
"[125] 11306",\
"[126] 11307",\
"[127] 11308",\
"",\
"[CAEN 0/channels]",\
"channel enabled = BOOL[128] :",\
"[0] y",\
"[1] y",\
"[2] y",\
"[3] y",\
"[4] y",\
"[5] y",\
"[6] y",\
"[7] y",\
"[8] y",\
"[9] y",\
"[10] y",\
"[11] y",\
"[12] y",\
"[13] y",\
"[14] y",\
"[15] y",\
"[16] y",\
"[17] y",\
"[18] y",\
"[19] y",\
"[20] y",\
"[21] y",\
"[22] y",\
"[23] y",\
"[24] y",\
"[25] y",\
"[26] y",\
"[27] y",\
"[28] y",\
"[29] y",\
"[30] y",\
"[31] y",\
"[32] y",\
"[33] y",\
"[34] y",\
"[35] y",\
"[36] y",\
"[37] y",\
"[38] y",\
"[39] y",\
"[40] y",\
"[41] y",\
"[42] y",\
"[43] y",\
"[44] y",\
"[45] y",\
"[46] y",\
"[47] y",\
"[48] y",\
"[49] y",\
"[50] y",\
"[51] y",\
"[52] y",\
"[53] y",\
"[54] y",\
"[55] y",\
"[56] y",\
"[57] y",\
"[58] y",\
"[59] y",\
"[60] y",\
"[61] y",\
"[62] y",\
"[63] y",\
"[64] y",\
"[65] y",\
"[66] y",\
"[67] y",\
"[68] y",\
"[69] y",\
"[70] y",\
"[71] y",\
"[72] y",\
"[73] y",\
"[74] y",\
"[75] y",\
"[76] y",\
"[77] y",\
"[78] y",\
"[79] y",\
"[80] y",\
"[81] y",\
"[82] y",\
"[83] y",\
"[84] y",\
"[85] y",\
"[86] y",\
"[87] y",\
"[88] y",\
"[89] y",\
"[90] y",\
"[91] y",\
"[92] y",\
"[93] y",\
"[94] y",\
"[95] y",\
"[96] y",\
"[97] y",\
"[98] y",\
"[99] y",\
"[100] y",\
"[101] y",\
"[102] y",\
"[103] y",\
"[104] y",\
"[105] y",\
"[106] y",\
"[107] y",\
"[108] y",\
"[109] y",\
"[110] y",\
"[111] y",\
"[112] y",\
"[113] y",\
"[114] y",\
"[115] y",\
"[116] y",\
"[117] y",\
"[118] y",\
"[119] y",\
"[120] y",\
"[121] y",\
"[122] y",\
"[123] y",\
"[124] y",\
"[125] y",\
"[126] y",\
"[127] y",\
"",\
NULL }

#define CRATE_6_COMMON_DEFINED

typedef struct {
  WORD      event_id;
  WORD      trigger_mask;
  char      buffer[32];
  INT       type;
  INT       source;
  char      format[8];
  BOOL      enabled;
  INT       read_on;
  INT       period;
  double    event_limit;
  DWORD     num_subevents;
  INT       log_history;
  char      frontend_host[32];
  char      frontend_name[32];
  char      frontend_file_name[256];
} CRATE_6_COMMON;

#define CRATE_6_COMMON_STR(_name) char *_name[] = {\
"[.]",\
"Event ID = WORD : 1000",\
"Trigger mask = WORD : 65535",\
"Buffer = STRING : [32] BUF6",\
"Type = INT : 66",\
"Source = INT : 16777215",\
"Format = STRING : [8] MIDAS",\
"Enabled = BOOL : y",\
"Read on = INT : 1",\
"Period = INT : 500",\
"Event limit = DOUBLE : 0",\
"Num subevents = DWORD : 0",\
"Log history = INT : 0",\
"Frontend host = STRING : [32] fe6",\
"Frontend name = STRING : [32] Crate 6",\
"Frontend file name = STRING : [256] crate.cpp",\
"",\
NULL }

#endif

#ifndef EXCL_GERMANIUM

#define GERMANIUM_ALARMCHECK_DEFINED

typedef struct {
  INT       lastfilltime;
} GERMANIUM_ALARMCHECK;

#define GERMANIUM_ALARMCHECK_STR(_name) char *_name[] = {\
"[.]",\
"LastFillTime = INT : 28741",\
"",\
NULL }

#define GERMANIUM_SETTINGS_DEFINED

typedef struct {
  BOOL      master;
  INT       diagnostic_print_level;
  INT       operator_message_level;
  BOOL      enabled;
  BOOL      synchronous;
  struct {
    BOOL      justfilled;
    char      lastfill[141];
    INT       timesincefill;
  } alarm;
} GERMANIUM_SETTINGS;

#define GERMANIUM_SETTINGS_STR(_name) char *_name[] = {\
"[.]",\
"Master = BOOL : n",\
"Diagnostic Print Level = INT : -1079790036",\
"Operator Message Level = INT : -1079790036",\
"Enabled = BOOL : y",\
"Synchronous = BOOL : y",\
"",\
"[Alarm]",\
"JustFilled = BOOL : n",\
"LastFill = STRING : [141] Thu Dec  5 02:04:01 CET 2013",\
"TimeSinceFill = INT : 14281",\
"",\
NULL }

#define GERMANIUM_COMMON_DEFINED

typedef struct {
  WORD      event_id;
  WORD      trigger_mask;
  char      buffer[32];
  INT       type;
  INT       source;
  char      format[8];
  BOOL      enabled;
  INT       read_on;
  INT       period;
  double    event_limit;
  DWORD     num_subevents;
  INT       log_history;
  char      frontend_host[32];
  char      frontend_name[32];
  char      frontend_file_name[256];
} GERMANIUM_COMMON;

#define GERMANIUM_COMMON_STR(_name) char *_name[] = {\
"[.]",\
"Event ID = WORD : 1000",\
"Trigger mask = WORD : 65535",\
"Buffer = STRING : [32] SYSTEM",\
"Type = INT : 66",\
"Source = INT : 16777215",\
"Format = STRING : [8] MIDAS",\
"Enabled = BOOL : y",\
"Read on = INT : 1",\
"Period = INT : 500",\
"Event limit = DOUBLE : 0",\
"Num subevents = DWORD : 0",\
"Log history = INT : 0",\
"Frontend host = STRING : [32] abner",\
"Frontend name = STRING : [32] Germanium",\
"Frontend file name = STRING : [256] crate.cpp",\
"",\
NULL }

#define GERMANIUM_CALIBRATION_DEFINED

typedef struct {
  double    constant;
  double    slope;
} GERMANIUM_CALIBRATION;

#define GERMANIUM_CALIBRATION_STR(_name) char *_name[] = {\
"[.]",\
"constant = DOUBLE : 80",\
"slope = DOUBLE : 4",\
"",\
NULL }

#endif

#ifndef EXCL_CRATE_7

#define CRATE_7_SETTINGS_DEFINED

typedef struct {
  BOOL      master;
} CRATE_7_SETTINGS;

#define CRATE_7_SETTINGS_STR(_name) char *_name[] = {\
"[.]",\
"Master = BOOL : y",\
"",\
NULL }

#define CRATE_7_COMMON_DEFINED

typedef struct {
  INT       log_history;
  INT       type;
  char      frontend_name[256];
  char      format[80];
} CRATE_7_COMMON;

#define CRATE_7_COMMON_STR(_name) char *_name[] = {\
"[.]",\
"Log history = INT : 0",\
"Type = INT : 66",\
"Frontend name = STRING : [256] Germanium",\
"Format = STRING : [80] ",\
"",\
NULL }

#endif

#ifndef EXCL_CRATE_3

#define CRATE_3_SETTINGS_DEFINED

typedef struct {
  BOOL      master;
  INT       diagnostic_print_level;
  INT       operator_message_level;
  BOOL      enabled;
  BOOL      synchronous;
} CRATE_3_SETTINGS;

#define CRATE_3_SETTINGS_STR(_name) char *_name[] = {\
"[.]",\
"Master = BOOL : y",\
"Diagnostic Print Level = INT : -1074740852",\
"Operator Message Level = INT : -1074740852",\
"Enabled = BOOL : y",\
"Synchronous = BOOL : y",\
"",\
NULL }

#define CRATE_3_COMMON_DEFINED

typedef struct {
  WORD      event_id;
  WORD      trigger_mask;
  char      buffer[32];
  INT       type;
  INT       source;
  char      format[8];
  BOOL      enabled;
  INT       read_on;
  INT       period;
  double    event_limit;
  DWORD     num_subevents;
  INT       log_history;
  char      frontend_host[32];
  char      frontend_name[32];
  char      frontend_file_name[256];
} CRATE_3_COMMON;

#define CRATE_3_COMMON_STR(_name) char *_name[] = {\
"[.]",\
"Event ID = WORD : 1000",\
"Trigger mask = WORD : 65535",\
"Buffer = STRING : [32] BUF3",\
"Type = INT : 66",\
"Source = INT : 16777215",\
"Format = STRING : [8] MIDAS",\
"Enabled = BOOL : y",\
"Read on = INT : 1",\
"Period = INT : 500",\
"Event limit = DOUBLE : 0",\
"Num subevents = DWORD : 0",\
"Log history = INT : 0",\
"Frontend host = STRING : [32] doe-dhcp253.bu.edu",\
"Frontend name = STRING : [32] Crate 3",\
"Frontend file name = STRING : [256] crate.cpp",\
"",\
NULL }

#endif

