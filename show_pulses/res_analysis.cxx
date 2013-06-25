#define _FILE_OFFSET_BITS 64

/*
 * slow_extract
 *
 * Extracts the slow control data (everything except the primary event ID 1)
 * from a MIDAS data file.
 */

#include <stdio.h>
#include <stdlib.h>

#ifndef __CINT__
#include "/home/nam/fadc_software/midas/include/midas.h"
#include "../../compress/mucap_compress.h"
#endif

#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"

// #define PRINT_SAMPLES 1

TCanvas *c = 0;
void process_file(char *input_filename, int addr, int fadc);
int process_event(void *pevent, int addr, int fadc);


bool get_input_event(FILE * fp, char *pevent, int event_number)
{
    int event_size, size_read;
    EVENT_HEADER *header;

    size_read = fread(pevent, 1, sizeof(EVENT_HEADER), fp);
    if (size_read != sizeof(EVENT_HEADER))
	return !SUCCESS;

    header = (EVENT_HEADER *) pevent;

    if (event_number == 0 && header->trigger_mask != MIDAS_MAGIC) {
	fprintf(stderr, "Input is not a valid MIDAS file\n");
	exit(1);
    }

    event_size = header->data_size;

    if ((unsigned int) event_size >= MAX_EVENT_SIZE - sizeof(EVENT_HEADER)) {
	fprintf(stderr, "Invalid event size %d\n", event_size);
	exit(1);
    }

    size_read = fread(pevent + sizeof(EVENT_HEADER), 1, event_size, fp);
    if (size_read != event_size)
	return !SUCCESS;

    return SUCCESS;
}

TH2 *hSamples;
TH1 *hChannelNSample;
TH1 *hChannelNIsland;
TH2 *hMinSample;
TH2 *hMaxSample;
TH2 *hPulseEnergy;
TH2 *hPed0;

void process_file(char *input_filename, int addr, int fadc)
{
    FILE *input_fp;
    char *input_event;
    int event_number;

  hSamples = new TH2D("samples", "Sample profile", 8, -0.5, 7.5, 4096, -0.5, 4095.5);
  hPed0 = new TH2D("ped0", "Pedestal from first samples", 8, -0.5, 7.5, 4096, -0.5, 4095.5);
  hChannelNSample = new TH1D("channelNSample", "Number of samples vs. channel", 8, -0.5, 7.5);
  hChannelNIsland = new TH1D("channelNIsland", "Number of islands vs. channel", 8, -0.5, 7.5);
  hMinSample = new TH2D("minSample", "Minimum sample in island", 8, -0.5, 7.5, 8192, -0.5, 8191.5);
  hMaxSample = new TH2D("maxSample", "Maximum sample in island", 8, -0.5, 7.5, 8192, -0.5, 8191.5);
  hPulseEnergy = new TH2D("pulseEnergy", "Reconstructed energy of pulse", 8, -0.5, 7.5, 4000, 0.5, 12000.5);

  char host_name[256], exp_name[32];
  cm_get_environment(host_name, sizeof(host_name), exp_name, sizeof(exp_name));
  cm_connect_experiment(host_name, exp_name, "show_pulses", NULL);
   cm_get_experiment_database(&hDB, NULL);
   compress_load_all();

    /* Open input and output files */
    input_fp = fopen(input_filename, "rb");
    if (!input_fp) {
	fprintf(stderr, "Unable to open %s: ", input_filename);
	perror("");
	exit(1);
    }

    /* Allocate buffer to hold MIDAS event */
    input_event = (char *) malloc(MAX_EVENT_SIZE*2);

    /* Loop over MIDAS events */
    event_number = 0;
    while (get_input_event(input_fp, input_event, event_number) == SUCCESS) {

	EVENT_HEADER *header = (EVENT_HEADER *) input_event;

        if(header->event_id == 1) {
          expand_event(header+1, header+1);
          int stop = process_event(header + 1, addr, fadc);
          if(stop) break;
	}

	event_number++;
    }

    fclose(input_fp);
}

double findPulse(int nsamples, int *samples){
  int sig_length_counter = 0;
  double sig_max_height = 0.;
  int sig_first_index = 0;
  double sig_integral = 0.;
  int sig_over_thr = 0;
  
  bool hasOvershoot = false;
  bool hasUndershoot = false;
  const int kLengthThreshold = 4;

  // (Serdar) i increased nsamples to 40 (was 16)

  if(nsamples < 40 ) return 0.;

  // Let's first find the pedestal 

  // (Serdar) here i check whether selecting the first 5 sample or 10 sample do change anything
  // on the pedestal determination, see histograms histPeds0first05bins or 10bins for more info
  // and i find both can be used, so i select first 10 bins to get more info.

  const int pedSamples = 10;
  double pedestalOffset = 0;
  double sumpedestal2 = 0;
  double pedestalsigma = 0 ;

  if(pedSamples > nsamples) return 0.;

  for(int i = 0; i < pedSamples; i++) {
    pedestalOffset += samples[i];
  }
  pedestalOffset /= pedSamples;

  for(int i = 0; i < pedSamples; i++) {
    sumpedestal2 += (pedestalOffset-samples[i])*(pedestalOffset-samples[i]);
  }
  pedestalsigma= sqrt(sumpedestal2/pedSamples);

  // printf("Pedestal = %f\n", pedestalOffset);
  // Pedestal found, let's look for a pulse now

  // (Serdar) here i calculated the sigma for pedestal calculation, then i will start
  // searching the pulse if sample is bigger the 4 pedestal sigma,

  const int kThreshold = 4*pedestalsigma;

  for(int i = 0; i < nsamples; i++) {
    double bin_content = -1.*((double)samples[i] - pedestalOffset);
    
    double next_bin_content = 0.;
    if(i<nsamples-1) next_bin_content = -1.*((double)samples[i+1] - pedestalOffset);

    if(bin_content >= kThreshold || next_bin_content >= kThreshold){
      sig_length_counter++;
      sig_over_thr++;
      
      if (sig_length_counter == 1){
	sig_first_index = i;
	// inspect left shoulder of this pulse
	int back;
	for(back=i-1; back>=0; back--){
	  double cont = -1.*((double)samples[back] - pedestalOffset);

	  // (Serdar) here i made 2 changes, first, instead of 1/4 of the kThreshold
	  // i have 3/4, which equals 3 sigma at the left shoulder

	  if(cont > 3*kThreshold/4 && (i-back)<4){ 
	    // accept it still as part of the left shoulder
	    sig_length_counter++;
	    sig_first_index = back;		
	  }
	  else break;
	}
      }
    }
    else{
      if (((bin_content < kThreshold) || (i == nsamples - 1)) && 
	  (sig_over_thr >= kLengthThreshold)){
	// It's the end of the pulse, so let's include part of the right 
	// shoulder
	int forw;  // index for samples right of momentary position
	for(forw=i+1;forw<nsamples; forw++){
	  double cont = -1.*((double)samples[forw] - pedestalOffset);
	  if(cont > kThreshold/4 && (forw-i)<4){
	    // accept it still as part of the left shoulder
	    sig_length_counter++;
	  }
	  else break;  // probably pedestal so let's stop
	  i = forw;
	}
	
	sig_integral = 0.;
	sig_max_height = 0.;
	
	for(int i = sig_first_index; i < sig_first_index+sig_length_counter; i++){
	  double bin_content = -1.*((double)samples[i] - pedestalOffset);
	  sig_integral += bin_content;
	  if (bin_content > sig_max_height){
	    sig_max_height = bin_content;
	  }
	}
	
	// Let's only deal with one pulse at this time!!!
	break;
      }
    }
  }

//  printf("Pulse accepted from sample %d to %d with energy %f\n", sig_first_index,
//	 sig_first_index+sig_length_counter, sig_integral);

  return sig_integral;
}

int processIsland(int t0, int nsamples, int *samples, int channel)
{
  hChannelNIsland->Fill(channel);

  if(nsamples < 16) return 0;

  int minI = -1;
  int maxI = -1;
  int minSample = 4097;;
  int maxSample = -1;

  double energy = findPulse(nsamples, samples);

  for(int i = 0; i < nsamples; i++) {
    hSamples->Fill(channel, samples[i]);
    hChannelNSample->Fill(channel);
    if(samples[i] > maxSample) {
      maxI = i;
      maxSample = samples[i];
    }
    if(samples[i] < minSample) {
      minI = i;
      minSample = samples[i];
    }
    if(i<10){
      hPed0->Fill(channel, samples[i]);
    }
  }

  hMinSample->Fill(channel, minSample);
  hMaxSample->Fill(channel, maxSample);
  hPulseEnergy->Fill(channel, energy);

  return 0;
}

int process_event(void *pevent, int addr, int fadc)
{
  for(int i = 0; i < 8; i++) {

    if(i != fadc && fadc >= 0) continue;

    // get the bank
    char name[80];
    sprintf(name, "N%c%02x", 'a' + i, addr);
    unsigned char *raw;
    int bank_size = bk_locate(pevent, name, &raw);
    bank_size = bank_size / 10;

#ifdef PRINT_SAMPLES
    printf("-------------------------------------------------------------------------------\n");
#endif
 
    int islandSamples[8192];
    int islandNsamples = 0;
    int islandT0 = -1;
    bool firstIsland = true;
 
    // loop through words to build up "islands"
    int lastTimestamp = 0;
    for(int j = 0; j < bank_size; j++) {

      // data format:
      //
      //  bits 
      //  78-52       timestamp
      //  51-48       overflow
      //  47-36       sampleB0
      //  35-24       sampleA0
      //  23-12       sampleB1
      //  11-0        sampleA1
      int timestamp =  (raw[j*10+0] << 20) |
                       (raw[j*10+1] << 12) |
                       (raw[j*10+2] << 4) |
                       (raw[j*10+3] >> 4);
      bool overflowB0 = ((raw[j*10+3] & 0x08) != 0);
      bool overflowA0 = ((raw[j*10+3] & 0x04) != 0);
      bool overflowB1 = ((raw[j*10+3] & 0x02) != 0);
      bool overflowA1 = ((raw[j*10+3] & 0x01) != 0);
      int sampleB0 = (overflowB0 << 12) |
                     (raw[j*10+4] << 4) |
                     (raw[j*10+5] >> 4);  
      int sampleA0 = (overflowA0 << 12) |
                     ((raw[j*10+5] & 0xf) << 8) |
                     (raw[j*10+6]);  
      int sampleB1 = (overflowB1 << 12) |
                     (raw[j*10+7] << 4) |
                     (raw[j*10+8] >> 4);  
      int sampleA1 = (overflowA1 << 12) |
                     ((raw[j*10+8] & 0xf) << 8) |
                     (raw[j*10+9]);  

     if(timestamp < lastTimestamp) {
       printf("Time ordering error!\n");
     }

      if(timestamp != lastTimestamp + 1) {
        if(!firstIsland) {
          int stop = processIsland(islandT0*2, islandNsamples, islandSamples, i);
#ifdef PRINT_SAMPLES
          printf("*\n");
#endif

          if(stop) {
            return 1;
          }
        }
        islandT0 = timestamp; 
        islandNsamples = 0;
        firstIsland = false;
      }
      lastTimestamp = timestamp;

#ifdef PRINT_SAMPLES
      printf("fadc=%d t=%d %d %d %d %d\n", i, timestamp, 
             sampleA1, sampleB1, sampleA0,sampleB0);
#endif

      islandSamples[islandNsamples++] = sampleA1;
      islandSamples[islandNsamples++] = sampleB1;
      islandSamples[islandNsamples++] = sampleA0;
      islandSamples[islandNsamples++] = sampleB0;
    }

    if(islandNsamples != 0) {
      processIsland(islandT0*2, islandNsamples, islandSamples, i);
#ifdef PRINT_SAMPLES
       printf("*\n");
#endif
    }
  }

  return 0;
}
