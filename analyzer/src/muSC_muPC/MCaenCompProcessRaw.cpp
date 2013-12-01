/********************************************************************\

  Name:         MCaenCompProcessERaw.cpp
  Created by:   Fred Gray

  Contents:     Module to merge raw CAEN TDC and compressor hits into 
                a time-ordered bank in terms of time and physics
                parameter number.
                
\********************************************************************/
                                                        
/*-- Include files -------------------------------------------------*/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include "TH1.h"
#include "TH2.h"

/* MuCap includes */
#include "common.h"

/*-- Parameters ----------------------------------------------------*/

/*-- Static parameters ---------------------------------------------*/

const int max_streams = kMaxCaens + kMaxComps;
const int initial_stream_size = 1024;
const int max_caen_trailing_hits = 64*1024;

double caen_clock_period[kMaxCaens];
double comp_clock_period[kMaxComps];
int caen_wiring_map[kNumCaenChannels][kMaxCaens];
int comp_wiring_map[kNumCompFpgaChannels][kMaxCompFpgas][kMaxComps];
channel_hit *temp_hits[max_streams];
int temp_hits_size[max_streams];
caen_hit *caen_trailing_hits;

bool caen_histograms_inited[kMaxCaens];
static TH1 *hCaenErrorBlocksSkipped;
static TH1 *hCaenErrorDAQ[kMaxCaens];
static TH2 *hCaenErrorDAQ_TDCInfo[kMaxCaens];
static TH1 *hCaenErrorEOB[kMaxCaens];
static TH1 *hCaenErrorTrailingEdgeStreamLimit[kMaxCaens];
static TH1 *hCaenErrorTrailingEdgeBlockLimit[kMaxCaens];
static TH1 *hCaenLeadingEdgesByChannel[kMaxCaens];
static TH1 *hCaenLeadingEdgesByBlock[kMaxCaens];
static TH1 *hCaenLeadingEdges32BitInterpolator[kMaxCaens];
static TH1 *hCaenLeadingEdges32BitInterpolatorNoRollover[kMaxCaens];
static TH1 *hCaenTrailingEdgesByChannel[kMaxCaens];
static TH1 *hCaenTrailingEdgesByBlock[kMaxCaens];
static TH1 *hCaenLastTime[kMaxCaens];

bool comp_histograms_inited[kMaxCaens];
static TH1 *hCompErrorBlocksSkipped;
static TH1 *hCompErrorStreamLimit[kMaxComps];
static TH1 *hCompBackwards[kMaxComps];
static TH1 *hCompBackwardsLimit[kMaxComps];
static TH1 *hCompLastTime[kMaxComps];

double beam_rate;

/*-- Module declaration --------------------------------------------*/

INT  MCaenCompProcessRaw_init(void);
INT  MCaenCompProcessRaw(EVENT_HEADER*, void*);
void double_stream_size(channel_hit **output_bank_handle, 
	 	        int *output_bank_size);
void sort_hits(int num_hits, channel_hit *hits);
INT  merge_hits(int num_streams, int *num_hits, 
		channel_hit **hits, channel_hit *output);
INT  make_caen(int caen, void *pevent, channel_hit **output_bank_handle, 
		int *output_bank_size, bool *bad_block,
	        caen_hit *caen_trailing_hits, int *num_caen_trailing_hits);
void init_caen_histograms(int caen);
INT  make_comp(int comp, void *pevent, channel_hit **output_bank_handle, 
		int *output_bank_size, bool *bad_block);
void init_comp_histograms(int comp);

extern HNDLE hDB;

ANA_MODULE MCaenCompProcessRaw_module = {
  "MCaenCompProcessRaw",         /* module name           */  
  "Fred Gray",                   /* author                */
  MCaenCompProcessRaw,           /* event routine         */
  NULL,                          /* BOR routine           */
  NULL,                          /* EOR routine           */
  MCaenCompProcessRaw_init,      /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/*-- init routine --------------------------------------------------*/

INT MCaenCompProcessRaw_init(void)
{
  HNDLE hKey;

  // Search through the ODB for the CAENs' clock and wiring information
  for (int caen = 0; caen < kMaxCaens; caen++) {
    for (int crate = 1; crate <= kMaxCrates; crate++) {
      char key_path[80];
      char key_name[80];
      sprintf(key_path, "Equipment/Crate %d/Settings/CAEN %d", crate, caen);
      if (db_find_key(hDB, 0, key_path, &hKey) == SUCCESS) {

	// get the clock information
	sprintf(key_name, "%s/ext clock frequency MHz", key_path);
	int caen_freq;
	int size = sizeof(caen_freq);
	db_get_value(hDB, 0, key_name, &caen_freq, &size, TID_INT, 0);
	if (caen_freq != 25) {
	  fprintf(stderr, "Error: CAEN %d clock freq found as %d, should be 25 (ODB problem)\n", caen, caen_freq);
	}
	caen_clock_period[caen] = 1000.0/caen_freq/32;

	// get the wiring information
	sprintf(key_name, "%s/wiring/channel", key_path);
	int caen_wiring[kNumCaenChannels];
	size = sizeof(caen_wiring);
	db_get_value(hDB, 0, key_name, caen_wiring, &size, TID_INT, 0);
	for (int chan = 0; chan < kNumCaenChannels; chan++) {
	  caen_wiring_map[chan][caen] = caen_wiring[chan];
	}

	break;
      }
    }
  }

  // Search through the ODB for the COMPs' clock and wiring information
  for (int comp = 0; comp < kMaxComps; comp++) {
    for (int crate = 1; crate <= kMaxCrates; crate++) {
      char key_path[80];
      char key_name[80];      
      sprintf(key_path, "Equipment/Crate %d/Settings/COMP %d", crate, comp);
      if (db_find_key(hDB, 0, key_path, &hKey) == SUCCESS) {
	
	// get the clock information
	sprintf(key_name, "%s/ext clock frequency MHz", key_path);
	int comp_freq;
	int size = sizeof(comp_freq);
	db_get_value(hDB, 0, key_name, &comp_freq, &size, TID_INT, 0);
	if (comp_freq != 50) {
	  fprintf(stderr, "Error: COMP %d clock freq found as %d, should be 50 (ODB problem)\n", comp, comp_freq);
	}
	comp_clock_period[comp] = 1500.0/comp_freq;
	
	// get the wiring information
	for (int fpga = 0; fpga < kMaxCompFpgas; fpga++) {
	  sprintf(key_name, "%s/wiring/FPGA %d/channel", key_path, fpga);
	  if (db_find_key(hDB, 0, key_name, &hKey) == SUCCESS) {
	    int comp_wiring[kNumCompFpgaChannels];
	    size = sizeof(comp_wiring);
	    db_get_value(hDB, 0, key_name, comp_wiring, &size, TID_INT, 0);
	    for (int chan = 0; chan < kNumCompFpgaChannels; chan++) {
	      comp_wiring_map[chan][fpga][comp] = comp_wiring[chan];
	    }
	  }
	}

	break;
      }
    }
  }

  // Create the hit streams
  for (int stream=0; stream<max_streams; stream++) {
    temp_hits[stream] = new channel_hit[initial_stream_size];
    temp_hits_size[stream] = initial_stream_size;
  }

  // Create a stream for CAEN trailing edge times
  caen_trailing_hits = new caen_hit[max_caen_trailing_hits];

  // Set CAEN-specific histogram initialization flags to false
  for(int caen = 0; caen < kMaxCaens; caen++) {
    caen_histograms_inited[caen] = false;
  }

  // Set COMP-specific histogram initialization flags to false
  for(int comp = 0; comp < kMaxComps; comp++) {
    comp_histograms_inited[comp] = false;
  }

  // Define general CAEN/COMP histograms
  char name[80], title[80];

  sprintf(name,  "blocks_skipped_caen_error");
  sprintf(title, "Blocks skipped due to CAEN errors");
  hCaenErrorBlocksSkipped = new TH1D(name, title, 
				     kMaxEvents, 0.5, kMaxEvents+0.5);

  sprintf(name,  "blocks_skipped_comp_error");
  sprintf(title, "Blocks skipped due to irresolvable COMP errors");
  hCompErrorBlocksSkipped = new TH1D(name, title, 
				     kMaxEvents, 0.5, kMaxEvents+0.5);

  return SUCCESS;
}

/*-- event routine -------------------------------------------------*/

INT MCaenCompProcessRaw(EVENT_HEADER *pheader, void *pevent)
{
  int num_hits[max_streams];
  int stream = 0;
  bool skip_block = false;

  // Process CAEN data.
  int num_caen_trailing_hits = 0;
  for(int caen = 0; caen < kMaxCaens; caen++) {
    bool bad_block = false;
    num_hits[stream] = make_caen(caen, pevent, &temp_hits[stream], 
		          &temp_hits_size[stream], &bad_block, 
			  caen_trailing_hits, &num_caen_trailing_hits);
    if(bad_block) skip_block = true;
    if(num_hits[stream] != 0) {
      stream++;
    }
  }

  if(skip_block) {
    fprintf(stderr, "Skipping block %d due to CAEN processing errors.\n", pheader->serial_number);
    hCaenErrorBlocksSkipped->Fill(pheader->serial_number);
    return ANA_SKIP;
  }

  // Process COMP data.
  for(int comp = 0; comp < kMaxComps; comp++) {
    bool bad_block = false;
    num_hits[stream] = make_comp(comp, pevent, &temp_hits[stream], 
    		                  &temp_hits_size[stream], &bad_block);
    if(bad_block) skip_block = true;
    if(num_hits[stream] != 0) {
      stream++;
    }
  }

  if(skip_block) {
    fprintf(stderr, "Skipping block %d due to COMP processing errors.\n", pheader->serial_number);
    hCompErrorBlocksSkipped->Fill(pheader->serial_number);
    return ANA_SKIP;
  }

  // Merge all CAEN and COMP data streams together into "HITS"
  channel_hit *output_bank;
  bk_create(pevent, "HITS", TID_DWORD, (DWORD *) &output_bank);
  int output_bank_size = merge_hits(stream, num_hits, temp_hits, output_bank);
  bk_close(pevent, output_bank + output_bank_size);

  // Write the CAEN trailing edge hit info into bank "CAET"
  caen_hit *caen_trailing_hit_bank;
  bk_create(pevent, "CAET", TID_DWORD, (DWORD *) &caen_trailing_hit_bank);
  for (int index = 0; index < num_caen_trailing_hits; index++) {
    caen_trailing_hit_bank[index].time = caen_trailing_hits[index].time;
    caen_trailing_hit_bank[index].caen = caen_trailing_hits[index].caen;
  }
  bk_close(pevent, caen_trailing_hit_bank + num_caen_trailing_hits);

  return SUCCESS;
}

/*-- subroutine ----------------------------------------------------*/

void double_stream_size(channel_hit **output_bank_handle, int *output_bank_size)
{
  int new_output_bank_size = *output_bank_size * 2;
  channel_hit *new_output_bank = new channel_hit[new_output_bank_size];
  memcpy(new_output_bank, *output_bank_handle, 
		  *output_bank_size*sizeof(channel_hit));
  delete[] *output_bank_handle;
  *output_bank_handle = new_output_bank;
  *output_bank_size = new_output_bank_size;
}

int hits_compare(const void *p1, const void *p2)
{
  channel_hit *hit1 = (channel_hit *) p1;
  channel_hit *hit2 = (channel_hit *) p2;

  if(hit1->time > hit2->time) {
    return 1;
  } else if(hit1->time < hit2->time) {
    return -1;
  } else {
    return 0;
  }
}

void sort_hits(int num_hits, channel_hit *hits)
{
  // For arbitrary inputs, the bubble sort is an extremely poor
  // algorithmic choice.  However, when the input is already nearly 
  // sorted, it can be a very good choice, as is the case here.

/*
  int sorted_through = 0;
  channel_hit temp;

  while(sorted_through < num_hits-1) {
    for(int i = sorted_through; i < num_hits-1; i++) {

      if(hits[i+1].time < hits[i].time) {
	// If necessary, swap two adjacent elements 
        temp = hits[i];
        hits[i] = hits[i+1];
        hits[i+1] = temp;
	if(i > 0) {
	  sorted_through = i-1;
	}
	break;
      } else {
        sorted_through = i+1;
      }
    }
  }
*/
	// sorry, the worst case is just too poor
  qsort(hits, num_hits, sizeof(channel_hit), hits_compare);

}

/*-- subroutine ----------------------------------------------------*/

INT merge_hits(int num_streams, int *num_hits, channel_hit **hits,
		channel_hit *output)
{
  int stream_ptr[num_streams];
  int total_num_hits = 0;

  for(int stream = 0; stream < num_streams; stream++) {
    total_num_hits += num_hits[stream]; 
    stream_ptr[stream] = 0;
  }

  for(int i = 0; i < total_num_hits; i++) {

    // look for the stream whose top-most unused element comes first
    int best_stream = 0;
    double best_time = 1e99;
    for(int stream = 0; stream < num_streams; stream++) {
      if(stream_ptr[stream] < num_hits[stream] && 
         hits[stream][stream_ptr[stream]].time < best_time) {
        best_stream = stream;
        best_time = hits[stream][stream_ptr[stream]].time;	
      }
    }

    // copy this earliest element to the output
    output[i] = hits[best_stream][stream_ptr[best_stream]];
    stream_ptr[best_stream]++;
  }

  return total_num_hits;
}

/*-- subroutine ----------------------------------------------------*/

INT make_caen(int caen, void *pevent, channel_hit **output_bank_handle,
              int *output_bank_size, bool *bad_block, 
	      caen_hit *caen_trailing_hits, int *num_caen_trailing_hits)
{
  channel_hit *output_bank = *output_bank_handle;

  // Open CAEN bank ("caen" == CAEN number)
  caen_word *caen_bank;
  char bank_name[8];
  sprintf(bank_name, "CAE%d", caen);
  int caen_bank_size = bk_locate(pevent, bank_name, (DWORD *) &caen_bank);
  if(caen_bank == NULL) return 0;
  int block_number = ((EVENT_HEADER *) pevent - 1)->serial_number;

  // Initialize caen-specific histograms if necessary
  if (!caen_histograms_inited[caen]) {
    init_caen_histograms(caen);
  }

  // This section loops over the CAEN data in "caen_bank" and performs 
  // the following tasks:
  //  (1) check for headers, EOBs, and DAQ-introduced error datums; 
  //  (2) count the numbers of rising and trailing edges;
  //  (3) track the linearity of the CAEN interpolator; 
  //  (4) calculate the absolute times of hit datums;
  //  (5) fill an output bank with leading edge hit info;
  //  (6) fill an output bank with trailing edge hit times.

  // Define and initialize useful arrays, counters, and flags
  double caen_bank_abstime[caen_bank_size];
  int  output_pointer = 0;
  int  prev_time_word = 0;
  int  num_overflows = 0;
  bool header_found = false;
  bool EOB_found = false;
  int  EOB_reported_data_count = 0;
  int  EOB_reported_error_status = 0;
  int  num_trailing_edges = 0;

  // Loop over the caen_bank elements
  for (int j=0; j<caen_bank_size; j++) {
    if(j<=2)
    // check for DAQ-introduced error datum; if found, fill appropriate histos 
    // and flag the block so that it can be skipped
    if(caen_bank[j].r2 == 3) {
      fprintf(stderr, "Encountered DAQ-generated error flag in block %d CAEN%d data at j=%d.\n", 
	      block_number, caen, j);
      if (!(*bad_block)) {
	hCaenErrorDAQ[caen]->Fill(1);
	*bad_block = true;
      }
      int error_bits = caen_bank[j].time;
      for (int TDC=0; TDC<4; TDC++) {
	if (error_bits & (1<<TDC)) {
	  hCaenErrorDAQ_TDCInfo[caen]->Fill(block_number, TDC);
 	}
      }
    }

    // check for header datum
    if (caen_bank[j].r2 == 2) {
      header_found = true;
    }

    // check for EOB datum; if found, extract reported data count and TDC chip
    // error status; if TDC chip error occurred, flag the block to be skipped
    if(caen_bank[j].r2 == 1) {
      EOB_found = true;
      EOB_reported_data_count = (caen_bank[j].time & 0xffff);
      EOB_reported_error_status = (caen_bank[j].channel & 0x07);
      if (EOB_reported_error_status) {
	fprintf(stderr, "Encountered EOB error flag in block %d CAEN%d data.\n", 
		block_number, caen);
	if (!(*bad_block)) {
	  hCaenErrorEOB[caen]->Fill(1);
	  *bad_block = true;
	}
      }
    }

    // in preparation for absolute timing calculations and output bank filling, 
    // ignore anything that isn't an ordinary datum 
    if(caen_bank[j].start != 0 || 
       caen_bank[j].r2 != 0) {
      continue;
    }

    // count/histogram the leading and trailing edge hit info; if there are too
    // many trailing edges, flag the block to be skipped
    if(caen_bank[j].edge == 1) {
      hCaenLeadingEdgesByChannel[caen]->Fill(caen_bank[j].channel);
      hCaenLeadingEdgesByBlock[caen]->Fill(block_number);
      hCaenLeadingEdges32BitInterpolator[caen]->Fill(caen_bank[j].time % 32);
      int parameter = caen_wiring_map[caen_bank[j].channel][caen];
      if (!((parameter>=6500) && (parameter<=6502))) {
	hCaenLeadingEdges32BitInterpolatorNoRollover[caen]->Fill(caen_bank[j].time % 32);
      }
    } else {
      num_trailing_edges++;
      if ((num_trailing_edges > kCaenTrailingEdgeLimit) && !(*bad_block)) {
	fprintf(stderr, "Encountered too many trailing edges in block %d CAEN%d data (%d).\n", 
		block_number, caen, num_trailing_edges);
	hCaenErrorTrailingEdgeBlockLimit[caen]->Fill(1);
//	*bad_block = true;
      }
      hCaenTrailingEdgesByChannel[caen]->Fill(caen_bank[j].channel);
      hCaenTrailingEdgesByBlock[caen]->Fill(block_number);
    }

    // Calculate the absolute times by dealing with time overflows: Because 
    // there are multiple levels of FIFO within the CAEN, backward-going times 
    // are normal features of the data and do not always indicate an overflow.  
    // Empirically, the distribution of these backward-going times seems to cut 
    // off at about -4000.  Meanwhile, if the minimum rate is 2.5 kHz, a backward 
    // step of at least 2^10 - 320000 is guaranteed for an overflow; a forward 
    // step of at least 320000 indicates the reversal of an overflow, which can
    // occur because of the multiple FIFO buffering.

    if(caen_bank[j].time < prev_time_word && 
       prev_time_word - caen_bank[j].time > 0x80000) {
      num_overflows++; 
    } else if(caen_bank[j].time > prev_time_word &&
	      caen_bank[j].time - prev_time_word > 0x80000) {
      num_overflows--;
    }
    prev_time_word = caen_bank[j].time;
    caen_bank_abstime[j] = 
      caen_clock_period[caen] * (caen_bank[j].time + num_overflows*0x100000);

    // add leading edge hit information to the output bank 
    if (caen_bank[j].edge == 1) {

      // double size of array if necessary, copying old entries
      if(output_pointer >= *output_bank_size) {
        double_stream_size(output_bank_handle, output_bank_size);
        output_bank = *output_bank_handle;
      }

      output_bank[output_pointer].time = caen_bank_abstime[j];
      output_bank[output_pointer].parameter = 
        caen_wiring_map[caen_bank[j].channel][caen];
      output_pointer++;
    }

    // add trailing edge hit information to the caen_trailing_hits array
    // (after performing a caen_trailing_hits array boundary check)
    if (caen_bank[j].edge == 0) {
      if (*num_caen_trailing_hits < max_caen_trailing_hits) {
	caen_trailing_hits[*num_caen_trailing_hits].time = caen_bank_abstime[j];
	caen_trailing_hits[*num_caen_trailing_hits].caen = caen;
	(*num_caen_trailing_hits)++;
      } else if (!(*bad_block)) {
	fprintf(stderr, "Exceeded trailing edge data stream limit in block %d CAEN%d data.\n", 
		block_number, caen);
	hCaenErrorTrailingEdgeStreamLimit[caen]->Fill(1);
	*bad_block = true;
      }
    }

  } // end loop over caen_bank

  // sort the output bank into time order
  sort_hits(output_pointer, output_bank);

  // fill diagnostic histograms
  double lastTime = -1;
  if(output_pointer != 0) {
    lastTime = output_bank[output_pointer-1].time;
  }
  hCaenLastTime[caen]->SetBinContent(block_number, lastTime);

  return output_pointer;
}

/*-- subroutine ----------------------------------------------------*/

void init_caen_histograms(int caen) 
{
  // This subroutine initializes all caen-specific histograms as needed.
  char name[80], title[80];
  
  sprintf(name, "DAQ_caen%d_edges_leading_by_channel", caen);
  sprintf(title, "CAEN %d leading edges vs. channel", caen);
  hCaenLeadingEdgesByChannel[caen] = 
    new TH1D(name, title, kNumCaenChannels, -0.5, (kNumCaenChannels-0.5));

  sprintf(name, "DAQ_caen%d_edges_leading_by_block", caen);
  sprintf(title, "CAEN %d leading edges vs. block number", caen);
  hCaenLeadingEdgesByBlock[caen] =
    new TH1D(name, title, kMaxEvents, 0.5, kMaxEvents+0.5);

  sprintf(name, "DAQ_caen%d_edges_leading_32BitInterp", caen);
  sprintf(title, "CAEN %d leading edges' 32-bit Interpolator Dist", caen); 
  hCaenLeadingEdges32BitInterpolator[caen] = 
    new TH1D(name, title, 32, -0.5, 31.5);

  sprintf(name, "DAQ_caen%d_edges_leading_32BitInterp_noRollover", caen);
  sprintf(title, "CAEN %d leading edges' 32-bit Interpolator Dist, No Rollover", caen); 
  hCaenLeadingEdges32BitInterpolatorNoRollover[caen] = 
    new TH1D(name, title, 32, -0.5, 31.5);

  sprintf(name, "DAQ_caen%d_edges_trailing_by_channel", caen);
  sprintf(title, "CAEN %d trailing edges vs. channel", caen);
  hCaenTrailingEdgesByChannel[caen] = 
    new TH1D(name, title, kNumCaenChannels, -0.5, (kNumCaenChannels-0.5));

  sprintf(name, "DAQ_caen%d_edges_trailing_by_block", caen);
  sprintf(title, "CAEN %d trailing edges vs. block number", caen);
  hCaenTrailingEdgesByBlock[caen] =
    new TH1D(name, title, kMaxEvents, 0.5, kMaxEvents+0.5);

  sprintf(name, "DAQ_caen%d_error_DAQ", caen);
  sprintf(title, "Blocks with DAQ-inserted CAEN %d errors", caen);
  hCaenErrorDAQ[caen] = 
    new TH1D(name, title, 3, -0.5, 2.5);
  sprintf(name, "DAQ_caen%d_error_DAQ_by_TDC", caen);
  sprintf(title, "CAEN %d DAQ-inserted errors: TDC vs. block number", caen);
  hCaenErrorDAQ_TDCInfo[caen] = 
    new TH2D(name, title, kMaxEvents, 0.5, kMaxEvents+0.5, 4, -0.5, 3.5);

  sprintf(name, "DAQ_caen%d_error_EOB", caen);
  sprintf(title, "Blocks with CAEN %d EOB error flag", caen);
  hCaenErrorEOB[caen] = 
    new TH1D(name, title, 3, -0.5, 2.5);

  sprintf(name, "DAQ_caen%d_error_trailing_edge_stream_limit", caen);
  sprintf(title, "Blocks where CAEN %d trailing edges exceeded stream limit", caen);
  hCaenErrorTrailingEdgeStreamLimit[caen] = 
    new TH1D(name, title, 3, -0.5, 2.5);

  sprintf(name, "DAQ_caen%d_error_trailing_edge_block_limit", caen);
  sprintf(title, "Blocks with too many CAEN %d trailing edges", caen);
  hCaenErrorTrailingEdgeBlockLimit[caen] = 
    new TH1D(name, title, 3, -0.5, 2.5);

  sprintf(name, "DAQ_caen%d_last_time", caen);
  sprintf(title, "CAEN %d last time vs. block number", caen);
  hCaenLastTime[caen] = 
    new TH1D(name, title, kMaxEvents, 0.5, kMaxEvents+0.5);
  
  caen_histograms_inited[caen] = true;
}

/*-- subroutine ----------------------------------------------------*/

INT make_comp(int comp, void *pevent, channel_hit **output_bank_handle,
		int *output_bank_size, bool *bad_block)
{
  channel_hit *output_bank = *output_bank_handle;
  int output_pointer = 0;
  int block_number = ((EVENT_HEADER *) pevent - 1)->serial_number;

  // Open COMP bank
  comp_word *comp_bank;
  char bank_name[8];
  sprintf(bank_name, "CMP%d", comp);
  int comp_bank_size = bk_locate(pevent, bank_name, (DWORD *) &comp_bank);
  if(comp_bank == NULL) {
    return 0;
  }

  // Initialize comp-specific histograms if necessary.
  if (!comp_histograms_inited[comp]) {
    init_comp_histograms(comp);
  }

  int prev_time[kMaxCompFpgas];
  for(int i = 0; i < kMaxCompFpgas; i++) {
    prev_time[i] = 0;
  }

  int num_backwards = 0;
  bool gone_backwards_here[comp_bank_size];
  for(int i = 0; i < comp_bank_size; i++) {
    gone_backwards_here[i] = false;
  }
  
  for(int i = 0; i < comp_bank_size; i++) {
    int fpga = comp_bank[i].r1.fpga;
    int time = comp_bank[i].r1.time;
    int a = comp_bank[i].r1.a;
    int b = comp_bank[i].r1.b;
    
    // check for a backward-going time or an impossible A/B combination
    if((fpga >= kMaxCompFpgas) || (time < prev_time[fpga]) || (!a && !b)) {
      // printf("Time order error at comp %d block %d word %d (prev_time = 0x%x)\n", comp, block_number, i, prev_time[fpga]);
      num_backwards++;

      // According to Rene's note, this condition is probably caused by a 
      // single missing data word that throws off the synchronization.
      // We should try going back one word, but we need to avoid the 
      // possibility of an infinite loop.
      
      if(!gone_backwards_here[i]) {
	gone_backwards_here[i] = true;
	i -= 2;
      }

      continue;
    }
   
    prev_time[fpga] = time;

    // deal with word A if it is present 
    if(a) {
      i++;
      unsigned int word = comp_bank[i].word;
      for(int j = 0; j < 32; j++) {
	if(word & (1 << j)) {

          // double size of array if necessary, copying old entries
          if(output_pointer >= *output_bank_size) {
            double_stream_size(output_bank_handle, output_bank_size);
            output_bank = *output_bank_handle;
          }

	  output_bank[output_pointer].time = comp_clock_period[comp] * time;
	  output_bank[output_pointer].parameter = 
	      comp_wiring_map[j][fpga][comp];
	  output_pointer++;
	}
      }
    }
    
    // deal with word B if it is present 
    if(b) {
      i++;
      unsigned int word = comp_bank[i].word;
      for(int j = 0; j < 32; j++) {
	if(word & (1 << j)) {
          // double size of array if necessary, copying old entries
          if(output_pointer >= *output_bank_size) {
            double_stream_size(output_bank_handle, output_bank_size);
            output_bank = *output_bank_handle;
          }

	  output_bank[output_pointer].time = comp_clock_period[comp] * time;
	  output_bank[output_pointer].parameter = 
	    comp_wiring_map[j + 32][fpga][comp];
	  output_pointer++;
	}
      }
    }
  } 

  // check if there were too many time-ordering errors
//  if (num_backwards > 0 && num_backwards <= 1000) {
  if (num_backwards > 0 && num_backwards <= 10) {
    fprintf(stderr, "Encountered %d backward time steps in block %d COMP%d data.\n",
	    num_backwards, block_number, comp);
  }
  //else if (num_backwards > 1000) {
 else if (num_backwards > 10) {
    fprintf(stderr, "Encountered too many (%d) backward time steps in block %d COMP%d data: skipping block.\n",
	    num_backwards, block_number, comp);
    if (!(*bad_block)) {
      hCompBackwardsLimit[comp]->Fill(1);
      *bad_block = true;
    }
  }

  // sort the output into time order
  sort_hits(output_pointer, output_bank);
  
  // fill diagnostic histograms
  hCompBackwards[comp]->SetBinContent(block_number, num_backwards);
  //if (output_pointer > 0){
  hCompLastTime[comp]->
    SetBinContent(block_number, output_bank[output_pointer-1].time);
  //}
  return output_pointer;
}

/*-- subroutine ----------------------------------------------------*/

void init_comp_histograms(int comp)
{
  // This subroutine initializes all comp-specific histograms as needed.
    char name[80], title[80];

    sprintf(name, "DAQ_comp%d_error_timing_per_block", comp);
    sprintf(title, "COMP %d timing errors vs. block number", comp);
    hCompBackwards[comp] = 
      new TH1D(name, title, kMaxEvents, 0.5, kMaxEvents+0.5);

    sprintf(name, "DAQ_comp%d_error_timing_limit", comp);
    sprintf(title, "Blocks where COMP %d backwards steps exceeded limit", comp);
    hCompBackwardsLimit[comp] =
      new TH1D(name, title, 3, -0.5, 2.5);

    sprintf(name, "DAQ_comp%d_error_stream_limit", comp);
    sprintf(title, "Blocks where COMP %d hits exceeded stream limit", comp);
    hCompErrorStreamLimit[comp] =
      new TH1D(name, title, 3, -0.5, 2.5);

    sprintf(name, "DAQ_comp%d_last_time", comp);
    sprintf(title, "COMP %d last time vs. block number", comp);
    hCompLastTime[comp] = 
      new TH1D(name, title, kMaxEvents, 0.5, kMaxEvents+0.5);

    comp_histograms_inited[comp] = true;
}

/*------------------------------------------------------------------*/
