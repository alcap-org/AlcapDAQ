//////////////////////////////////////////////////////////////////////
// 
// New Louvain/Berkeley Flash ADC compression
// 
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "midas.h"

#include "mucap_compress.h"
#include "mucap_structures.h"

BOOL should_compress_n2fadc_tpc = TRUE;

const int num_n2fadc_tpc_channels = 4;

void n2fadc_tpc_dump(int num_fadc_words, unsigned char *data);

void n2fadc_tpc_optimize()
{
}

void n2fadc_tpc_load()
{
  int size = sizeof(BOOL);
  db_get_value(hDB, 0, "/Compression/Lossless/NFADC/Enabled", &should_compress_n2fadc_tpc,
    &size, TID_BOOL, TRUE);
}

int encode_n2fadc_tpc(unsigned char * input, int input_size, io_buffer * output, int boardNumber)
{
  int num_words = input_size/9;

  // first deal with the timestamps
  int run_length = 0;
  int last_timestamp = -2;
  int island_timestamp = -1;
  int last_island_timestamp = 0;
  for(int j = 0; j < num_words; j++) {
    int timestamp =  (input[j*9+0] << 12) |
                     (input[j*9+1] << 4) |
                     (input[j*9+2] >> 4);

    if(timestamp != last_timestamp + 1 || run_length == 255) {
      if(island_timestamp != -1) {
        io_buffer_put(output, island_timestamp, 20); 
        io_buffer_put(output, run_length, 8);
      }

      island_timestamp = timestamp;
      run_length = 0;
    } 

    last_timestamp = timestamp;
    run_length++;
  }

  // final run
  if(island_timestamp != -1) {
    io_buffer_put(output, island_timestamp, 20); 
    io_buffer_put(output, run_length, 8);
  }

  // now the FADC codes
  int lastSample = 0;
  for(int j = 0; j < num_words; j++) {
    int sample[4];

    bool overflowB0 = ((input[j*9+2] & 0x08) != 0);
    bool overflowA0 = ((input[j*9+2] & 0x04) != 0);
    bool overflowB1 = ((input[j*9+2] & 0x02) != 0);
    bool overflowA1 = ((input[j*9+2] & 0x01) != 0);
    sample[3] = (overflowB0 << 12) |
                     (input[j*9+3] << 4) |
                     (input[j*9+4] >> 4);
    sample[2] = (overflowA0 << 12) |
                       ((input[j*9+4] & 0xf) << 8) |
                       (input[j*9+5]);
    sample[1] = (overflowB1 << 12) |
                     (input[j*9+6] << 4) |
                     (input[j*9+7] >> 4);
    sample[0] = (overflowA1 << 12) |
                       ((input[j*9+7] & 0xf) << 8) |
                       (input[j*9+8]);

    for(int k = 0; k < 4; k++) {
      int diff = sample[k] - lastSample;
      lastSample = sample[k]; 


      if(diff <= 7 && diff >= -7) { 
        io_buffer_put(output, diff+8, 4);
      } else {
        io_buffer_put(output, 0, 4);
        io_buffer_put(output, sample[k], 13);
      }
    } 
  }

  return flush_output_buffer(output);
}

int n2fadc_tpc_compress(unsigned char *input, int input_size, unsigned char *output, int userParam)
{
  io_buffer output_buffer;

  // store size of input data
  int *uncompressed_size_p = (int *) output;
  *uncompressed_size_p = input_size;
  output += sizeof(int);

  // compress samples
  start_output_buffer(&output_buffer, output);
  int compressed_size = encode_n2fadc_tpc(input, input_size, &output_buffer, userParam);
  output += compressed_size;

  return compressed_size + sizeof(int);
}

int decode_n2fadc_tpc(io_buffer * input, unsigned char * output, int input_size, int boardNumber)
{
  int num_fadc_words = input_size/9;

  // first retrieve the timestamps
  int j = 0;
  while(j < num_fadc_words) {

    int timestamp = io_buffer_get(input, 20);
    int run_length = io_buffer_get(input, 8);

    for(int k = 0; k < run_length; k++) {
      int t2 = timestamp + k;
      output[(j+k)*9+0] = (t2 >> 12) & 0xff;
      output[(j+k)*9+1] = (t2 >> 4) & 0xff;
      output[(j+k)*9+2] = (t2 & 0xf) << 4;
    }
    j += run_length; 

  }

  // now the FADC codes
  int lastSample = 0;
  for(int j = 0; j < num_fadc_words; j++) {
    int sample[4];
    for(int k = 0; k < 4; k++) {
      int encodedDiff = io_buffer_get(input, 4) & 0xf;
      if(encodedDiff == 0)  {
        sample[k] = io_buffer_get(input, 13) & 0x1fff;
      } else {
        sample[k] = lastSample + encodedDiff - 8;
      }
      lastSample = sample[k];
    } 

    int overflows = ((sample[0] & 0x1000) ? 0x01 : 0) |
                    ((sample[1] & 0x1000) ? 0x02 : 0) |
                    ((sample[2] & 0x1000) ? 0x04 : 0) |
                    ((sample[3] & 0x1000) ? 0x08 : 0);
    output[j*9+2] |= overflows;
    output[j*9+3] = (sample[3] >> 4) & 0xff;
    output[j*9+4] = ((sample[3] & 0xf) << 4) | ((sample[2] >> 8) & 0xf);
    output[j*9+5] = (sample[2] & 0xff);
    output[j*9+6] = (sample[1] >> 4) & 0xff;
    output[j*9+7] = ((sample[1] & 0xf) << 4) | ((sample[0] >> 8) & 0xf);
    output[j*9+8] = (sample[0] & 0xff);
  }

  return input_size;
}

int n2fadc_tpc_expand(unsigned char *input, int input_size, unsigned char * output, int userParam)
{
  // 
  // Uncompress the data produced by n2fadc_tpc_compress().
  // 

  // Get uncompressed data size
  int *uncompressed_size_p = (int *) input;
  int uncompressed_size = *uncompressed_size_p;
  input += sizeof(int);
  input_size -= sizeof(int);

  // Uncompress data
  io_buffer input_buffer;
  start_input_buffer(&input_buffer, input);
  decode_n2fadc_tpc(&input_buffer, output, uncompressed_size, userParam);

  return uncompressed_size; 
}

void n2fadc_tpc_dump(int num_fadc_words, unsigned char *data)
{
  for(int i = 0; i < num_fadc_words; i++) {
    printf("%d: ", i);
    for(int j = 0; j < 9; j++) {
      printf("%02x ", data[i*9+j]);
    }
    printf("\n");
  }
}
