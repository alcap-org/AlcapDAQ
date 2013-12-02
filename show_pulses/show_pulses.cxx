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
//#include "/home/nam/fadc_software/midas/include/midas.h"
#include "midas.h"
#include "../analyzer/compress/mucap_compress.h"
#endif

#include "TH1.h"
#include "TCanvas.h"

#define PRINT_SAMPLES 1

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

void process_file(char *input_filename, int addr, int fadc)
{
	FILE *input_fp;
	char *input_event;
	int event_number;

	c = new TCanvas();

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
		printf("event_number %d\n", event_number);
	}

	fclose(input_fp);
}

int processIsland(int t0, int nsamples, int *samples, int channel)
{

	c->Clear();
	char title[80];
	sprintf(title, "Waveform: channel %d, t0=%d", channel, t0);

	double sum = 0;
	int min = 4097;
	int max = -1;

	double binWidth = 100./100.00;
	TH1D *waveform = new TH1D("waveform", title, nsamples, -0.5*binWidth, (nsamples-0.5)*binWidth);
	for(int i = 0; i < nsamples; i++) {
		waveform->SetBinContent(i+1, samples[i]);
		sum += samples[i];

		if(samples[i] < min) min = samples[i];
		if(samples[i] > max) max = samples[i];
	}
	waveform->Draw();

	double mean = sum/nsamples;
	double sumSqrDeviations = 0;

	for(int i = 0; i < nsamples; i++) {
		double deviation = samples[i] - mean;
		sumSqrDeviations += deviation*deviation;
	}
	double rms = sqrt(sumSqrDeviations/(nsamples-1));

	printf("Number of samples = %d, Min = %d, Max = %d, Mean = %lf, RMS = %lf\n", nsamples, min, max, mean, rms);

	//  if(max > 3500) {
	c->Update();
	c->Modified();

	char dummy[80];
	fgets(dummy, sizeof(dummy), stdin);
	if(dummy[0] == 'q') {
		return 1;
	}
//}

	delete waveform;
	//delete c;

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
			int stop = processIsland(islandT0*2, islandNsamples, islandSamples, i);
#ifdef PRINT_SAMPLES
			printf("*\n");
#endif
			if (stop) return 1;
		}
	}

	return 0;
}
